/**
******************************************************************************************************************************
*
*@file ir_example.h
*
*@brief Header file for IR example
*
*Copyright (c) 2024, BLUETRUM
******************************************************************************************************************************
**/
#ifndef _IR_EXAMPLE_H
#define _IR_EXAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

#define IRRX_IO_PORT        GPIOB_REG
#define IRRX_IO_PIN         GPIO_PIN_6

#define IRTX_IO_PORT        GPIOB_REG
#define IRTX_IO_PIN         GPIO_PIN_5


void irrx_example_init(void);
void irtx_example_init(void);
void irrx_example(void);
void irtx_example(void);

#ifdef __cplusplus
}
#endif

#endif
