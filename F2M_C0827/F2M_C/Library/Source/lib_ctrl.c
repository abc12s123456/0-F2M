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
#include "lib_ctrl.h"
#include "libc.h"


/*
PID参数整定方法：
1、确定Kp时，首先去掉PID的积分项和微分项，可以令Ki=0、Kd=0，使之成为纯比例调节。
   输入设定为系统允许输出最大值的60%~70%，比例系数Kp由0开始逐渐增大，直至系统出现
   震荡；再反过来，从此时的比例系数Kp逐渐减小，直至系统震荡消失。记录此时的比例系
   数Kp，设定PID的比例系数Kp为当前值的60%~70%。
2、确定了Kp以后，设定一个较大的积分时间常数



采样周期的选取应与PID参数的整定进行综合考虑，采样周期应远小于过程的扰动信号的周期，
在执行器的响应速度比较慢时，过小的采样周期将失去意义，因此可适当选大一些。在计算机
速度允许的条件下，采样周期短，则控制品质好；当过程的纯滞后时间较长时，一般选择采样
周期为纯滞后时间的1/4~1/8.

PID执行周期 ≥ 传感器采样周期，即传感器采样一次，PID执行一次，或传感器采样多次，PID
执行一次。
*/

/* 判断两个浮点数是否相等，若相等，返回True，若不等，返回False */
#define fequal(f1, f2)       (Bool)(((double)(f1 - f2) < EPS) && ((double)(f2 - f1) < EPS))


void PPID_Init(PPID_Type *pid)
{
    pid->Ek_0 = 0;
    pid->Ek_1 = 0;
    pid->Integral = 0;
    
    if(fequal(pid->IMax, pid->IMin) == True)  pid->IMax = FLOAT_MAX;
    if(fequal(pid->OMax, pid->OMin) == True)  pid->OMax = FLOAT_MAX;
}

float PPID_Get(PPID_Type *pid, float target, float actual)
{
    float err, output;
    
    err = target - actual;
    
    pid->Integral += err;
    if(pid->Integral > pid->IMax)  pid->Integral = pid->IMax;
    else if(pid->Integral < pid->IMin)  pid->Integral = pid->IMin;
    else 
    {}
    
    output = pid->Kp * err +
             pid->Ki * pid->Integral +
             pid->Kd * (err - pid->Ek_1);
    if(output > pid->OMax)  output = pid->OMax;
    else if(output < pid->OMin)  output = pid->OMin;
    else
    {}
    
    pid->Ek_1 = err;
    
    return output;
}


void IPID_Init(IPID_Type *pid)
{
    pid->Ek_0 = 0.0f;
    pid->Ek_1 = 0.0f;
    pid->Ek_2 = 0.0f;
}

float IPID_Get(IPID_Type *pid, float target, float actual)
{
    float inc;
    
    pid->Ek_0 = target - actual;
    
    inc = pid->Kp * (pid->Ek_0 - pid->Ek_1) +
          pid->Ki * pid->Ek_0 +
          pid->Kd * (pid->Ek_0 - 2 * pid->Ek_1 + pid->Ek_2);
    
    pid->Ek_2 = pid->Ek_1;
    pid->Ek_1 = pid->Ek_0;
    
    return inc;
}


void SPID_Init(SPID_Type *pid)
{
    pid->STarget = 0;
    if(fequal(pid->STMax, pid->STMin) == True)  pid->STMax = FLOAT_MAX;
}


/**
 * @target：系统最终目标值
 * @step：步进值
 * SPID的输出是作为PPID或IPID的输入来执行的
 */
float SPID_Get(SPID_Type *pid, float target, float step)
{
    float err;
    
    err = target - pid->STarget;
    
    if(fabs(err) <= step)
    {
        pid->STarget = target;
    }
    else
    {
        if(err > 0)  pid->STarget += step;
        else if(err < 0)  pid->STarget -= step;
        else
        {}
    }
    
    if(pid->STarget > pid->STMax)  pid->STarget = pid->STMax;
    else if(pid->STarget < pid->STMin)  pid->STarget = pid->STMin;
    else
    {}
    
    return pid->STarget;
}




#include "fw_debug.h"
#if MODULE_TEST && LIB_TEST && PID_TEST

#include "fw_exti.h"
#include "fw_gpio.h"
#include "fw_delay.h"

#include "stiming.h"
#include "rbuffer.h"


#include "fw_spi.h"
#include "lcd.h"


