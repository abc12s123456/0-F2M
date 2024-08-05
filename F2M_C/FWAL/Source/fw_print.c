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
#include "fw_print.h"

#include <string.h>
#include <stdarg.h>

#include "mm.h"


#if (ITM_LOG_EN == ON && HW_LOG_EN == OFF)                                       /* ITM需要连接SWO */

#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000 + 4 * n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000 + 4 * n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000 + 4 * n)))
#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000
//#define TRCENA          0x00000001

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;
    
int fputc(int ch, FILE *f) 
{
    if(DEMCR & TRCENA) 
    {
        while(ITM_Port32(0) == 0);
        ITM_Port8(0) = ch;
    }
    return(ch);
}

#endif  //(ITM_LOG_EN == ON && HW_LOG_EN == OFF)

#if (ITM_LOG_EN == ON || HW_LOG_EN == ON)

/*
%.2f表示输出的浮点类型强制保留两位小数，不够用0补齐，超出四舍五入；
%.2d表示当输出整型宽度少于2时，输出右对齐，左边用0补齐，宽度超出2则原样输出；
*/
void Log_Array(const char *format, void *pdata, u32 num)
{
    if(strcmp(format, "%.2D") == 0 || strcmp(format, "%2d") == 0 ||
       strcmp(format, "%.2X") == 0 || strcmp(format, "%2x") == 0)
    {
        u8 *p = (u8 *)pdata;
        while(num--)
        {
            Printf(format, *p++);
            Printf(" ");
        }
    }
    else if(strcmp(format, "%.4D") == 0 || strcmp(format, "%4d") == 0 ||
            strcmp(format, "%.4X") == 0 || strcmp(format, "%4x") == 0)
    {
        u16 *p = (u16 *)pdata;
        while(num--)
        {
            Printf(format, *p++);
            Printf(" ");
        }
    }
    else if(strcmp(format, "%.8D") == 0 || strcmp(format, "%8d") == 0 ||
            strcmp(format, "%.8X") == 0 || strcmp(format, "%8x") == 0)
    {
        u32 *p = (u32 *)pdata;
        while(num--)
        {
            Printf(format, *p++);
            Printf(" ");
        }
    }
    else
    {
        char *p = (char *)pdata;
        while(num--)
        {
            Printf(format, *p++);
        }
    }
    Printf("\r\n");
}

#endif  //(ITM_LOG_EN == ON || HW_LOG_EN == ON)


/**
@功能：fw_printf
@参数：format，输出格式
       ...，不定参数
@返回：int
@说明：1、允许用户动态开关控制台printf操作；
       2、多任务时，关联的映射需要支持互斥访问；
*/
__INLINE_STATIC_ int FW_Print_NULL(const char *format, ...){return 0;}

int (*fw_printf)(const char *format, ...) = FW_Print_NULL;

void FW_Console_Map(int (*fp)(const char *, ...))
{
    if(fp != NULL)  fw_printf = fp;
}

void FW_Console_Close(void)
{
    fw_printf = FW_Print_NULL;
}
/**/


/**
@功能：Assert错误打印信息
@参数：file, 文件名
       line, 行数
@返回：无
@说明：1、ASSERT_FULL有定义时才会调用
       2、优先使用用户定义的实现
*/
__WEAK_ void Assert_Failed(u8 *file, u32 line)
{
    Printf("Assert failed in file:%s, line:%d", (char *)file, line);
}
/**/

