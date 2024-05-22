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
 
#ifndef _KEY_H_
#define _KEY_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef enum
{
    Key_State_Idle = 0,              //空闲
//    Key_State_Debounce,              //消抖
//    Key_State_PressDown,             //按下(若长时间一直保持，表示按键未释放)
    Key_State_Release,               //释放
//    Key_State_PrePressDown,          //按下前的长按状态
    Key_State_Short,                 //短按
    Key_State_Press,                 //按下
    Key_State_Double,                //双击
    Key_State_Long,                  //长按
    Key_State_KeepPress,             //保持按下
}Key_State_Enum;


typedef struct
{
    u8 *Buffer;
    u8 Head;
    u8 Tail;
    u8 Size;
}KB_Type;


typedef struct Key
{
    FW_Device_Type Device;
    
    __RO_ u32 Start_Time;
    
    void (*Init)(struct Key *dev);
    u8   (*Read)(struct Key *dev);
    
    u32  Pin : 16;
    u32  Level : 1;
    
    __RO_ u32 Export_State : 4;
    __RO_ u32 Internal_State : 4;
    
    __RO_ u32 Cur_Value : 1;
    __RO_ u32 Pre_Value : 1;
    
    __RO_ u32 Debounce_Flag : 1;
    
    __RO_ u32 Equal_Flag : 1;
    __RO_ u32 Unequal_Flag : 1;
    
    __RO_ u32 Short_Flag : 1;
    __RO_ u32 Double_Flag : 1;
    __RO_ u32 Long_Flag : 1;
    __RO_ u32 LPress_Flag : 1;
    
    u32  Long_Time : 16;           //长按键的计时时间，单位：ms
    
    u32  : 14;
}Key_Type;


void Key_DeInit(Key_Type *dev);
void Key_Init(Key_Type *dev);

u8   Key_Get(Key_Type *dev);
u8   Key_Async_Get(Key_Type *dev);

u8   Key_Read_Level(Key_Type *dev);


#ifdef __cplusplus
}
#endif

#endif