/* 屏幕 */
#if 0
static FW_SPI_Type SSPI0;
static void SSPI0_Config(void *dev)
{
    FW_SPI_Type *spi = dev;
    FW_SPI_Driver_Type *drv = FW_Driver_Find("io->spi");
    FW_Device_SetDriver(spi, drv);
    
    spi->CS_Pin = PB12;
    spi->SCK_Pin = PB13;
    spi->MOSI_Pin = PB14;
    
    spi->Clock_Polarity = FW_SPI_ClockPolarity_H;
    spi->Clock_Phase = FW_SPI_ClockPhase_Edge2;
    spi->Data_Width = FW_SPI_DataWidth_8Bits;
    spi->CS_VL = LEVEL_L;
    spi->First_Bit = FW_SPI_FirstBit_MSB;
}
FW_DEVICE_STATIC_REGIST("sspi0", &SSPI0, SSPI0_Config, SSPI0);
#else
#define LCD_SPI_NAME         "spi1"
static  void LCD_SPI_Pre_Init(void *pdata)
{
    FW_SPI_Type *spi = FW_Device_Find(LCD_SPI_NAME);
    
    spi->CS_Pin = PB12;
    spi->SCK_Pin = PB13;
    spi->MISO_Pin = PIN_NULL;
    spi->MOSI_Pin = PB15;
    
    spi->Baudrate = 400000;
    spi->Clock_Polarity = FW_SPI_ClockPolarity_H;
    spi->Clock_Phase = FW_SPI_ClockPhase_Edge2;
    spi->Data_Width = FW_SPI_DataWidth_8Bits;
    spi->CS_VL = LEVEL_L;
    spi->First_Bit = FW_SPI_FirstBit_MSB;
}
FW_PRE_INIT(LCD_SPI_Pre_Init, NULL);
#endif

static LCD8_Type ST7735;
static void ST7735_Config(void *dev)
{
    LCD8_Type *lcd = dev;
    FW_SPI_Type *spi = FW_Device_Find("sspi0");
    
    FW_Device_SetParent(lcd, spi);
    FW_Device_SetDriver(lcd, FW_Driver_Find("spi->dev"));
    
    lcd->Rotate = LCD_Rotate_0;
    lcd->Pixel_W = 80;
    lcd->Pixel_H = 160;
    lcd->X_Offset = 24;
    lcd->Y_Offset = 0;
    
    lcd->Data_Width = LCD_Data_Width_8Bits;
    
    lcd->RST_Pin = PB15;
    lcd->RS_Pin = PA8;
}
FW_DEVICE_STATIC_REGIST("st7735", &ST7735, ST7735_Config, ST7735);


static LCD_Type LCD;
static void LCD_Config(void *dev)
{
    LCD8_Type *parent = FW_Device_Find("st7735");
    
    FW_Device_SetParent(dev, parent);
    FW_Device_SetDriver(dev, FW_Driver_Find("st7735->lcd"));
}
FW_DEVICE_STATIC_REGIST("lcd", &LCD, LCD_Config, LCD1);


/* 位置检测 */
#include "fw_adc.h"

static FW_ADC_Device_Type HA0;
static void HA0_Config(void *dev)
{
    FW_ADC_Device_Type *adc = dev;
    
    adc->Channel = FW_ADC_CH1;
    adc->P_Pin = PA1;
    adc->Sample_Period = 1000;
    adc->Sample_Count = 10;
    
    FW_Device_SetParent(dev, FW_Device_Find("adc0"));
}
FW_DEVICE_STATIC_REGIST("ha0", &HA0, HA0_Config, HA0);

static FW_ADC_Device_Type HA1;
static void HA1_Config(void *dev)
{
    FW_ADC_Device_Type *adc = dev;
    
    adc->Channel = FW_ADC_CH2;
    adc->P_Pin = PA2;
    adc->Sample_Period = 1000;
    adc->Sample_Count = 10;
    
    FW_Device_SetParent(dev, FW_Device_Find("adc0"));
}
FW_DEVICE_STATIC_REGIST("ha1", &HA1, HA1_Config, HA1);



/* 电机控制 */
#include "fw_timer.h"

static FW_ADC_Device_Type MSC;
static void MSC_Config(void *dev)
{
    FW_ADC_Device_Type *adc = dev;
    
    adc->Channel = FW_ADC_CH3;
    adc->P_Pin = PA3;
    adc->Sample_Period = 1000;
    adc->Sample_Count = 10;
    
    FW_Device_SetParent(dev, FW_Device_Find("adc0"));
}
FW_DEVICE_STATIC_REGIST("msc", &MSC, MSC_Config, M_SC);


#define HTIM_NAME    "timer3"
static void HTIM_Pre_Init(void *pdata)
{
    FW_TIM_Type *timer = FW_Device_Find(HTIM_NAME);
    timer->EN = OFF;
    timer->Mode = FW_TIM_Mode_PWM;
}   
FW_PRE_INIT(HTIM_Pre_Init, NULL);


