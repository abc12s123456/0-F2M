#ifndef _LED_H_
#define _LED_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"
#include "stiming.h"


typedef struct LED
{
    FW_Device_Type Device;
    
    void (*Init)(struct LED *dev);
    void (*CTL)(struct LED *dev, u8 state);
    
    u32  Pin : 16;
    u32  Level : 1;
    u32  State : 1;
    
    u32  : 14;
    
    STiming_Type *Timing;
}LED_Type;


void LED_DeInit(LED_Type *dev);
void LED_Init(LED_Type *dev);

void LED_ON(LED_Type *dev);
void LED_OFF(LED_Type *dev);

void LED_Shine(LED_Type *dev, u16 period, u8 times);
void LED_Blink(LED_Type *dev, u16 period);
void LED_Adj_Blink(LED_Type *dev, u16 period, u8 dc);

void LED_Breath(LED_Type *dev, u16 period);
void LED_Exq_Breath(LED_Type *dev, u16 period);


#ifdef __cplusplus
}
#endif

#endif

