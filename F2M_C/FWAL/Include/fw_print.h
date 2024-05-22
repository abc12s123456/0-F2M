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

#ifndef _FW_PRINT_H_
#define _FW_PRINT_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_define.h"
#include "fw_debug.h"
#include <stdio.h>


/*
 * @log color / mode
 */
#if (ITM_LOG_EN == ON)
#define LOG_MODE(n)          ""
#define LOG_COLOR(n)         ""
#define LOG_COLOR_L(n)       ""
#else
#define LOG_MODE(n)          "\033["#n"m"
#define LOG_COLOR(n)         "\033[0;"#n"m"
#define LOG_COLOR_L(n)       "\033[1;"#n"m"
#endif

#define LOG_BLACK            LOG_COLOR(0)         /* 黑 */
#define LOG_RED              LOG_COLOR(1)         /* 红 */
#define LOG_GREEN            LOG_COLOR(2)         /* 绿 */
#define LOG_YELLOW           LOG_COLOR(3)         /* 黄 */
#define LOG_BLUE             LOG_COLOR(4)         /* 蓝 */
#define LOG_PURPLE           LOG_COLOR(5)         /* 紫 */
#define LOG_CYAN             LOG_COLOR(6)         /* 青 */
#define LOG_WHITE            LOG_COLOR(7)         /* 白 */

#define LOG_BLACK_L          LOG_COLOR_L(0)       /* 浅黑 */
#define LOG_RED_L            LOG_COLOR_L(1)       /* 浅红 */
#define LOG_GREEN_L          LOG_COLOR_L(2)       /* 浅绿 */
#define LOG_YELLOW_L         LOG_COLOR_L(3)       /* 浅黄 */
#define LOG_BLUE_L           LOG_COLOR_L(4)       /* 浅蓝 */
#define LOG_PURPLE_L         LOG_COLOR_L(5)       /* 浅紫 */
#define LOG_CYAN_L           LOG_COLOR_L(6)       /* 浅青 */
#define LOG_WHITE_L          LOG_COLOR_L(7)       /* 浅白 */

#define LOG_DEFAULT          LOG_MODE(0)          /* 默认 */
#define LOG_BOLD             LOG_MODE(1)          /* 粗体/高亮 */
#define LOG_UNDERLINE        LOG_MODE(4)          /* 单下划线 */
#define LOG_BLINK            LOG_MODE(5)          /* 闪烁 */
#define LOG_REVERSE          LOG_MODE(7)          /* 反显 */
#define LOG_HIDE             LOG_MODE(8)          /* 隐藏 */
#define LOG_CLEAR            LOG_MODE(2J)         /* 清屏 */
#define LOG_CLRLINE          LOG_MODE(1K\r)       /* 清除光标至行尾的内容 */


/*
 * @log function
 */
#if ((ITM_LOG_EN == ON) || (HW_LOG_EN == ON))
#define Printf(format, ...)       printf(format, ##__VA_ARGS__)
#define Log(format, ...)          printf("file:" __FILE__ "(%d):" format, __LINE__, ##__VA_ARGS__)

void Log_Array(const char *format, void *pdata, u32 num);
#else
#define Printf(format, ...)
#define Log(format, ...)
#define Log_Array(format, pdata, num)
#endif


#if (LOG_COLOR_EN == ON)
#define LOG_HDR(level, color, format, ...)\
        Printf(color "-" #level);\
        Printf(" [%s]: ", __FUNCTION__);\
        Printf(format, ##__VA_ARGS__);\
        Printf(LOG_DEFAULT)
#else
#define LOG_HDR(level, color, format, ...)
#endif


#if (LOG_LEVEL >= LOG_LEVEL_DEBUG)
#define LOG_D(format, ...)  LOG_HDR(D, LOG_BLACK, format, ##__VA_ARGS__)
#else
#define LOG_D(format, ...)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_INFO)
#define LOG_I(format, ...)  LOG_HDR(I, LOG_GREEN, format, ##__VA_ARGS__)
#else
#define LOG_I(format, ...)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_WARN)
#define LOG_W(format, ...)  LOG_HDR(W, LOG_YELLOW, format, ##__VA_ARGS__)
#else
#define LOG_W(format, ...)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_ERROR)
#define LOG_E(format, ...)  LOG_HDR(E, LOG_RED, format, ##__VA_ARGS__)
#else
#define LOG_E(format, ...)
#endif


extern int (*fw_printf)(const char *format, ...);

void FW_Console_Map(int (*fp)(const char *, ...));
void FW_Console_Close(void);


#ifdef __cplusplus
}
#endif

#endif
