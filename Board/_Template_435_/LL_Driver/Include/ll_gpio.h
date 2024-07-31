#ifndef _LL_GPIO_H_
#define _LL_GPIO_H_


#ifdef __cplusplus
extenrn "C"{
#endif


#include "fw_type.h"
#include "ll_include.h"


#define GPIOx(pin)                (GPIOA + ((((pin) >> 8) - 1) << 12))
#define GPIO_PINx(pin)            (1 << ((pin) & 0xFF))

#define GPIO_PORTx_SOURCE(pin)(\
(GPIOx(pin) == GPIOA) ? GPIO_PORT_SOURCE_GPIOA :\
(GPIOx(pin) == GPIOB) ? GPIO_PORT_SOURCE_GPIOB :\
(GPIOx(pin) == GPIOC) ? GPIO_PORT_SOURCE_GPIOC :\
(GPIOx(pin) == GPIOD) ? GPIO_PORT_SOURCE_GPIOD :\
(GPIOx(pin) == GPIOE) ? GPIO_PORT_SOURCE_GPIOE :\
(GPIOx(pin) == GPIOF) ? GPIO_PORT_SOURCE_GPIOF :\
(GPIOx(pin) == GPIOG) ? GPIO_PORT_SOURCE_GPIOG :\
INVALUE)

#define GPIO_PINx_SOURCE(pin)     ((pin) & 0xFF)


#ifdef __cplusplus
}
#endif


#endif
