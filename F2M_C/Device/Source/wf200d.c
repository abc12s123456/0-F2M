#include "wf200d.h"

#include "fw_gpio.h"
#include "fw_delay.h"
#include "fw_i2c.h"
#include "fw_spi.h"

#include "fw_print.h"

#include "sensor.h"


/*

*/


#define WF200D_TIMEOUT            10

#define WF200D_REG_CONFIG         0xA5           //

#define WF200D_REG_SAMP_CMD       0x30
#define WF200D_SAMP_TEMP_ONCE     0x08
#define WF200D_SAMP_PRES_ONCE     0x09
#define WF200D_SAMP_COMB_ONCE     0x0A
#define WF200D_SAMP_COMB_CONTINUE 0x0B

#define WF200D_REG_STATUS         0x02           //状态寄存器

#define WF200D_REG_PRES_MSB       0x06
#define WF200D_REG_PRES_CSB       0x07
#define WF200D_REG_PRES_LSB       0x08

#define WF200D_REG_TEMP_MSB       0x09
#define WF200D_REG_TEMP_LSB       0x0A

#define WF200D_I2C_SLAVE_ADDR     0x6D           //WF200D作为I2C设备时的从机地址

#define WF200D_PRES_NV            0x1000000
#define WF200D_TEMP_NV            0x10000

#define WF200D_PRES_NM            0x800000
#define WF200D_TEMP_NM            0x8000

#define WF200D_PRES_MAX
#define WF200D_PRES_MIN           


/* I2C驱动 */
__INLINE_STATIC_ void WF200D_I2C_Init(WF200D_Type *dev)
{
    FW_I2C_Type *i2c = FW_Device_GetParent(dev);
    FW_I2C_Init(i2c);
    FW_I2C_SetDelay(i2c, 0, 0, 0, 5);
}

__INLINE_STATIC_ void WF200D_I2C_WriteByte(WF200D_Type *dev, u8 reg_addr, u8 value)
{
    FW_I2C_Type *i2c = FW_Device_GetParent(dev);
    FW_I2C_Write(i2c, reg_addr, &value, 1);
}

__INLINE_STATIC_ u8   WF200D_I2C_ReadByte(WF200D_Type *dev, u8 reg_addr)
{
    FW_I2C_Type *i2c = FW_Device_GetParent(dev);
    u8 value = 0;
    FW_I2C_Read(i2c, reg_addr, &value, 1);
    return value;
}


/* SPI驱动 */
__INLINE_STATIC_ void WF200D_SPI_Init(WF200D_Type *dev)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    FW_SPI_Init(spi);
}

__INLINE_STATIC_ void WF200D_SPI_WriteByte(WF200D_Type *dev, u8 reg_addr, u8 value)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    u8 msg[2];
    
    msg[0] = reg_addr;
    msg[1] = value;
    
    FW_SPI_CSSet(spi, Enable);
    FW_SPI_Write(spi, 0, msg, 2);
    FW_SPI_CSSet(spi, Disable);
}

__INLINE_STATIC_ u8   WF200D_SPI_ReadByte(WF200D_Type *dev, u8 reg_addr)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    u8 value;
    
    FW_SPI_CSSet(spi, Enable);
    FW_SPI_Write(spi, 0, &reg_addr, 1);
    FW_SPI_Read(spi, 0, &value, 1);
    FW_SPI_CSSet(spi, Disable);
    
    return value;
}


__INLINE_STATIC_ void WF200D_Write_Byte(WF200D_Type *dev, u8 reg_addr, u8 value)
{
    dev->Write_Byte(dev, reg_addr, value);
}

__INLINE_STATIC_ u8   WF200D_Read_Byte(WF200D_Type *dev, u8 reg_addr)
{
    return dev->Read_Byte(dev, reg_addr);
}


