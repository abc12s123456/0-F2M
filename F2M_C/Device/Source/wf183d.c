#include "wf183d.h"

#include "fw_gpio.h"
#include "fw_delay.h"
#include "fw_i2c.h"
#include "fw_uart.h"

#include "fw_print.h"

#include "sensor.h"


#define WF183D_I2C_REG_CMD             0x0A

#define WF183D_I2C_REG_PRES_B3         0x0B
#define WF183D_I2C_REG_PRES_B2         0x0C
#define WF183D_I2C_REG_PRES_B1         0x0D
#define WF183D_I2C_REG_PRES_B0         0x0E

#define WF183D_I2C_REG_TEMP_B1         0x0F
#define WF183D_I2C_REG_TEMP_B0         0x10

#define WF183D_I2C_REG_STATUS          0x13

#define WF183D_I2C_SLAVE_ADDR          0xDA


/* I2C驱动 */
__INLINE_STATIC_ void WF183D_I2C_WriteByte(WF183D_Type *dev, u8 reg_addr, u8 value)
{
    FW_I2C_Type *i2c = FW_Device_GetParent(dev);
    FW_I2C_Write(i2c, reg_addr, &value, 1);
}

__INLINE_STATIC_ u8   WF183D_I2C_ReadByte(WF183D_Type *dev, u8 reg_addr)
{
    FW_I2C_Type *i2c = FW_Device_GetParent(dev);
    u8 value = 0;
    FW_I2C_Read(i2c, reg_addr, &value, 1);
    return value;
}

__INLINE_STATIC_ void WF183D_I2C_Init(WF183D_Type *dev)
{
    FW_I2C_Type *i2c = FW_Device_GetParent(dev);
    FW_I2C_Init(i2c);
    FW_I2C_SetDelay(i2c, 0, 0, 0, 5);
    
    (void)WF183D_I2C_WriteByte;
    (void)WF183D_I2C_ReadByte;
}

__INLINE_STATIC_ u8   WF183D_I2C_GetTemprature(WF183D_Type *dev, s32 *pdata)
{
    return 0;
}

__INLINE_STATIC_ u8   WF183D_I2C_GetPressure(WF183D_Type *dev, s32 *pdata)
{
    return 0;
}


/* UART驱动 */
__INLINE_STATIC_ u8   CRC_Get(u8 *pdata, u8 num)
{
    u8 crc = 0;
    u8 i;
    
    while(num--)
    {
        crc ^= *pdata++;
        for(i = 0; i < 8; i++)
        {
            if(crc & 0x01)  crc = (crc >> 1) ^ 0x8C;
            else  crc >>= 1;
        }
    }
    
    return crc;
}

__INLINE_STATIC_ void WF183D_UART_Init(WF183D_Type *dev)
{
    FW_UART_Type *uart = FW_Device_GetParent(dev);
    if(uart->Baudrate != 9600)  uart->Baudrate = 9600;
    FW_UART_Init(uart);
}

__INLINE_STATIC_ u8   WF183D_UART_GetTemprature(WF183D_Type *dev, s32 *pdata)
{
    FW_UART_Type *uart = FW_Device_GetParent(dev);
    u8 msg[6] = {0x55, 0x04, 0x0E, 0x6A, 0x00};
    u8 len, crc;
    
    FW_UART_Write(uart, 0, msg, 4);
    FW_Delay_Ms(50);
    
    len = FW_UART_ReadHL(uart, 0xAA, 1, 0, msg, sizeof(msg));
    if(len)
    {
        crc = CRC_Get(msg, len - 1);
        if(crc == msg[len - 1] && msg[2] == 0x0A)
        {
            *pdata = *(s32 *)&msg[3];
            return Sensor_RS_OK;
        }
        else
        {
            return Sensor_RS_VerifyErr;
        }
    }
    
    return Sensor_RS_NoData;
}

__INLINE_STATIC_ u8   WF183D_UART_GetPressure(WF183D_Type *dev, s32 *pdata)
{
    FW_UART_Type *uart = FW_Device_GetParent(dev);
    u8 msg[8] = {0x55, 0x04, 0x26, 0x8B, 0x00};
    u8 len, crc;
    
    WF183D_UART_GetTemprature(dev, (s32 *)pdata);
    
    FW_UART_Write(uart, 0, msg, 4);
    FW_Delay_Ms(100);

    RRX:
    len = FW_UART_ReadHL(uart, 0xAA, 1, 0, msg, sizeof(msg));
    if(len)
    {
        if(msg[2] != 0x18)  goto RRX;
        crc = CRC_Get(msg, len - 1);
        if(crc == msg[len - 1])
        {
            *pdata = *(s32 *)&msg[3];
            return Sensor_RS_OK;
        }
        else
        {
            return Sensor_RS_VerifyErr;
        }
    }
    
    return Sensor_RS_NoData;
}


/* OWI驱动 */
#define OWI_1T     25   //通讯时序周期，单位：us

__INLINE_STATIC_ u8   WF183D_OWI_ReadByte(WF183D_Type *dev)
{
    return 0;
}

