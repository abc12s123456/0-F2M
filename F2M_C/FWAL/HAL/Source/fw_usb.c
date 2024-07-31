#include "fw_usb.h"
#include "fw_gpio.h"
#include "fw_delay.h"


void FW_USB_Init(FW_USB_Type *dev)
{
    FW_USB_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    if(dev->EN_Pin)
    {
        if(dev->EN_VL == LEVEL_L)
        {
            FW_GPIO_Init(dev->EN_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
            
            FW_GPIO_SET(dev->EN_Pin);
            FW_Delay_Ms(100);
            FW_GPIO_CLR(dev->EN_Pin);
        }
        else
        {
            FW_GPIO_Init(dev->EN_Pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
            
            FW_GPIO_CLR(dev->EN_Pin);
            FW_Delay_Ms(100);
            FW_GPIO_SET(dev->EN_Pin);
        }
        
        dev->Connect_Flag = True;
    }
    
    drv->Init(dev);
}








