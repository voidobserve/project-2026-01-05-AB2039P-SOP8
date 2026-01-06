#include "include.h"
#include "driver_saradc.h"
#include "usb_audio.h"
#include "usb_com.h"
#include "usb_audio_desc.h"
#include "bsp_audio_dec.h"

#if USB_AUDIO_EN

#if VC_RM_DG_ADPCM_DEC_EN
#define RING_BUF_LEN           (256*4)  //ADPCM_DEC_OBUF_LEN * 2 * 2
#elif VC_RM_DG_mSBC_DEC_EN
#define RING_BUF_LEN           (120*4)  //mSBC_DEC_OBUF_LEN * 2 * 2
#elif VC_RM_DG_SBC_DEC_EN
#define RING_BUF_LEN           (96*4)   //SBC_DEC_OBUF_LEN * 2 * 2
#else
#define RING_BUF_LEN           (32*10)
#endif

uda_t uda_0 AT(.usb_buf.usb);

typedef struct {
    ring_buf_t ring_buf;
    s16 speed;
} au_cbuf_cb_t;

u8 usbmic_compli_xp_sys = 0;

#if USB_MIC_EN

AT(.usbdev.com.table)
static const u8 mic_vol_tbl[3] = {
    0x00,//min
    0x00,//max
    0x01,//res
};

AT(.usbdev.com.table)
static const u8 mic_vol_tbl1[3] = {
    0xf9,//min
    0x1e,//max
    0x50,//res
};

epcb_t epcb2_tx AT(.usb_buf.usb);
u8 ep2_isoc_in[AUDIO_ISOIN_SIZE + 2];
u8 ude_micbuf[RING_BUF_LEN] AT(.usb_buf.aubuf.mic);
au_cbuf_cb_t aumic_cb AT(.usb_buf.aubuf.mic);
#endif

void ude_isoc_tx_process(void);
void uda_run_loop_execute(void);

#if USB_MIC_EN
AT(.usbdev.com)
void uda_set_isocin_flag(u8 val)
{
    uda_t *uda = &uda_0;
    uda->flag_isocin_int = val;
}

AT(.usbdev.com)
uint8_t uda_get_isocin_flag(void)
{
    uda_t *uda = &uda_0;
    return uda->flag_isocin_int;
}

AT(.text.func.usbdev)
void uda_mic_start_do(void)
{
    /* Start mic */
    printf("[UAC] mic start\n");

#if BSP_SARADC_MIC_EN
    bsp_saradc_mic_start();
#endif
}

AT(.text.func.usbdev)
void uda_mic_stop_do(void)
{
    /* Sotp mic */

    printf("[UAC] mic stop\n");

#if BSP_SARADC_MIC_EN
    bsp_saradc_mic_stop();
#endif
}

// API for upper layer
AT(.usbdev.com)
void usb_audio_mic_stream(void *ibuf, u32 sample)
{
    bool active_tx_flag = 0;
    uint32_t ret = 0;
    if (aumic_cb.ring_buf.data_len < AUDIO_ISOIN_SIZE) {
        active_tx_flag = 1;
    }

    ret = ring_buf_write(&aumic_cb.ring_buf, ibuf, (sample << 1));

    if (active_tx_flag && ret) {
        ude_isoc_tx_process();
    }
}

AT(.usbdev.com)
static void uda_mic_stop(uda_t *uda)
{
    printf("%s\n", __func__);

#if BSP_SARADC_MIC_EN
    uda_mic_stop_do();
#endif

    uda->flag_isocin_en = 0;

#if LE_VOICE_REMOTE_DONGLE_EN
    bsp_audio_dec_deinit();
#endif //LE_VOICE_REMOTE_DONGLE_EN
}

AT(.usbdev.com)
static void uda_mic_start(uda_t *uda)
{
    printf("%s\n", __func__);

    uda->flag_isocin_en = 1;

    memset(uda->isoc_in->buf, 0, AUDIO_ISOIN_SIZE);

    if (usb_ep_transfer(uda->isoc_in)) {
        usb_ep_start_transfer(uda->isoc_in, AUDIO_ISOIN_SIZE);
    }

    memset(&aumic_cb, 0, sizeof(aumic_cb));
    ring_buf_init(&aumic_cb.ring_buf, ude_micbuf, RING_BUF_LEN);

#if BSP_SARADC_MIC_EN
    uda_mic_start_do();
#endif

#if LE_VOICE_REMOTE_DONGLE_EN
    bsp_audio_dec_init();
#endif //LE_VOICE_REMOTE_DONGLE_EN
}


