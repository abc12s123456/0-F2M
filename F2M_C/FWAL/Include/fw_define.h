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
 
#ifndef _FW_DEFINE_H_
#define _FW_DEFINE_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"
#include "fw_config.h"
#include "libc.h"


/**
 * @这些定义没有实际的意义，只是表明参数的读写、功能属性
 */
#define __RO_                          //只读，由内部逻辑处理，不需要用户参与的变量
#define __WO_                          //只写，用户只能写入，不需要查看的变量
#define __WR_                          //读写，由用户写入，并且会经过内部逻辑处理的变量
#define __WM_                          //必须写入，用户必须要配置的变量(默认为可读写，不包括具有默认值的变量)
#define __WS_                          //可选择写入的变量(默认可读写，未写入时有默认值)
#define __RE_                          //预留
#define __UW_                          //用户写入

#define __LI_                          //底层中断函数



/**
 * @返回值
 */
typedef s32 fw_ret_t;

#define FW_ROK                                   (0x00)                         /* 操作成功 */
#define FW_RERR                                  (0x01)                         /* 发生错误 */
#define FW_RFAIL                                 (0x01)                         /* 操作失败 */
#define FW_RDEV_EXIST                            (0x02)                         /* 设备已存在 */
#define FW_RNODEV                                (0x03)                         /* 设备不存在 */
#define FW_RDEV_ADY_OPEN                         (0x04)                         /* 设备已打开 */
#define FW_RDEV_ADY_CLOSE                        (0x05)                         /* 设备已打开 */
#define FW_RTIMEOUT                              (0x06)                         /* 超时 */
#define FW_RNODRV                                (0x07)                         /* 无驱动 */


/**
 * @错误码
 */
typedef s32 fw_err_t;
 
#define FW_EOK                                   (0x00)                         /* 无错误 */
#define FW_ERR                                   (0x01)                         /* 发生通用错误 */
#define FW_ETIMEOUT                              (0x02)                         /* 超时 */
#define FW_ECNTOUT                               (0x03)                         /* 计数超限 */
#define FW_EFULL                                 (0x04)                         /* 溢出 */
#define FW_EEMPTY                                (0x05)                         /* 空 */
#define FW_ENOMEM                                (0x06)                         /* 无存储 */
#define FW_ENOSYS                                (0x07)                         /* 无系统 */
#define FW_EBUSY                                 (0x08)                         /* 忙 */
#define FW_EIO                                   (0x09)                         /* IO错误 */
#define FW_EINVAL                                (0x0A)                         /* 无效值错误 */
#define FW_EARGS                                 (0x0B)                         /* 参数错误 */


///**
// * @状态码
// */
//typedef s32 fw_sta_t;

//#define FW_SOK                                   (0x00)                         /* 正常 */
//#define FW_SFAIL                                 (0x01)                         /* 失败 */
//#define FW_STIMEOUT                              (0x02)                         /* 超时 */
//#define FW_SNODRV                                (0x03)                         /* 无驱动 */
//#define FW_SDEV_EXIST                            (0x04)                         /*  */                         


/**
 * @空值
 */
#define FW_NULL                                  (0x00)


/**
 * @offset of & container of
 */
#define Offset_Of(type, member)                  ((u32)&((type *)0)->member)
#define Container_Of(p, type, member)            ((type *)((u8 *)(p) - Offset_Of(type, member)))


/**
 * @Return the most contiguous size aligned at specified width. ALIGN_GET_SIZE(13, 4)
 * would return 16.
 */
#define ALIGN_GET_SIZE(size, align)              (((size) + (align) - 1) & ~((align) - 1))


/**
 * @Return the down number of aligned at specified width. ALIGN_GET_DOWN(13, 4)
 * would return 12.
 */
#define ALIGN_GET_DOWN(size, align)              ((size) & ~((align) - 1))


/**
 * @字符串拼接
 */
#if defined(__CC_ARM) || defined(__CLANG_ARM)

#define STRCAT(s1, s2)                           (s1##s2)

/**
 * @链接起止地址
 */
#define SECTION(section_name)                    __USED_ __SECTION_(section_name)

#define SECTION_START(section_name)              &STRCAT(section_name, $$Base)
#define SECTION_END(section_name)                &STRCAT(section_name, $$Limit)

#define SECTION_GET_START(section_name, type)    ((type *)SECTION_START(section_name))
#define SECTION_GET_END(section_name, type)      ((type *)SECTION_END(section_name))

/**
 * @链接声明
 */
#define SECTION_DECLARE(section_name, type)\
    extern const type *STRCAT(section_name, $$Base);\
    extern const void *STRCAT(section_name, $$Limit)

/**
 * @函数链接
 */
