#include "include.h"
#include "usb_com.h"
#include "usb_audio.h"
#include "usb_audio_desc.h"
#include "usb_usr_audio.h"
#include "api_ring_buf.h"
#include "res.h"

#define TRACE_EN            1
#if TRACE_EN
#define TRACE(...)          my_printf("[UAC] "); \
                            my_printf(__VA_ARGS__)
#define TRACE_R(...)        my_print_r(__VA_ARGS__)
#else
#endif


#define USB_USR_AUDIO_ADPCM_BLOCK                   32
#define USB_USR_AUDIO_CACHE_MIC_PKTS                3
#define USB_USR_AUDIO_CACHE_MIC_POOL                (SAMPLE_2_PCM16_BYTE(ADPCM_BYTE_2_SAMPLE(\
                                                    USB_USR_AUDIO_ADPCM_BLOCK*USB_USR_AUDIO_CACHE_MIC_PKTS)))

#define usb_usr_audio_ring_read(buf, siz)           ring_buf_read(&ring_buf_cb, buf, siz)
#define usb_usr_audio_ring_write(buf, siz)          ring_buf_write(&ring_buf_cb, buf, siz)


static void usb_usr_audio_init(void);
static void usb_usr_audio_mic_start(void);
static void usb_usr_audio_mic_stop(void);
static void usb_usr_audio_mic_mute_set(bool mute);
static void usb_usr_audio_mic_vol_set(uint16_t vol);
static uint32_t usb_usr_audio_mic_data_request(uint8_t *pcm16_data, uint32_t size);

static ring_buf_t ring_buf_cb;
static uint8_t ring_buf_pool[USB_USR_AUDIO_CACHE_MIC_POOL];

static uint8_t uac_decode_cache_pcm16_buf[SAMPLE_2_PCM16_BYTE(ADPCM_BYTE_2_SAMPLE(USB_USR_AUDIO_ADPCM_BLOCK))];
static uint8_t uac_decode_cache_adpcm_buf[USB_USR_AUDIO_ADPCM_BLOCK];

usb_usr_audio_cb_t usb_usr_audio_cb = {
    usb_usr_audio_init,
    usb_usr_audio_mic_start,
    usb_usr_audio_mic_stop,
    usb_usr_audio_mic_mute_set,
    usb_usr_audio_mic_vol_set,
    usb_usr_audio_mic_data_request,
};


static void usb_usr_audio_init(void)
{
    adpcm_sample_idx_reset();
    ring_buf_init(&ring_buf_cb, ring_buf_pool, sizeof(ring_buf_pool));
}

static void usb_usr_audio_mic_start(void)
{
    TRACE("mic start\n");
}

static void usb_usr_audio_mic_stop(void)
{
    TRACE("mic stop");
}

static void usb_usr_audio_mic_mute_set(bool mute)
{
    TRACE("set mute: %d\n", mute);
}

static void usb_usr_audio_mic_vol_set(uint16_t vol)
{
    TRACE("set vol: 0x%x\n", vol);
}

static uint32_t usb_usr_audio_mic_data_request(uint8_t *pcm16_data, uint32_t size)
{
#if 0
    // Dear AB203x User!
    // 
    // In order to verify the fidelity of voice data in the transmission process and
    // exclude the influence of hardware circuit and audio sampling, a verification
    // method is provided here to replace the sampled data with wav file data. The
    // operations you need are as follows:
    //     Apply these code block, and copy "Output/bin/res_bak/test_wav.wav" to "Output/bin/res/test_wav.wav".
    //
    #define TEST_WAV_BLOCK_SIZE         (size)
    static u32 test_wav_offset;
    static u32 test_wav_mute_tick;
    static u16 test_wav_src_ofs;
    void *ptr = pcm16_data;

    if (!test_wav_src_ofs) {
        u8 par1; u32 par2; u16 par3;
        u16 wav_res_analize(u8 *, u8 *, u32 *, u16 *);
        test_wav_src_ofs = wav_res_analize((u8 *)RES_BUF_TEST_WAV_WAV, &par1, &par2, &par3);
    }

    if (test_wav_mute_tick && !tick_check_expire(test_wav_mute_tick, 2000)) {
        memset(ptr, 0x00, TEST_WAV_BLOCK_SIZE);
    } else {
        test_wav_mute_tick = 0;
        memcpy(ptr, (u8 *)(RES_BUF_TEST_WAV_WAV + test_wav_offset), TEST_WAV_BLOCK_SIZE);
        if ((test_wav_offset + TEST_WAV_BLOCK_SIZE) >= (RES_LEN_TEST_WAV_WAV & ~0x1ff)) {
            test_wav_offset = test_wav_src_ofs;
            test_wav_mute_tick = tick_get();
        } else {
            test_wav_offset += TEST_WAV_BLOCK_SIZE;
        }
    }

    return size;
#endif

    return usb_usr_audio_ring_read(pcm16_data, size);
}

/////////////////////////////////////////////////////////////////////////////////////

bool usb_usr_audio_mic_is_start(void)
{
    return (uda_get_isocin_flag());
}

AT(.com_text.usb.uac.dec)
void usb_usr_audio_mic_decode(void)
{
    adpcm_decode_block(uac_decode_cache_pcm16_buf, uac_decode_cache_adpcm_buf, ADPCM_BYTE_2_SAMPLE(USB_USR_AUDIO_ADPCM_BLOCK));

    if (!usb_usr_audio_ring_write(uac_decode_cache_pcm16_buf, sizeof(uac_decode_cache_pcm16_buf))) {
        // TRACE("~overflow~\n");
    }
}

AT(.com_text.usb.uac.stream)
bool usb_usr_audio_mic_write(void *mic_data, uint32_t size)
{
    /* ASSERT(size == USB_USR_AUDIO_ADPCM_BLOCK) */

    if (ude_cb.connected) {
        memcpy(uac_decode_cache_adpcm_buf, mic_data, size);
        thread_driver_user_msg_post(THREAD_DRIVER_MSG_UAC_DEC);
    }

    return true;
}