///USB AUDIO iso in process(mic)
AT(.usbdev.com)
void ude_isoc_tx_process(void)
{
    int mlen;
    au_cbuf_cb_t *s = &aumic_cb;
    uda_t *uda = &uda_0;
    u16 frame_size = AUDIO_ISOIN_SIZE;

    if (usb_ep_transfer(uda->isoc_in)) {
        mlen = frame_size + s->speed;
        if (!ring_buf_read(&s->ring_buf, uda->isoc_in->buf, mlen)) {
            //no data
            return;
        }
        if (uda->mic_mute) {
            memset(uda->isoc_in->buf, 0, mlen);
        }
        usb_ep_start_transfer(uda->isoc_in, mlen);
    }
}

AT(.usbdev.com)
bool uda_get_mic_mute(void)
{
    u8 buf = uda_0.mic_mute;

    return usb_ep0_transfer(&buf, 1);
}

AT(.usbdev.com)
void uda_set_mic_mute(u8 val)
{
    uda_0.mic_mute = val;
    printf("mic set mute: %0d\n", val);
}

AT(.usbdev.com)
void uda_set_mic_volume(u16 val)
{
    printf("mic set volume: %04x\n", val);
    uda_0.mic_vol = val;
}

AT(.usbdev.com)
bool uda_get_mic_volume(u8 req)
{
    u8 buf[2] = {0};
    u8 value;

    if(usbmic_compli_xp_sys) {
        uda_t *uda = &uda_0;
        if (req == UA_GET_CUR) {
            value = uda->mic_vol;
        } else {
            value = mic_vol_tbl[(req&0x7f) - 2];
        }
        buf[0] = value;
        buf[1] = 0x00;
        if (req != UA_GET_RES) {
            value = mic_vol_tbl1[(req&0x7f) - 2];
            buf[1] = value;
        }
        if (req == UA_GET_CUR) {
            buf[0] = 0x00;
            buf[1] = 0x09;
        }
    } else {
        uda_t *uda = &uda_0;
        if (req == UA_GET_CUR) {
            value = uda->mic_vol;
            printf("value4 = %02x\n",value);
        } else {
            value = mic_vol_tbl[(req&0x7f) - 2];
            printf("value5 = %02x\n",value);
        }
        buf[0] = value;
        buf[1] = 0x00;
        if (req != UA_GET_RES) {
            buf[1] = 0x80;
        }
    }

    return usb_ep0_transfer(buf, 2);
}

#endif


void usb_audio_process(void)
{
    uda_run_loop_execute();
}

///USB AUDIO endpoint reset
AT(.usbdev.com)
void usb_isoc_ep_reset(void)
{
    uda_t *uda = &uda_0;

#if USB_MIC_EN
    if (uda->isoc_in->index) {
        usb_ep_reset(uda->isoc_in);
    }
#endif // USB_MIC_EN
}

AT(.usbdev.com)
void uda_run_loop_execute(void)
{
    uda_t *uda = &uda_0;

#if USB_MIC_EN
    if (uda->flag_isocin_en != uda->flag_isocin_int) {
        if (uda->flag_isocin_int) {
            delay_5ms(2);           //delay 10ms启动MIC, 等待SET CUR命令先处理（例如设采样率）
            uda_mic_start(uda);
        } else {
            uda_mic_stop(uda);
        }
    }
#endif // USB_MIC_EN
}

void uda_exit(uda_t *uda)
{
#if USB_MIC_EN
    if (uda->flag_isocin_en) {
        uda_mic_stop(uda);
    }
#endif
}

void uda_init(void )
{
    epcb_t *epcb;

    memset(&uda_0, 0, sizeof(uda_t));

    uda_t *uda = &uda_0;

#if USB_MIC_EN
    uda_0.isoc_in  = &epcb2_tx;
    memset(uda->isoc_in, 0x00, sizeof(epcb_t));
    usb_ep_callback_register(ude_isoc_tx_process, USB_AUDIO_EP_IN_INDEX, EP_DIR_IN);
     //usb audio mic ep init
    memset(ep2_isoc_in,  0, sizeof(ep2_isoc_in));
    epcb = uda->isoc_in;
    epcb->dir = EP_DIR_IN;
    epcb->index = USB_AUDIO_EP_IN_INDEX;
    epcb->type = EP_TYPE_ISOC;
    epcb->epsize = AUDIO_ISOIN_SIZE;
    epcb->buf = ep2_isoc_in;
    epcb->first_pkt = 0;
    epcb->xlen = 0;

    if (usb_ep_get_max_len(epcb->index) < AUDIO_ISOIN_SIZE) {
        printf("--->err: ep size overflow\n");
        while(1);
    }

    usb_ep_init(epcb);

    uda->mic_vol = 0x6a;        //min:0x00, max:0xff
    uda->mic_mute = 0;
    uda->flag_isocin_int = 0;
    uda->flag_isocin_en = 0;
#endif

    printf("uda_init success\n");
}

void usb_audio_init(void)
{
#if BSP_SARADC_MIC_EN
    bsp_saradc_mic_init();
#endif
}

void usb_audio_deinit(void)
{
    uda_t *uda = &uda_0;

    uda_exit(uda);
}

#endif