#define FUNCTION_LINK_DEFINE(name, type)         SECTION_DECLARE(name, type)


/**
 * @function x export
 */
typedef struct
{
    void (*PF)(void *);
    void *Pdata;
}FW_Function_Type;


#define FUNCTION_INIT_DECLARE(func)\
    SECTION("Function_Init_Table") const FW_Function_Type Init_##func = {func}
    
#define FUNCTION_REGIST_DECLARE(func)\
    SECTION("Function_Regist_Table") const FW_Function_Type Regist_##func = {func}


#define FW_XX_INIT(section_name, pf, pdata)\
    SECTION(section_name) const FW_Function_Type __##pf =\
    {\
        pf,\
        (void *)(pdata),\
    }

#elif defined(__IAR_SYSTEMS_ICC__)

#elif defined(__GNUC__)

#elif defined(__TI_COMPILER_VERSION__)

#else
    #error  unsupported any tool chain
#endif  /* defined(__CC_ARM) || defined(__CLANG_ARM) */


/**
 * @Init函数
 */
#define FW_SUB_INIT(pf, pdata)         FW_XX_INIT("FW_Sub_Init_Table", pf, pdata)
#define FW_COMP_INIT(pf, pdata)        FW_XX_INIT("FW_Component_Init_Table", pf, pdata)
#define FW_ENV_INIT(pf, pdata)         FW_XX_INIT("FW_ENV_Init_Table", pf, pdata)
#define FW_APP_INIT(pf, pdata)         FW_XX_INIT("FW_APP_Init_Table", pf, pdata)
#define FW_CLK_INIT(pf, pdata)         FW_XX_INIT("FW_Clock_Init_Table", pf, pdata)

#define FW_PRE_INIT(pf, pdata)         FW_XX_INIT("Pre_Init_Table", pf, pdata)
#define FW_SYSTEM_INIT(pf)             FW_XX_INIT("System_Init_Table", pf, NULL)
#define FW_DEV_INIT(pf, pdata)         FW_XX_INIT("Device_Init_Table", pf, pdata)
#define FW_BOARD_INIT(pf, pdata)       FW_XX_INIT("Board_Init_Table", pf, pdata)

/**
 * @Init函数声明
 */
#define FW_INIT_DECLARE(name, type)    SECTION_DECLARE(name, type)


/**
 * @Init函数链接
 */
#define FW_INIT_LINK(name, type)\
{\
    type *pstart, *pend, *pdev;\
    \
    pstart = SECTION_GET_START(name, type);\
    pend = SECTION_GET_END(name, type);\
    \
    if(pstart != NULL && pend != NULL);\
    {\
        for(pdev = pstart; pdev < pend; pdev++)\
        {\
            if(pdev->PF)  pdev->PF(pdev->Pdata);\
        }\
    }\
}


/**
 * @功能: 调用链接函数(FXE定义的)
 * @参数: name, FXER
 *        type, FXE_Type
 * @返回: 无
 * @说明: 无
 * @修改: 无
 */
#define FUNCTION_EXPORT_LINK(name, type)\
{\
    type *pstart, *pend, *pdev;\
    \
    pstart = SECTION_GET_START(name, type);\
    pend = SECTION_GET_END(name, type);\
    \
    if(pstart != NULL && pend != NULL);\
    {\
        for(pdev = pstart; pdev < pend; pdev++)\
        {\
            if(pdev->pf)  pdev->pf();\
        }\
    }\
}


/**
 * @功能: 函数重载的实现(形式上的)
 */
#if defined(VA_ARGS_NUM_MAX)

#if 1

#define ARG(x)                                        (x)

#if (VA_ARGS_NUM_MAX == 6)
#define __VA_ARGS_NUM(A0, A1, A2, A3, A4, N, ...)     ((N == 1) ? ((#A0)[0] != NULL) : N)
#define VA_NUM(...)                                   __VA_ARGS_NUM(__VA_ARGS__, 5, 4, 3, 2, 1, 0)
#elif (VA_ARGS_NUM_MAX == 5)
#define __VA_ARGS_NUM(A0, A1, A2, A3, N, ...)         ((N == 1) ? ((#A0)[0] != NULL) : N)
#define VA_NUM(...)                                   __VA_ARGS_NUM(__VA_ARGS__, 4, 3, 2, 1, 0)
#elif (VA_ARGS_NUM_MAX <= 4)
#define __VA_ARGS_NUM(A0, A1, A2, N, ...)             ((N == 1) ? ((#A0)[0] != NULL) : N)
#define VA_NUM(...)                                   __VA_ARGS_NUM(__VA_ARGS__, 3, 2, 1, 0)
#endif  /* #if (VA_ARGS_NUM_MAX == 6) */

