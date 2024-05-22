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
 
#ifndef _IP5310_H_
#define _IP5310_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef enum
{
    IP5310_Residual_L0 = 0x00,    //c <= 3%
    IP5310_Residual_L1 = 0x01,    //3% < c <= 25%
    IP5310_Residual_L2 = 0x03,    //25% < c <= 50%
    IP5310_Residual_L3 = 0x07,    //50% < c <= 75%
    IP5310_Residual_L4 = 0x0F,    //c > 75%
    IP5310_Residual_Unknown = 0xEE,
}IP5310_Residual_Enum;


typedef enum
{
    IP5310_State_Stop = 0,        //未充电
    IP5310_State_Charge,          //正在充电
    IP5310_State_Done,            //已充满
    IP5310_State_Unknown,
}IP5310_State_Enum;


typedef enum
{
    IP5310_Load_Heavy = 0,
    IP5310_Load_Light = 1,
    IP5310_Load_Unknown = 2,
}IP5310_Load_Enum;


typedef struct
{
    FW_Device_Type Device;
    
    u32 WKP_Pin : 16;
    u32 WKP_VL : 1;
    
    u32 : 15;
    
    
}IP5310_Type;


void IP5310_Init(IP5310_Type *dev);

u8   IP5310_Get_Residual(IP5310_Type *dev);
u8   IP5310_Get_State(IP5310_Type *dev);
u8   IP5310_Get_Load(IP5310_Type *dev);


#ifdef __cplusplus
}
#endif

#endif

