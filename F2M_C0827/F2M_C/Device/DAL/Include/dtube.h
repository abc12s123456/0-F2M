#ifndef _DTUBE_H_
#define _DTUBE_H_

#if __cplusplus
extern "C"{
#endif


#include "fw_device.h"


/* Using the same data port as an object */
typedef struct DTube DTube_Type;

    
struct DTube 
{
    FW_Device_Type Device;
    
    u8 *buffer;
    
    u32 Digits_Max : 4;           //single tube's max display digits
    u32 Number : 8;               //tube's num，using the same data as one(eg. hc595)
    
    u32 BC_EN : 1;                //brightness compensation
    
    u32 Lumen : 8;
    u32 Comp_Value : 8;
    
    u32 : 3;
    
    void (*Init)(DTube_Type *dev);
    void (*Port_Write)(DTube_Type *dev, u8 *pdata, u32 num);
    void (*Digit_CTL)(DTube_Type *dev, u8 digit, u8 state);
    
    u8   (*Get_BC)(u8 *buff, u8 num);
    void (*BC_Handle)(DTube_Type *dev, u8 comp_num);
};


void DTube_Init(DTube_Type *dev);

void DTube_Set_BC(DTube_Type *dev, u8 state);

void DTube_Timer_Handler(DTube_Type *dev, u32 period);
void DTube_Clear(DTube_Type *dev);
void DTube_Digit_Off(DTube_Type *dev);

void DTube_Post(DTube_Type *dev);



#if __cplusplus
}
#endif

#endif

