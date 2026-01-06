#include "include.h"
#include "bsp_ir_tx.h"
#include "driver_ir.h"
#include "driver_gpio.h"

#if (BSP_IR_TX_EN)

AT(.ir.tx)
static uint8_t irtx_enc_buf[150];  //nec 134byte

static uint16_t bsp_irtx_nec_txdata_encode(uint8_t *dest, uint8_t address, uint8_t cmd)
{
    uint16_t cnt = 0;
    uint16_t offset = 0;

    //logical '0'  '1'
    uint16_t logical_0[2], logical_1[2]; //unit: 1us
    logical_0[0] = 0x8000|0x0233;  //563*1=563us   2byte表示一个电平，其中bit[15] 1：高电平； 0：低电平； 后面15bit表示持续时间
    logical_0[1] = 0x0000|0x0233;  //563*1=563us
    logical_1[0] = 0x8000|0x8233;  //563*1=563us
    logical_1[1] = 0x0000|0x0698;  //1688*1=1.688ms

    //boot code
    uint16_t boot_code[2];
    boot_code[0] = 0x8000|0x2328;  //9000*1=9ms
    boot_code[1] = 0x0000|0x0fa0;  //4000*1=4ms
    memcpy(&dest[offset], &boot_code, sizeof(boot_code));
    offset += sizeof(boot_code);
    //8 bit address code
    for(cnt = 0; cnt < 8; cnt++) {
        if((address >> cnt) & 0x01) {
            memcpy(&dest[offset], &logical_1, sizeof(logical_1));
            offset += sizeof(logical_1);
        } else {
            memcpy(&dest[offset], &logical_0, sizeof(logical_0));
            offset += sizeof(logical_0);
        }
    }

    //8 bit address inverse code
    for(cnt = 0; cnt < 8; cnt++) {
        if((address >> cnt) & 0x01) {
            memcpy(&dest[offset], &logical_0, sizeof(logical_0));
            offset += sizeof(logical_0);
        } else {
            memcpy(&dest[offset], &logical_1, sizeof(logical_1));
            offset += sizeof(logical_0);
        }
    }

    //command code
    for(cnt = 0; cnt < 8; cnt++) {
        if((cmd >> cnt) & 0x01) {
            memcpy(&dest[offset], &logical_1, sizeof(logical_1));
            offset += sizeof(logical_1);
        } else {
            memcpy(&dest[offset], &logical_0, sizeof(logical_0));
            offset += sizeof(logical_0);
        }
    }

    //command inverse code
    for(cnt = 0; cnt < 8; cnt++) {
        if((cmd >> cnt) & 0x01) {
            memcpy(&dest[offset], &logical_0, sizeof(logical_0));
            offset += sizeof(logical_0);
        } else {
            memcpy(&dest[offset], &logical_1, sizeof(logical_1));
            offset += sizeof(logical_1);
        }
    }

    //end code 563us high
    uint16_t end_code = 0x8000|0x0233;  //563*1=563us
    memcpy(&dest[offset], &end_code, sizeof(end_code));
    offset += sizeof(end_code);

    return offset;
}

static uint16_t bsp_irtx_nec_repeat_txdata_encode(uint8_t *dest)
{
    uint16_t offset = 0;
    //repeat code
    uint16_t repeat_code[2];
    repeat_code[0] = 0x8000|0x2328;  //9000*1=9ms
    repeat_code[1] = 0x0000|0x08ca;  //2250*1=2.25ms
    memcpy(&dest[offset], &repeat_code, sizeof(repeat_code));
    offset += sizeof(repeat_code);

    //end code 563us high
    uint16_t end_code = 0x8000|0x0233;  //563*1=563us
    memcpy(&dest[offset], &end_code, sizeof(end_code));
    offset += sizeof(end_code);

    return offset;
}

static void bsp_irtx_io_init(gpio_typedef* gpiox, uint32_t gpio_pin)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = gpio_pin;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;

    gpio_init(gpiox, &gpio_init_structure);

    gpio_func_mapping_config(gpiox, gpio_pin, GPIO_CROSSBAR_OUT_IRTX);
}

void bsp_irtx_init(gpio_typedef* gpiox, uint32_t gpio_pin)
{
    irtx_param_t irtx_param;

    clk_gate1_cmd(CLK_GATE1_IRTX, CLK_EN);
    clk_irtx_clk_set(CLK_IR_CLK_X24M_CLKDIV24);

    bsp_irtx_io_init(gpiox, gpio_pin);

    irtx_param.invert_en = IRTX_INV_DIS;
    irtx_param.carrier_config.carrier_en = IRTX_MOD_EN;
    irtx_param.carrier_config.mod_clksel = IRTX_MOD_CLKSEL_3M;
    irtx_param.carrier_config.ircw_duty = 79 / 3;  //1/3
    irtx_param.carrier_config.ircw_length = 79-1;  //3M/79=38KHz

    irtx_param.rdma_config.en = IRTX_DMA_MODE_EN;  //DMA一次读取2byte

    irtx_init(&irtx_param);
    irtx_cmd(ENABLE);
}

void bsp_itx_send_data(uint8_t address, uint8_t cmd, bool repeat)
{
    uint16_t len;
    if (repeat) {
        len = bsp_irtx_nec_repeat_txdata_encode(irtx_enc_buf);
    } else {
        len = bsp_irtx_nec_txdata_encode(irtx_enc_buf, address, cmd);
    }

    len /= 2;
    irtx_dma_kick((uint32_t)&irtx_enc_buf, len-1);  //len unit:2byte
    irtx_kick();
}

// void irtx_example(void)
// {
//     static bool irtx_repeat_test_flag = 0;
//     static uint32_t irtx_send_tick = -1;

//     if (tick_check_expire(irtx_send_tick, 110)) {
//         irtx_send_tick = tick_get();

//         if (irtx_repeat_test_flag == 0) {
//             irtx_repeat_test_flag = 1;
//             bsp_itx_send_data(0x4E, 0x1F, 0);
//             printf("irtx\n");
//         } else {
//             bsp_itx_send_data(0x4E, 0x1F, 1);
//             printf("repeat\n");
//         }
//     }
// }

#endif //#if (BSP_IR_TX_EN)
