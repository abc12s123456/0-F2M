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
 
#ifndef _RBUFFER_H_
#define _RBUFFER_H_

#ifdef __cplupsplus
extern "C"{
#endif


#include "fw_define.h"


/*
RB将任何数据都视为字节流，以字节为单位去进行操作
*/
typedef struct
{
    u8  *Buffer;
    u32 Size;
    
    volatile u32 Head_Index;
    volatile u32 Tail_Index;
    
    u32 Full_Flag : 1;
    u32 Dynamic_Flag : 1;
    
    u32 : 30;
}RB_Type;


RB_Type *RB_Create(u32 size);
void RB_Delete(RB_Type *rb);

Bool RB_Init(RB_Type *rb, u32 size);

u8   *RB_Get_Buffer(RB_Type *rb);
void RB_Clear_Buffer(RB_Type *rb);
void RB_Fill_Buffer(RB_Type *rb, u8 value);
u32  RB_Get_BufferSize(RB_Type *rb);
u32  RB_Get_DataLength(RB_Type *rb);

u32  RB_Write(RB_Type *rb, const u8 *pdata, u32 num);
u32  RB_Read(RB_Type *rb, u8 *pdata, u32 num);

u32  RB_Read_All(RB_Type *rb, u8 *pdata, u32 cntout);
u32  RB_Read_To(RB_Type *rb, const char *s, u8 *pdata, u32 cntout);
u32  RB_Read_Line(RB_Type *rb, u8 *pdata, u32 cntout);
u32  RB_Read_Inter(RB_Type *rb, const char *s1, const char *s2, u8 *pdata, u32 cntout);
u32  RB_Read_Fisrt(RB_Type *rb, const char *s, u8 *pdata, u32 num);
u32  RB_Read_HL(RB_Type *rb, const u8 head, u8 length_site, u8 other_num, u8 *pdata, u32 cntout);

char *RB_Find_String(RB_Type *rb, const char *s);
Bool RB_String_IsExist(RB_Type *rb, const char *s);



#ifdef __cplusplus
}
#endif

#endif