__INLINE_STATIC_ void WF183D_OWI_Init(WF183D_Type *dev)
{
    FW_GPIO_Init(dev->OWI_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_SET(dev->OWI_Pin);
    
    (void)WF183D_OWI_ReadByte;
}

__INLINE_STATIC_ u8   WF183D_OWI_GetTemprature(WF183D_Type *dev, s32 *pdata)
{
    return 0;
}

__INLINE_STATIC_ u8   WF183D_OWI_GetPressure(WF183D_Type *dev, s32 *pdata)
{
    return 0;
}


__INLINE_STATIC_ void WF183D_Init(Sensor_Type *sensor)
{
    WF183D_Type *dev = FW_Device_GetParent(sensor);
    char *name = FW_Device_GetName(sensor);
    void *p, *q;
    
    p = FW_Device_GetDriver(dev);
    if(p == NULL)
    {
        LOG_D("WF183D-%驱动为空\r\n", name);
    }
    
    q = FW_Driver_Find("uart->dev");
    if(q && p == q)
    {
        /* UART_GPD_Driver */
        dev->Init           = WF183D_UART_Init;
        dev->Get_Pressure   = WF183D_UART_GetPressure;
        dev->Get_Temprature = WF183D_UART_GetTemprature;
        goto Init;
    }
    
    q = FW_Driver_Find("i2c->dev");
    if(q && p == q)
    {
        /* I2C_GPD_Driver */
        dev->Init           = WF183D_I2C_Init;
        dev->Get_Pressure   = WF183D_I2C_GetPressure;
        dev->Get_Temprature = WF183D_I2C_GetTemprature;
        goto Init;
    }
    
    if(dev->OWI_Pin != PIN_NULL)
    {
        /* OWI_Driver */
        dev->Init           = WF183D_OWI_Init;
        dev->Get_Pressure   = WF183D_OWI_GetPressure;
        dev->Get_Temprature = WF183D_OWI_GetTemprature;
        goto Init;
    }
    
    /* 驱动异常 */
    LOG_D("WF183D-%s驱动异常\r\n", name);
    return;
    
    Init:
    sensor->Type = Sensor_Pressure;
    
    dev->Init(dev);
}

__INLINE_STATIC_ void WF183D_Reset(Sensor_Type *sensor)
{

}

__INLINE_STATIC_ void WF183D_Set_Period(Sensor_Type *sensor, u16 period)
{

}

__INLINE_STATIC_ u8   WF183D_Get_Temprature(Sensor_Type *sensor, s32 *pdata)
{
    WF183D_Type *dev = FW_Device_GetParent(sensor);
    return dev->Get_Temprature(dev, pdata);
}

__INLINE_STATIC_ u8   WF183D_Get_Pressure(Sensor_Type *sensor, s32 *pdata)
{
    WF183D_Type *dev = FW_Device_GetParent(sensor);
    return dev->Get_Pressure(dev, pdata);
}

__INLINE_STATIC_ s32  WF183D_Press_Formula(s32 value)
{
    return value;
}




/*  */
__CONST_STATIC_ Sensor_Driver_Type Pressure_Driver =
{
    .Init           = WF183D_Init,
    
    .Reset          = WF183D_Reset,
    .Set_Period     = WF183D_Set_Period,
    
    .Get_Temprature = WF183D_Get_Temprature,
    .Get_Sample     = WF183D_Get_Pressure,
    .Formula        = WF183D_Press_Formula,
};
FW_DRIVER_REGIST("wf183d->sensor", &Pressure_Driver, WF183D);




#include "fw_debug.h"
#if MODULE_TEST && SENSOR_TEST && WF183D_TEST


#define UART_NAME            "uart0"
static  void UART_Pre_Init(void *pdata)
{
    FW_UART_Type *uart = FW_Device_Find(UART_NAME);
    uart->TX_Pin = PA9;
    uart->RX_Pin = PA10;
    uart->Baudrate = 9600;
    uart->RX_RB_Size = 64;
    uart->Config.RX_Mode = TRM_INT;
}
FW_PRE_INIT(UART_Pre_Init, NULL);


#define WF183D_NAME          "wf183d"
#define WF183D_DRV_ID        FW_DRIVER_ID(FW_Device_UART_Device, UART_GP_DRV_NUM)
static  WF183D_Type          WF183D;
static  void WF183D_Config(void *dev)
{
    FW_Device_SetParent(dev, FW_Device_Find(UART_NAME));
    FW_Device_SetDriver(dev, FW_Driver_Find("uart->dev"));
}
FW_DEVICE_STATIC_REGIST(WF183D_NAME, &WF183D, WF183D_Config, WF183D);


#define SENSOR_NAME          "sensor"
static  Sensor_Type          Sensor;
static  void Sensor_Config(void *dev)
{
    FW_Device_SetParent(dev, FW_Device_Find(WF183D_NAME));
    FW_Device_SetDriver(dev, FW_Driver_Find("wf183d->sensor"));
}
FW_DEVICE_STATIC_REGIST(SENSOR_NAME, &Sensor, Sensor_Config, Sensor_Pressure);


#define MAX      10
s32 Pressure = 0;
s16 Temprature = 0;
static s32 P_Buf[MAX];


void Test(void)
{
    u16 VCC_EN = PA11;
    s32 sum = 0, tmp;
    u8 state, i, cnt = 0;
    
    Sensor_Type *sensor;
    
    
    sensor = FW_Device_Find(SENSOR_NAME);
    if(sensor == NULL)
    {
        while(1);
    }
    
    
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_CLR(VCC_EN);
    
    Sensor_Init(sensor);
    
    while(1)
    {
//        state = Sensor_Get_Temprature(sensor, (s16 *)&tmp);
//        if(state == Sensor_RS_OK)
//        {
//            Temprature = (s16)tmp;
//        }
        
        state = Sensor_Get_Sample(sensor, &tmp);
        if(state != Sensor_RS_OK)
        {
            continue;
        }
        
        P_Buf[cnt++] = tmp;
        if(cnt >= 10)
        {
            cnt = 0;
        }
        
        sum = 0;
        for(i = 0; i < MAX; i++)
        {
            sum += P_Buf[i];
        }
        
        Pressure = sum / MAX;
        
        FW_Delay_Ms(100);
    }
}


#endif

