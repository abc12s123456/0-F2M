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
 
#include "ip5310.h"

#include "fw_i2c.h"
#include "fw_debug.h"
#include "fw_print.h"


#define REG_SYS_CTL0         0x00
#define REG_SYS_CTL1         0x01
#define REG_SYS_CTL2         0x0E
#define REG_SYS_CTL3         0x18
#define REG_SYS_CTL4         0x1A
#define REG_SYS_CTL5         0x0A

#define REG_CHARGE_CTL0      0x20
#define REG_CHARGE_CTL1      0x21
#define REG_CHARGE_CTL2      0x22
#define REG_CHARGE_CTL3      0x23

#define REG_CHG_DIG_CTL0     0x24
#define REG_CHG_DIG_CTL1     0x26

#define REG_READ0            0x70
#define REG_READ1            0x71
#define REG_READ2            0x72
#define REG_READ3            0x74
#define REG_READ4            0x77

#define REG_ADD_MASK         0x00000000


void IP5310_Init(IP5310_Type *dev)
{
    FW_I2C_Type *i2c = FW_Device_GetParent(dev);
    
    if(i2c == NULL)
    {
        LOG_E("ip5310's i2c driver is null\r\n");
        return;
    }
    
    FW_I2C_Init(i2c);
}

void IP5310_Set_Boost(IP5310_Type *dev, u8 state)
{
    
}


//u8   IP5310_Get_Residual(IP5310_Type *dev)
//{
//    FW_I2C_Type *i2c = FW_Device_GetParent(dev);
//    u8 residual;
//    
//    if(FW_I2C_Read(i2c, REG_ADD_MASK | REG_READ3, &residual, 1) != 0)
//    {
//        residual >>= 4;
//        residual = (residual == 0x0F) ? 80 :
//                   (residual == 0x07) ? 60 :
//                   (residual == 0x03) ? 40 :
//                   (residual == 0x01) ? 20 :
//                   0;
//        return residual;
//    }
//    
//    return IP5310_Residual_Unknown;
//}
u8   IP5310_Get_Residual(IP5310_Type *dev)
{
    FW_I2C_Type *i2c = FW_Device_GetParent(dev);
    u8 residual;
    
    if(FW_I2C_Read(i2c, REG_ADD_MASK | REG_READ3, &residual, 1) != 0)
    {
        residual >>= 4;
        return residual;
    }
    
    return IP5310_Residual_Unknown;
}


u8   IP5310_Get_State(IP5310_Type *dev)
{
    FW_I2C_Type *i2c = FW_Device_GetParent(dev);
    u8 state;
 
    if(FW_I2C_Read(i2c, REG_ADD_MASK | REG_READ0, &state, 1) != 0)
    {
        state &= 0x08;
        if(state)
        {
            if(FW_I2C_Read(i2c, REG_ADD_MASK | REG_READ1, &state, 1) != 0)
            {
                state &= 0x08;
                if(state)
                {
                    return IP5310_State_Done;
                }
                else
                {
                    return IP5310_State_Charge;
                }
            }
        }
        else
        {
            return IP5310_State_Stop;
        }
    }
    
    return IP5310_State_Unknown;
}

u8   IP5310_Get_Load(IP5310_Type *dev)
{
    FW_I2C_Type *i2c = FW_Device_GetParent(dev);
    u8 load;
 
    if(FW_I2C_Read(i2c, REG_READ2, &load, 1) != 0)
    {
        load &= 0x04;
        load >>= 2;
        return load;
    }
    
    return IP5310_Load_Unknown;
}

u8   IP5310_Get_CTL0(IP5310_Type *dev)
{
    FW_I2C_Type *i2c = FW_Device_GetParent(dev);
    u8 value;
    
    if(FW_I2C_Read(i2c, REG_SYS_CTL0, &value, 1) != 0)
    {
        value &= 0x3F;
        return value;
    }
    
    return 0xFF;
}



#include "project_debug.h"
#if MODULE_TEST && I2C_DEVICE_TEST && IP5310_TEST
#include "fw_gpio.h"
#include "led.h"


#define SI2C_DRV_ID         FW_DRIVER_ID(FW_Device_I2C, I2C_IOSIM_DRV_NUM)

static FW_I2C_Type SI2C0;
static void SI2C0_Config(void *dev)
{
    SI2C0.SCL_Pin = PF1;
    SI2C0.SDA_Pin = PF0;
    
    FW_Device_SetDriver(&SI2C0, FW_Driver_Find(SI2C_DRV_ID));
    FW_Device_SetDID(&SI2C0, SI2C_DRV_ID);
}
FW_DEVICE_STATIC_REGIST("si2c0", &SI2C0, SI2C0_Config, SI2C0);


static IP5310_Type PM;
static void PM_Config(void *dev)
{
    FW_I2C_Type *i2c = FW_Device_Find("hi2c1");
    
    FW_Device_SetParent(dev, i2c);
    
    i2c->SCL_Pin = PF1;
    i2c->SDA_Pin = PF0;
    i2c->ID = 0xEA;
}
FW_DEVICE_STATIC_REGIST("pm0", &PM, PM_Config, PM_IP5310);


void Test(void)
{
    IP5310_Type *pm;
    u8 tmp;
//    u16 led = PA0;
    
    LED_Type led = {0};
    
    pm = FW_Device_Find("pm0");
    if(pm == NULL)
    {
        while(1);
    }
    
//    LED_DeInit(&led);
    led.Pin = PA0;
    led.Level = LEVEL_H;
    LED_Init(&led);
    
//    IP5310_Init(pm);
//    
//    tmp = IP5310_Get_Residual(pm);
//    if(tmp)
//    {
//        tmp = tmp;
//    }
//    
//    tmp = IP5310_Get_ChargeState(pm);
//    if(tmp)
//    {
//        tmp = tmp;
//    }
//    
//    tmp = IP5310_Get_CTL0(pm);
//    if(tmp)
//    {
//        tmp = tmp;
//    }
    
    while(1)
    {
        LED_Breath(&led, 1000);
    }
}


#endif

