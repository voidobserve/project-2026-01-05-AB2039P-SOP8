#include "include.h"
#include "ble_voice.h"
#include "ble_client.h"
#include "bsp_audio_dec.h"

void ble_voice_remote_cmd_proc(uint16_t con_handle, uint16_t attribute_handle, uint8_t *buffer, uint16_t buffer_size)
{
#if VC_RM_DG_ADPCM_DEC_EN
    int i = 0;
    server_info_t *server_info = ble_get_server_info(0);
    static uint8_t ble_adpcm_open_mic_cmd[] = {0x0c, 0x00, 0x01};  //OPEN MIC CMD
    if (attribute_handle == 0x5a) {  //CHAR_CTL
        if (buffer[0] == 0x08) {     //START_SEATCH
            ble_update_conn_param(con_handle, 6, 49, 200);
            while(i < server_info->characteristic_cnt) {
                if(server_info->characteristic[i].value_handle == 0x55) {  //OPEN MIC
                    printf("adpcm voice remote open mic\n");
                    bsp_set_audio_dec_adpcm_en(1);
                    ble_write_req_for_character(server_info->conn_handle, &server_info->characteristic[i], ble_adpcm_open_mic_cmd, sizeof(ble_adpcm_open_mic_cmd));
                    return;
                }
                i++;
            }
        }
    }
#endif //VC_RM_DG_ADPCM_DEC_EN

#if VC_RM_DG_mSBC_DEC_EN
    int i = 0;
    server_info_t *server_info = ble_get_server_info(0);
    static uint8_t ble_msbc_open_mic_cmd[] = {0x00, 0x00, 0x00, 0x80, 0x01};  //OPEN MIC CMD
    if (attribute_handle == 0x42) {  //VOICE EN CTRL
        if (buffer[0] == 0x01) {         //OPEN MIC CMD
            ble_update_conn_param(con_handle, 6, 49, 200);
            while(i < server_info->characteristic_cnt) {
                if(server_info->characteristic[i].value_handle == 0x3f) {  //OPEN MIC
                    printf("msbc voice remote open mic\n");
                    bsp_set_audio_dec_msbc_en(1);
                    ble_write_req_for_character(server_info->conn_handle, &server_info->characteristic[i], ble_msbc_open_mic_cmd, sizeof(ble_msbc_open_mic_cmd));
                    return;
                }
                i++;
            }
        }
    }
#endif //VC_RM_DG_ADPCM_DEC_EN

#if VC_RM_DG_SBC_DEC_EN
    int i = 0;
    server_info_t *server_info = ble_get_server_info(0);
    static uint8_t ble_sbc_open_mic_cmd[] = {0x00, 0x00, 0x00, 0x80, 0x01};  //OPEN MIC CMD
    if (attribute_handle == 0x42) {  //VOICE EN CTRL
        if (buffer[0] == 0x01) {         //OPEN MIC CMD
            ble_update_conn_param(con_handle, 6, 49, 200);
            while(i < server_info->characteristic_cnt) {
                if(server_info->characteristic[i].value_handle == 0x3f) {  //OPEN MIC
                    printf("sbc voice remote open mic\n");
                    bsp_set_audio_dec_sbc_en(1);
                    ble_write_req_for_character(server_info->conn_handle, &server_info->characteristic[i], ble_sbc_open_mic_cmd, sizeof(ble_sbc_open_mic_cmd));
                    return;
                }
                i++;
            }
        }
    }
#endif //VC_RM_DG_ADPCM_DEC_EN
}

void ble_voice_audio_dec_proc(uint16_t con_handle, uint16_t attribute_handle, uint8_t *buffer, uint16_t buffer_size)
{
#if VC_RM_DG_ADPCM_DEC_EN
    //ATV attribute_handle: 0x57 is voice data, 0x5a is sync
    static uint8_t ble_voice_adpcm_data[6+128];
    static uint16_t ble_voice_adpcm_data_offset = 0;
    if (bsp_get_audio_dec_adpcm_en()) {
        if (attribute_handle == 0x57) {
            memcpy(&ble_voice_adpcm_data[ble_voice_adpcm_data_offset], buffer, buffer_size);
            ble_voice_adpcm_data_offset += buffer_size;
        } else if (attribute_handle == 0x5a) {
            ble_voice_adpcm_data_offset = 0;
            bsp_audio_dec_proc(ble_voice_adpcm_data, (6+128));
        }
    }
#endif //VC_RM_DG_ADPCM_DEC_EN

#if VC_RM_DG_mSBC_DEC_EN
    //XM mSBC data: byte2~byte59
    if (bsp_get_audio_dec_msbc_en()) {
        if ((attribute_handle == 0x46) || (attribute_handle == 0x4a) || (attribute_handle == 0x4e)) {
            bsp_audio_dec_proc(&buffer[2], 57);
        }
    }
#endif //VC_RM_DG_ADPCM_DEC_EN

#if VC_RM_DG_SBC_DEC_EN
    //SBC data: byte2~byte54
    //SBC encode config: SBC_FREQ_16000, SBC_BLK_12, SBC_SB_8, SBC_MODE_MONO, SBC_AM_LOUDNESS, 30;
    if (bsp_get_audio_dec_sbc_en()) {
        if ((attribute_handle == 0x46) || (attribute_handle == 0x4a) || (attribute_handle == 0x4e)) {
            bsp_audio_dec_proc(&buffer[2], 54);
        }
    }
#endif //VC_RM_DG_ADPCM_DEC_EN
}
