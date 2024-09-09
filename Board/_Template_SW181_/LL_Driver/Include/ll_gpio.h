#ifndef _LL_GPIO_H_
#define _LL_GPIO_H_


#ifdef __cplusplus
extenrn "C"{
#endif


#include "fw_type.h"
#include "ll_include.h"


#define GPIOx(pin)                (GPIO_TypeDef *)(APB_BASE + (((pin) & 0xFF00) << 4))
#define GPIO_PORTx(pin)           (PORT_TypeDef *)((((pin) & 0xFF00) == PORTG) ? PORTG_BASE : (APB_BASE + (((pin) >> 4) + 0x90)))
#define GPIO_PINx(pin)            ((pin) & 0xFF)


#ifdef __cplusplus
}
#endif


#endif
