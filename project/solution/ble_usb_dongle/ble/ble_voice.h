#ifndef __BLE_VOICE_H
#define __BLE_VOICE_H

void ble_voice_remote_cmd_proc(uint16_t con_handle, uint16_t attribute_handle, uint8_t *buffer, uint16_t buffer_size);
void ble_voice_audio_dec_proc(uint16_t con_handle, uint16_t attribute_handle, uint8_t *buffer, uint16_t buffer_size);
#endif //__BLE_VOICE_H
