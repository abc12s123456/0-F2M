/*
 * F2M
 * Copyright (C) 2024 abc12s123456 382797263@qq.com.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://github.com/abc12s123456/F2M
 *
 */
#ifndef _FW_MOD_CONF_H_
#define _FW_MOD_CONF_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"
#include "project_mod_conf.h"


#ifndef SYSTEM_MOD_EN
#define SYSTEM_MOD_EN        ON
#endif

#ifndef DELAY_MOD_EN
#define DELAY_MOD_EN         ON
#endif

#ifndef GPIO_MOD_EN
#define GPIO_MOD_EN          ON
#endif

#ifndef ADC_MOD_EN
#define ADC_MOD_EN           OFF
#endif

#ifndef BKP_MOD_EN
#define BKP_MOD_EN           OFF
#endif

#ifndef CAN_MOD_EN
#define CAN_MOD_EN           OFF
#endif

#ifndef DAC_MOD_EN
#define DAC_MOD_EN           OFF
#endif

#ifndef EXTI_MOD_EN
#define EXTI_MOD_EN          OFF
#endif

#ifndef OS_MOD_EN
#define OS_MOD_EN            OFF
#endif

#ifndef PWR_MOD_EN
#define PWR_MOD_EN           OFF
#endif

#ifndef RTC_MOD_EN
#define RTC_MOD_EN           OFF
#endif

#ifndef UART_MOD_EN
#define UART_MOD_EN          OFF
#endif

#ifndef SUART_MOD_EN
#define SUART_MOD_EN         OFF
#endif

#if defined(SUART_MOD_EN) && SUART_MOD_EN
#if !defined(UART_MOD_EN) && !UART_MOD_EN
#error "The UART_MOD_EN is undefined or not 'ON'"
#endif
#endif

#ifndef FLASH_MOD_EN
#define FLASH_MOD_EN         OFF
#endif

#ifndef WDG_MOD_EN
#define WDG_MOD_EN           OFF
#endif

#ifndef S2RAM_MOD_EN
#define S2RAM_MOD_EN         OFF
#endif

#ifndef SPI_MOD_EN
#define SPI_MOD_EN           OFF
#endif

#ifndef SSPI_MOD_EN
#define SSPI_MOD_EN          OFF
#endif

#if defined(SSPI_MOD_EN) && SSPI_MOD_EN
#if !defined(SPI_MOD_EN) || !SPI_MOD_EN
#error "The SPI_MOD_EN is undefined or not 'ON'"
#endif
#endif

#ifndef I2C_MOD_EN
#define I2C_MOD_EN           OFF
#endif

#ifndef SI2C_MOD_EN
#define SI2C_MOD_EN          OFF
#endif

#if defined(SI2C_MOD_EN) && SI2C_MOD_EN
#if !defined(I2C_MOD_EN) || !I2C_MOD_EN
#error "The I2C_MOD_EN is undefined or not 'ON'"
#endif
#endif

#ifndef TIM_MOD_EN
#define TIM_MOD_EN           OFF
#endif

#if defined(DELAY_MOD_EN) && DELAY_MOD_EN &&\
    defined(GPIO_MOD_EN) && GPIO_MOD_EN
#ifndef SBUS_MOD_EN
#define SBUS_MOD_EN          OFF
#endif

#ifndef OW_MOD_EN
#define OW_MOD_EN            OFF
#endif
#endif

#ifndef USB_MOD_EN
#define USB_MOD_EN           OFF
#endif


#ifdef __cplusplus
}
#endif

#endif