__INLINE_STATIC_ void WF200D_Init(Sensor_Type *sensor)
{
    WF200D_Type *dev = FW_Device_GetParent(sensor);
    char *name = FW_Device_GetName(dev);
    void *p, *q;
    
    /* 获取WF200D的驱动 */
    p = FW_Device_GetDriver(dev);
    if(p == NULL)
    {
        LOG_D("WF200D-%s驱动为空\r\n", name);
        return;
    }
    
    q = FW_Driver_Find("i2c->dev");
    if(q && p == q)
    {
        /* I2C_GPD_Driver */
        dev->Init       = WF200D_I2C_Init;
        dev->Write_Byte = WF200D_I2C_WriteByte;
        dev->Read_Byte  = WF200D_I2C_ReadByte;
        goto Init;
    }
    
    q = FW_Driver_Find("spi->dev");
    if(q && p == q)
    {
        /* SPI_GPD_Driver */
        dev->Init       = WF200D_SPI_Init;
        dev->Write_Byte = WF200D_SPI_WriteByte;
        dev->Read_Byte  = WF200D_SPI_ReadByte;
        goto Init;
    }
    
    /* 驱动异常 */
    LOG_D("WF200D-%s驱动异常\r\n", name);
    return;
    
    Init:
    sensor->Type = Sensor_Pressure;
    
//    if(dev->RDY_Pin)
//    {
//        FW_GPIO_Init(dev->RDY_Pin, FW_GPIO_Mode_IPU, FW_GPIO_Speed_Low);
//    }
    
    dev->Init(dev);
}

__INLINE_STATIC_ void WF200D_Reset(Sensor_Type *sensor)
{
    
}

//__INLINE_STATIC_ u8   WF200_Get_Type(Sensor_Type *sensor)
//{
//    return Sensor_Pressure;
//}

__INLINE_STATIC_ void WF200D_Set_Period(Sensor_Type *sensor, u16 period)
{
    
}

__INLINE_STATIC_ u8   WF200D_Get_Temprature(Sensor_Type *sensor, s32 *pdata)
{
    WF200D_Type *dev = FW_Device_GetParent(sensor);
    s16 temp = 0;
    u8 i;
    
    i = 0;
    WF200D_Write_Byte(dev, WF200D_REG_SAMP_CMD, WF200D_SAMP_TEMP_ONCE);
    while(WF200D_Read_Byte(dev, WF200D_REG_STATUS) & 0x08)
    {
        FW_Delay_Ms(10);
        if(++i >= WF200D_TIMEOUT)  return Sensor_RS_Fail;
    }
    
//    i = 0;
//    while(FW_GPIO_Read(dev->RDY_Pin) != LEVEL_L)
//    {
//        FW_Delay_Ms(10);
//        if(++i >= WF200D_TIMEOUT)  return Sensor_RS_Fail;
//    }
    
    temp += WF200D_Read_Byte(dev, WF200D_REG_TEMP_MSB) << 8;
    temp += WF200D_Read_Byte(dev, WF200D_REG_TEMP_LSB);
    
    if(temp & WF200D_TEMP_NM)  temp -= WF200D_TEMP_NV;
    
    *pdata = (s32)temp;
    
    return Sensor_RS_OK;
}

__INLINE_STATIC_ u8   WF200D_Get_Pressure(Sensor_Type *sensor, s32 *pdata)
{
    WF200D_Type *dev = FW_Device_GetParent(sensor);
    s32 press = 0;
    u8 i;
    
    i = 0;
    WF200D_Write_Byte(dev, WF200D_REG_SAMP_CMD, WF200D_SAMP_PRES_ONCE);
    while(WF200D_Read_Byte(dev, WF200D_REG_STATUS) & 0x08)
    {
        FW_Delay_Ms(10);
        if(++i >= WF200D_TIMEOUT)  return Sensor_RS_Fail;
    }
    
//    i = 0;
//    while(FW_GPIO_Read(dev->RDY_Pin) != LEVEL_L)
//    {
//        FW_Delay_Ms(10);
//        if(++i >= WF200D_TIMEOUT)  return Sensor_RS_Fail;
//    }
    
    press += WF200D_Read_Byte(dev, WF200D_REG_PRES_MSB) << 16;
    press += WF200D_Read_Byte(dev, WF200D_REG_PRES_CSB) << 8;
    press += WF200D_Read_Byte(dev, WF200D_REG_PRES_LSB);
    
    if(press & WF200D_PRES_NM) press -= WF200D_PRES_NV;
    
    *pdata = press;
    
    return Sensor_RS_OK;
}


