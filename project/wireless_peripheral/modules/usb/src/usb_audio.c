#include "include.h"
#include "driver_saradc.h"
#include "usb_audio.h"
#include "usb_com.h"
#include "usb_audio_desc.h"
#include "usb_usr_audio.h"

#if USB_AUDIO_EN

uda_t uda_0 AT(.usb_buf.usb);

#if USB_MIC_EN

AT(.usbdev.com.table)
static const u8 mic_vol_tbl[3] = {
    0x00,//min
    0x00,//max
    0x01,//res
};

epcb_t epcb2_tx AT(.usb_buf.usb);
u8 ep2_isoc_in[AUDIO_ISOIN_SIZE + 2];
#endif


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

AT(.usbdev.com)
static void uda_mic_stop(uda_t *uda)
{
    uda->flag_isocin_en = 0;

    if (usb_usr_audio_cb.mic_stop) {
        usb_usr_audio_cb.mic_stop();
    }
}

AT(.usbdev.com)
static void uda_mic_start(uda_t *uda)
{
    uda->flag_isocin_en = 1;

    memset(uda->isoc_in->buf, 0, AUDIO_ISOIN_SIZE);

    if (usb_ep_transfer(uda->isoc_in)) {
        usb_ep_start_transfer(uda->isoc_in, AUDIO_ISOIN_SIZE);
    }

    if (usb_usr_audio_cb.mic_start) {
        usb_usr_audio_cb.mic_start();
    }
}

///USB AUDIO iso in process(mic)
AT(.usbdev.com)
void ude_isoc_tx_process(void)
{
    uda_t *uda = &uda_0;
    u16 ansy_frame_size = 0;

    if (usb_ep_transfer(uda->isoc_in)) {
        if (!uda->mic_mute && usb_usr_audio_cb.mic_data_request) {
            ansy_frame_size = usb_usr_audio_cb.mic_data_request(uda->isoc_in->buf, AUDIO_ISOIN_SIZE);
        }

        if (!ansy_frame_size) {
            ansy_frame_size = AUDIO_ISOIN_SIZE;
            memset(uda->isoc_in->buf, 0, ansy_frame_size);
        }

        usb_ep_start_transfer(uda->isoc_in, ansy_frame_size);
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
    
    if (usb_usr_audio_cb.mic_mute_set) {
        usb_usr_audio_cb.mic_mute_set(val);
    }
}

AT(.usbdev.com)
void uda_set_mic_volume(u16 val)
{
    uda_0.mic_vol = val;

    if (usb_usr_audio_cb.mic_vol_set) {
        usb_usr_audio_cb.mic_vol_set(val);
    }
}

AT(.usbdev.com)
bool uda_get_mic_volume(u8 req)
{
    u8 buf[2] = {0};
    u8 value;
    uda_t *uda = &uda_0;

    if (req == UA_GET_CUR) {
        value = uda->mic_vol;
    } else {
        value = mic_vol_tbl[(req&0x7f) - 2];
    }

    buf[0] = value;
    buf[1] = (req != UA_GET_RES)? 0x80: 0x00;

    return usb_ep0_transfer(buf, 2);
}

#endif

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

    if (usb_usr_audio_cb.init) {
        usb_usr_audio_cb.init();
    }
}

void usb_audio_init(void)
{
}

void usb_audio_deinit(void)
{
    uda_t *uda = &uda_0;

    uda_exit(uda);
}

void usb_audio_process(void)
{
    uda_run_loop_execute();
}

#endif
