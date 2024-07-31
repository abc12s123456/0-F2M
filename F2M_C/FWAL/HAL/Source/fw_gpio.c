#include "fw_gpio.h"
#include "fw_debug.h"
#include "fw_print.h"


/* MCU原生GPIO */
FW_GPIO_Type GPIO;


__INLINE_STATIC_ void FW_GPIO_ToggleUseGetOutput(FW_GPIO_Type *dev, u16 pin);
__INLINE_STATIC_ void FW_GPIO_ToggleUseLAPI(FW_GPIO_Type *dev, u16 pin);


static void FW_GPIO_Driver_Probe(void *dev)
{
    FW_GPIO_Driver_Type **drv = (FW_GPIO_Driver_Type **)(&GPIO.Device.Driver);
    
    *drv = FW_Driver_Find("ll->gpio");
    if(*drv == NULL)
    {
        /* native gpio driver is not instanced */
        while(1);
    }
    
    /* 实例化MCU自带GPIO的Pin_Toggle */
    if((*drv)->Pin_Toggle)  GPIO.Pin_Toggle = FW_GPIO_ToggleUseLAPI;
    else  if((*drv)->Pin_Toggle)  GPIO.Pin_Toggle = FW_GPIO_ToggleUseGetOutput;
    else  GPIO.Pin_Toggle = NULL;
}
FW_DEVICE_STATIC_REGIST("gpio", &GPIO, FW_GPIO_Driver_Probe, GPIO);


void GPIO_PinDeInit(FW_GPIO_Type *dev, u16 pin)
{
    if(pin == PIN_NULL)  return;
    
    FW_ASSERT(dev);
    
    FW_GPIO_Driver_Type *drv = (FW_GPIO_Driver_Type *)FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Pin_DeInit);
    
    drv->Pin_DeInit(dev, pin);
}

void GPIO_PinInit(FW_GPIO_Type *dev, u16 pin, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed)
{
    if(pin == PIN_NULL)  return;
    
    FW_ASSERT(dev);
    
    FW_GPIO_Driver_Type *drv = (FW_GPIO_Driver_Type *)FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Pin_Init);
    
    drv->Pin_Init(dev, pin, mode, speed);
}

void GPIO_PinWrite(FW_GPIO_Type *dev, u16 pin, u8 value)
{
    if(pin == PIN_NULL)  return;
    
    FW_ASSERT(dev);
    
    FW_GPIO_Driver_Type *drv = (FW_GPIO_Driver_Type *)FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Pin_Write);

    drv->Pin_Write(dev, pin, value);
}

u8   GPIO_PinGetOutput(FW_GPIO_Type *dev, u16 pin)
{
    if(pin == PIN_NULL)  return (u8)GPIO_RET_INVALUE;
    
    FW_ASSERT(dev);
    
    FW_GPIO_Driver_Type *drv = (FW_GPIO_Driver_Type *)FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Pin_GetOutput)

    return drv->Pin_GetOutput(dev, pin);
}

u8   GPIO_PinRead(FW_GPIO_Type *dev, u16 pin)
{
    if(pin == PIN_NULL)  return (u8)GPIO_RET_INVALUE;
    
    FW_ASSERT(dev);
    
    FW_GPIO_Driver_Type *drv = (FW_GPIO_Driver_Type *)FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Pin_Read);

    return drv->Pin_Read(dev, pin);
}

void GPIO_PinToggle(FW_GPIO_Type *dev, u16 pin)
{
    dev->Pin_Toggle(dev, pin);
}


void GPIO_PortDeInit(FW_GPIO_Type *dev, u16 port)
{
    if(port == PORT_NULL)  return;
    
    FW_ASSERT(dev);
    
    FW_GPIO_Driver_Type *drv = (FW_GPIO_Driver_Type *)FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Port_DeInit);

    drv->Port_DeInit(dev, port);
}

void GPIO_PortInit(FW_GPIO_Type *dev, u16 port, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed)
{
    if(port == PORT_NULL)  return;
    
    FW_ASSERT(dev);
    
    FW_GPIO_Driver_Type *drv = (FW_GPIO_Driver_Type *)FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Port_Init);

    drv->Port_Init(dev, port, mode, speed);
}

void GPIO_PortWrite(FW_GPIO_Type *dev, u16 port, u32 value)
{
    if(port == PORT_NULL)  return;
    
    FW_ASSERT(dev);
    
    FW_GPIO_Driver_Type *drv = (FW_GPIO_Driver_Type *)FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Port_Write);

    drv->Port_Write(dev, port, value);
}

u32  GPIO_PortGetOutput(FW_GPIO_Type *dev, u16 port)
{
    if(port == PORT_NULL)  return 0;
    
    FW_ASSERT(dev);
    
    FW_GPIO_Driver_Type *drv = (FW_GPIO_Driver_Type *)FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Port_GetOutput);
    
    return drv->Port_GetOutput(dev, port);
}

u32  GPIO_PortRead(FW_GPIO_Type *dev, u16 port)
{
    if(port == PORT_NULL)  return 0;
    
    FW_ASSERT(dev);
    
    FW_GPIO_Driver_Type *drv = (FW_GPIO_Driver_Type *)FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Port_Read);
    
    return drv->Port_Read(dev, port);
}

__INLINE_STATIC_ void FW_GPIO_ToggleUseGetOutput(FW_GPIO_Type *dev, u16 pin)
{
    if(pin == PIN_NULL)  return;
    
    FW_GPIO_Driver_Type *drv = (FW_GPIO_Driver_Type *)FW_Device_GetDriver(dev);
    
    FW_ASSERT(dev);
    FW_ASSERT(drv);
    FW_ASSERT(drv->Pin_Write);
    FW_ASSERT(drv->Pin_GetOutput);
    
    if(drv->Pin_GetOutput(dev, pin) == LEVEL_H)
    {
        drv->Pin_Write(dev, pin, LEVEL_L);
    }
    else
    {
        drv->Pin_Write(dev, pin, LEVEL_H);
    }
}

__INLINE_STATIC_ void FW_GPIO_ToggleUseLAPI(FW_GPIO_Type *dev, u16 pin)
{
    if(pin == PIN_NULL)  return;
    
    FW_GPIO_Driver_Type *drv = (FW_GPIO_Driver_Type *)FW_Device_GetDriver(dev);
    
    FW_ASSERT(dev);
    FW_ASSERT(drv);
    FW_ASSERT(drv->Pin_Toggle);
    
    drv->Pin_Toggle(dev, pin);
}

