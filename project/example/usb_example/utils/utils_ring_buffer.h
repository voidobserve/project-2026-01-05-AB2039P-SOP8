#ifndef _UTILS_RING_BUFFER_H
#define _UTILS_RING_BUFFER_H


typedef struct{
    uint8_t *buf;
    uint32_t buf_read;           //read位置
    uint32_t buf_write;          //write位置
    uint32_t data_len;           //可使用的数据长度
    uint32_t total_len;          //ringbuf数据长度
} ring_buffer_t;


void utils_ring_buffer_init(ring_buffer_t *ring_buf, uint8_t *buffer, uint32_t size);
void utils_ring_buffer_clear(ring_buffer_t *ring_buf);
uint32_t utils_ring_buffer_write(ring_buffer_t *ring_buf, uint8_t *buffer, uint32_t size);
uint32_t utils_ring_buffer_read(ring_buffer_t *ring_buf, uint8_t *buffer, uint32_t size);
uint32_t utils_ring_buffer_data_len_get(ring_buffer_t *ring_buf);
uint8_t* utils_ring_buffer_get(ring_buffer_t *ring_buf, uint32_t size);

#endif // _UTILS_RING_BUFFER_H