static PWM_Type PWM;
static void PWM_Config(void *dev)
{
    PWM_Type *pwm = dev;
    PWM_Config_Type *config = &pwm->Config;
    
    config->Pin_P = PB8;
    config->CH_P = FW_TIM_CH2;
    config->Frequency = 10000;
    config->DC = 9000;
    config->IT_EN = OFF;
    
    FW_Device_SetParent(dev, FW_Device_Find(HTIM_NAME));
    FW_Device_SetDriver(dev, FW_Driver_Find("timer->pwm"));
}
FW_DEVICE_STATIC_REGIST("pwm", &PWM, PWM_Config, PWM);


typedef struct
{
    PWM_Type *PWM;
    u16 DIR_Pin;
    
    u16 Run_Flag : 1;
    
    u16 : 15;
}M_Type;


static M_Type Motor;


void Motor_Init(void)
{
    M_Type *m = &Motor;
    
    m->PWM = &PWM;
    m->DIR_Pin = PB9;
    m->Run_Flag = False;
    
    PWM_Init(m->PWM);
    PWM_CTL(m->PWM, Disable);
    
    FW_GPIO_Init(m->DIR_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
}

void Motor_Open(void)
{
    M_Type *m = &Motor;
    
    FW_GPIO_SET(m->DIR_Pin);
    PWM_CTL(m->PWM, Enable);
}

void Motor_Close(void)
{
    M_Type *m = &Motor;
    
    FW_GPIO_CLR(m->DIR_Pin);
    PWM_CTL(m->PWM, Enable);
}

void Motor_Stop(void)
{
    M_Type *m = &Motor;
    PWM_CTL(m->PWM, Disable);
}



#include "lv_obj.h"
#include "lv_disp.h"
#include "lv_label.h"

#include "lv_port_disp.h"

#include "lv_color.h"

#include "key.h"


/* 
 * target = (60 * 1000) / 滴数设置值
 */
typedef struct
{
    RB_Type Sample;
    
    u32 Start_Tick;
    
    volatile u32 Count;
    
    u32 Target : 16;
    u32 Less_Err : 16;
    u32 More_Err : 16;
    
    u32 Rate : 8;
    
    u32 First_Flag : 1;
    
    u32 : 7;
}Drop_Rate_Type;


typedef struct
{
    Drop_Rate_Type Drop_Rate;
    
}IPara_Type;


static void EXTI_Infu_Callback(void *pdata)
{
    Drop_Rate_Type *dr = &((IPara_Type *)pdata)->Drop_Rate;
    RB_Type *rb = &dr->Sample;
    u16 duration;
    
    duration = FW_Delay_GetMsDuration(dr->Start_Tick);
    RB_Write(rb, (u8 *)&duration, sizeof(duration));
    
    dr->Start_Tick = FW_Delay_GetMsStart();
    
    dr->Count++;
}


#define DRIP_RATE_LESS_ERR  5
#define DRIP_RATE_MORE_ERR  8

#define UNIT_TIME           (60000)

static u16 Buffer[10];

static void Drop_Init(Drop_Rate_Type *dr)
{
    u8 err;
    
    if(dr->Rate <= 100)  err = DRIP_RATE_LESS_ERR;
    else  err = DRIP_RATE_MORE_ERR;
    
    dr->Target = UNIT_TIME / dr->Rate;
    dr->Less_Err = UNIT_TIME / (dr->Rate - err) - dr->Target;
    dr->More_Err = dr->Target - UNIT_TIME / (dr->Rate + err);
    
    dr->Sample.Buffer = (u8 *)Buffer;
    RB_Init(&dr->Sample, sizeof(Buffer));
    
    dr->First_Flag = True;
}


/* 
 * PID介入的时间：
 * period0 = 检测到最近两滴的时间间隔
 * period1 = 设置滴速的时间间隔
 * 取period0与period1中的较小值
 * 
 * 有采样结果，立即调整；超出最大间隔时间，则调整
 */
IPID_Type IPID;
FW_EXTI_Type EXTI_Infu;
IPara_Type IPara;


u16 HA0_Value, HA1_Value;
Key_Type K4, K5;


#if 1
void Test(void)
{
    IPID_Type *pid = &IPID;
    FW_EXTI_Type *exti = &EXTI_Infu;
    u16 period;
    
    FW_ADC_Device_Type *ha0, *ha1;
    
    
    LCD_Type *lcd = FW_Device_Find("lcd");
    if(lcd == NULL)
    {
        while(1);
    }
    
    Key_Type *k4, *k5;
    u8 kv4, kv5;
    
    k4 = &K4, k5 = &K5;
    
    
    ha0 = FW_Device_Find("ha0");
    FW_ADC_Init(ha0);
    
    ha1 = FW_Device_Find("ha1");
    FW_ADC_Init(ha1);
    
    FW_ADC_Bus_Init(FW_Device_GetParent(ha0));
    
    
    k4->Pin = PB10;
    k4->Level = LEVEL_H;
    Key_Init(k4);
    
    k5->Pin = PB11;
    k5->Level = LEVEL_H;
    Key_Init(k5);
    
    Motor_Init();
    
    Drop_Init(&IPara.Drop_Rate);
    
    exti->Pin = PA11;
    exti->Trigger = FW_EXTI_Trigger_Rising;
    exti->IH_CB = EXTI_Infu_Callback;
    exti->IH_Pdata = &IPara;
    exti->EN = ON;
    FW_EXTI_Init(exti);
    
    lv_init();
    
    extern lv_font_t lv_font_st_16;
    
    lv_obj_t *label = lv_label_create(lv_scr_act());
//    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
//    lv_obj_set_style_text_font(label, &lv_font_st_16, 0);
    lv_obj_set_width(label, 80);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    
    lv_obj_t *lab_h0 = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(lab_h0, &lv_font_montserrat_12, 0);
    lv_obj_set_width(lab_h0, 80);
    lv_obj_align(lab_h0, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_x(lab_h0, 0);
    lv_obj_set_y(lab_h0, 30);
    
    lv_obj_t *lab_h1 = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(lab_h1, &lv_font_montserrat_12, 0);
    lv_obj_set_width(lab_h1, 80);
    lv_obj_align(lab_h1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_x(lab_h1, 0);
    lv_obj_set_y(lab_h1, 60);
    
    while(1)
    {
        lv_task_handler();
        
        kv4 = Key_Get(k4);
        kv5 = Key_Get(k5);
        
//        if(kv4 == Key_State_Short)
        if(Key_Read_Level(k4) == k4->Level)
        {
            Motor_Open();
            FW_Delay_Ms(500);
            Motor_Stop();
        }
//        else if(kv5 == Key_State_Short)
        else if(Key_Read_Level(k5) == k5->Level)
        {
            Motor_Close();
            FW_Delay_Ms(500);
            Motor_Stop();
        }
        else
        {
        
        }
        
        FW_ADC_Read(ha0, &HA0_Value, 1);
        FW_ADC_Read(ha1, &HA1_Value, 1);
        
        lv_obj_set_style_text_color(label, lv_color_make(0x00, 0x00, 0xFF), LV_STATE_DEFAULT);
        lv_label_set_text(label, "滴速控制");  //LVGL的编码格式为UTF-8，需要将MDK的Encoding设置为UTF-8才可以正常显示
//        lv_label_set_text_fmt(label, "Drop=%d", IPara.Drop_Rate.Count);
        lv_label_set_text_fmt(lab_h0, "%d", HA0_Value);
        lv_label_set_text_fmt(lab_h1, "%d", HA1_Value);
        
        FW_Delay_Ms(20);
    }
}

#else

#include "key.h"
#include "fw_system.h"
#include "fw_uart.h"

void Test(void)
{
    u16 VCC_EN = PA2;
    Key_Type K1, K2;
    u8 kv, kv2;
    u8 cnt = 0;
    
    FW_System_Init();
    FW_Delay_Init();

    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_SET(VCC_EN);
    
    Key_DeInit(&K1);
    K1.Pin = PA0;
    K1.Level = LEVEL_L;
    Key_Init(&K1);
    
    Key_DeInit(&K2);
    K2.Pin = PB5;
    K2.Level = LEVEL_H;
    Key_Init(&K2);
    
    FW_UART_PrintInit(PB13, 9600);
    
    printf("系统上电\r\n");
    
    while(1)
    {
        kv = Key_Get(&K1);
        kv2 = Key_Get(&K2);
        
        if(cnt++ >= 100)
        {
            cnt = 0;
            printf("运行正常, state = %d\r\n", kv);
        }
        
        switch(kv2)
        {
            case Key_State_Short:
                printf("短按\r\n");
                break;
            
            case Key_State_Long:
                printf("长按\r\n");
                break;
            
            case Key_State_KeepPress:
                printf("持续按下");
                break;
                
            default:
                break;
        }
        
        switch(kv)
        {
            case Key_State_Short:
                printf("短按\r\n");
                break;
            
            case Key_State_Long:
                while(Key_Read_Level(&K1) == K1.Level)
                {
                    FW_Delay_Ms(100);
                }
                printf("长按，VCC_EN=0\r\n");
                FW_GPIO_CLR(VCC_EN);
                while(1);
                break;
            
            case Key_State_KeepPress:
                while(Key_Read_Level(&K1) == K1.Level)
                {
                    FW_Delay_Ms(100);
                }
                printf("持续按下，VCC_EN=0\r\n");
                FW_GPIO_CLR(VCC_EN);
                while(1);
                break;
                
            default:
                break;
        }
        
        FW_Delay_Ms(10);
    }
}

#endif


#endif

