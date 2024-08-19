#ifndef _LL_GPIO_H_
#define _LL_GPIO_H_


#ifdef __cplusplus
extenrn "C"{
#endif


#include "fw_type.h"
#include "ll_include.h"


#define GPIOx(pin)             ((GPIO_TypeDef *)(GPIOA_BASE + (((pin >> 8) - 1) << 10)))
#define GPIO_Pin_x(pin)        (1 << (pin & 0xFF))
#define GPIO_PinSource_x(pin)  (pin & 0xFF)


void LL_GPIO_PinAFConfig(u16 pin, u8 GPIO_AF);


#ifdef __cplusplus
}
#endif


#endif