s32  WF200D_Press_Formula(s32 value)
{
    return (value / 8388608);
}




/*  */
__CONST_STATIC_ Sensor_Driver_Type Pressure_Driver =
{
    .Init           = WF200D_Init,
    
    .Reset          = WF200D_Reset,
    .Set_Period     = WF200D_Set_Period,
    
    .Get_Temprature = WF200D_Get_Temprature,
    .Get_Sample     = WF200D_Get_Pressure,
    .Formula        = WF200D_Press_Formula,
};
FW_DRIVER_REGIST("wf200d->sensor", &Pressure_Driver, WF200D_P);




#include "fw_debug.h"
#if MODULE_TEST && SENSOR_TEST && WF200D_TEST


#define SI2C0_NAME           "si2c0"
static  FW_I2C_Type          SI2C0;
static  void SI2C0_Config(void *dev)
{
    FW_I2C_Type *i2c = dev;
    FW_I2C_Driver_Type *drv = FW_Driver_Find("io->i2c");
    
//    i2c->SCL_Pin = PB11;
//    i2c->SDA_Pin = PB10;
    i2c->SCL_Pin = PB13;
    i2c->SDA_Pin = PB14;
    i2c->ID      = WF200D_I2C_SLAVE_ADDR << 1;
    i2c->First_Bit = FW_I2C_FirstBit_MSB;
    
    FW_Device_SetDriver(i2c, drv);
}
FW_DEVICE_STATIC_REGIST(SI2C0_NAME, &SI2C0, SI2C0_Config, SI2C0);


#define SPI_DEV_NAME         "hspi1"
static void SPI_Pre_Init(void *pdata)
{
    FW_SPI_Type *spi = FW_Device_Find(SPI_DEV_NAME);
    
    spi->MOSI_Pin = PB15;
    spi->MISO_Pin = PB14;
    spi->SCK_Pin = PB13;
    spi->CS_Pin = PB12;
    
    spi->Baudrate = 1000000;
    spi->First_Bit = FW_SPI_FirstBit_MSB;
    spi->Clock_Polarity = FW_SPI_ClockPolarity_H;
    spi->Clock_Phase = FW_SPI_ClockPhase_Edge2;
}
FW_PRE_INIT(SPI_Pre_Init, NULL);


#define WF200D_NAME          "wf200d"
static  WF200D_Type          WF200D;
static  void WF200D_Config(void *dev)
{
    ((WF200D_Type *)dev)->RDY_Pin = PA12;
    FW_Device_SetParent(dev, FW_Device_Find(SI2C0_NAME));
    FW_Device_SetDriver(dev, FW_Driver_Find("i2c->dev"));
//    FW_Device_SetParent(dev, FW_Device_Find(SPI_DEV_NAME));
//    FW_Device_SetDriver(dev, FW_Driver_Find("spi->dev"));
}
FW_DEVICE_STATIC_REGIST(WF200D_NAME, &WF200D, WF200D_Config, WF200D);


#define SENSOR_NAME          "sensor"
static  Sensor_Type          Sensor;
static  void Sensor_Config(void *dev)
{
    FW_Device_SetParent(dev, FW_Device_Find(WF200D_NAME));
    FW_Device_SetDriver(dev, FW_Driver_Find("wf200d->sensor"));
}
FW_DEVICE_STATIC_REGIST(SENSOR_NAME, &Sensor, Sensor_Config, Sensor_Pressure);



#define MAX      10
s32 Pressure = 0;
s16 Temprature = 0;
static s32 P_Buf[MAX];


void Test(void)
{
    u16 VCC_EN = PC13;
    s32 sum = 0, tmp;
    u8 state, i, cnt = 0;
    
    Sensor_Type *sensor;
    
    
    sensor = FW_Device_Find(SENSOR_NAME);
    if(sensor == NULL)
    {
        while(1);
    }
    
    
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
    FW_GPIO_SET(VCC_EN);
    
    Sensor_Init(sensor);
    
    while(1)
    {
        state = Sensor_Get_Temprature(sensor, (s16 *)&tmp);
        if(state == Sensor_RS_OK)
        {
            Temprature = (s16)tmp;
        }
        
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
        
        FW_Delay_Ms(500);
    }
}


#endif

