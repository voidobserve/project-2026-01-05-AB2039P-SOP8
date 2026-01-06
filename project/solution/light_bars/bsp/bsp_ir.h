/**
******************************************************************************************************************************
*
*@file bsp_ir.h
*
*@brief Header file for user IR
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#ifndef _BSP_IR_H
#define _BSP_IR_H


#ifdef __cplusplus
extern "C" {
#endif

#define IRRX_IO_PORT        GPIOB_REG
#define IRRX_IO_PIN         GPIO_PIN_5

#define IRCLK_IO_PORT       GPIOB_REG
#define IRCLK_IO_PIN        GPIO_PIN_6

void bsp_irrx_decode_init(void);
void bsp_irrx_handler_process(void);
void bsp_irrx_irq(void);

#ifdef __cplusplus
}
#endif


#endif