#if (VA_ARGS_NUM_MAX >= 6)
#define __VA_ARG6(A0, A1, A2, A3, A4, A5, ...)        A5
#define VA6(...)   ARG(__VA_ARG6(__VA_ARGS__, NULL, NULL, NULL, NULL, NULL, NULL))
#endif

#if (VA_ARGS_NUM_MAX >= 5)
#define __VA_ARG5(A0, A1, A2, A3, A4, ...)            A4
#define VA5(...)   ARG(__VA_ARG5(__VA_ARGS__, NULL, NULL, NULL, NULL, NULL))
#endif

#if (VA_ARGS_NUM_MAX >= 4)
#define __VA_ARG3(A0, A1, A2, A3, ...)                A3
#define __VA_ARG2(A0, A1, A2, ...)                    A2
#define __VA_ARG1(A0, A1, ...)                        A1
#define __VA_ARG0(A0, ...)                            A0

#define VA4(...)   ARG(__VA_ARG4(__VA_ARGS__, NULL, NULL, NULL, NULL))
#define VA3(...)   ARG(__VA_ARG3(__VA_ARGS__, NULL, NULL, NULL))
#define VA2(...)   ARG(__VA_ARG2(__VA_ARGS__, NULL, NULL))
#define VA1(...)   ARG(__VA_ARG1(__VA_ARGS__, NULL))
#define VA0(...)   ARG(__VA_ARG0(__VA_ARGS__) + 0)
#endif  /* #if (VA_ARGS_NUM_MAX >= 4) */

__INLINE_STATIC_ int VA_ARGS_ERR(const char *info, ...)
{
    while(1);
}

#define VA_ARGS_NUM_ERR(...)  VA_ARGS_ERR("输入参数个数错误\r\n")

#else

#define ARG(x)                                        (x)

#if (VA_ARGS_NUM_MAX == 6)
#define __VA_ARGS_NUM(A0, A1, A2, A3, A4, A5, ...)    (A5)
#define VA_NUM(...)                                   __VA_ARGS_NUM(__VA_ARGS__, 5, 4, 3, 2, 1, 0)
#elif (VA_ARGS_NUM_MAX == 5)
#define __VA_ARGS_NUM(A0, A1, A2, A3, A4, ...)        (A4)
#define VA_NUM(...)                                   __VA_ARGS_NUM(__VA_ARGS__, 4, 3, 2, 1, 0)
#elif (VA_ARGS_NUM_MAX <= 4)
#define __VA_ARGS_NUM(A0, A1, A2, A3, ...)            (A3)
#define VA_NUM(...)                                   __VA_ARGS_NUM(__VA_ARGS__, 3, 2, 1, 0)
#endif  /* #if (VA_ARGS_NUM_MAX == 6) */

#if (VA_ARGS_NUM_MAX >= 6)
#define __VA_ARG6(A0, A1, A2, A3, A4, A5, ...)        A5
#define VA6(...)   ARG(__VA_ARG6(__VA_ARGS__, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
#endif

#if (VA_ARGS_NUM_MAX >= 5)
#define __VA_ARG5(A0, A1, A2, A3, A4, ...)            A4
#define VA5(...)   ARG(__VA_ARG5(__VA_ARGS__, NULL, NULL, NULL, NULL, NULL, NULL))
#endif

#if (VA_ARGS_NUM_MAX >= 4)
#define __VA_ARG3(A0, A1, A2, A3, ...)                A3
#define __VA_ARG2(A0, A1, A2, ...)                    A2
#define __VA_ARG1(A0, A1, ...)                        A1
#define __VA_ARG0(A0, ...)                            A0

#define VA4(...)   ARG(__VA_ARG4(__VA_ARGS__, NULL, NULL, NULL, NULL, NULL))
#define VA3(...)   ARG(__VA_ARG3(__VA_ARGS__, NULL, NULL, NULL, NULL))
#define VA2(...)   ARG(__VA_ARG2(__VA_ARGS__, NULL, NULL, NULL))
#define VA1(...)   ARG(__VA_ARG1(__VA_ARGS__, NULL, NULL))
#define VA0(...)   ARG(__VA_ARG0(__VA_ARGS__, NULL) + 0)
#endif  /* #if (VA_ARGS_NUM_MAX >= 4) */

__INLINE_STATIC_ int VA_ARGS_ERR(const char *info, ...)
{
    while(1);
}

#define VA_ARGS_NUM_ERR(...)  VA_ARGS_ERR("输入参数个数错误\r\n")

#endif


#endif  /* #if defined(VA_NUM_MAX) */





#ifdef __cplusplus
}
#endif

#endif

