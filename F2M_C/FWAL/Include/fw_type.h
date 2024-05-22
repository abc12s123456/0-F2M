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
 
#ifndef _FW_TYPE_H_
#define _FW_TYPE_H_

#ifdef __cplusplus
extern "C"{
#endif


/*
 * @attribute/inline/static/volatile……
 */
#if defined(__CC_ARM) || defined(__CLANG_ARM)

    #define __SECTION_(x)        __attribute__((section(x)))
    #define __UNUSED_            __attribute__((unused))
    #define __USED_              __attribute__((used))
    #define __WEAK_              __attribute__((weak))
    
    #define __ALIGN_(n)          __attribute__((aligned(n)))
    #define __ALIGN_TYPE_(type)  __attribute__((aligned(sizeof(type))))
    #define __ALIGN_SIZE_(size)  __attribute__((aligned(size)))
    #define __ALIGN_BYTE_(num)   __ALIGN_(num)
    
    #define __AT_(addr)          __attribute__((at(addr)))
    
    #define __STATIC_            static
    #define __INLINE_            __inline
    #define __INLINE_STATIC_     __inline static
    #define __IO_                volatile
    #define __IO_STATIC_         volatile static
    #define __CONST_             const
    #define __CONST_STATIC_      const static
    #define __readonly_          const static

    #define __REG_               register
    
    #define __ASM_               __asm
    
//    #define __section_(x)        __attribute__((section(x)))
//    #define __unused_            __attribute__((unused))
//    #define __used_              __attribute__((used))
//    #define __weak_              __attribute__((weak))
//    
//    #define __align_(n)          __attribute__((aligned(n)))
//    #define __align_type_(type)  __attribute__((aligned(sizeof(type))))
//    #define __align_size_(size)  __attribute__((aligned(size)))
//    #define __align_byte_(num)   __align_(num)
//    
//    #define __at_(addr)          __attribute__((at(addr)))
//    
//    #define __static_            static
//    #define __inline_            __inline
//    #define __inline_static_     __inline static
//    #define __io_                volatile
//    #define __io_static_         volatile static
//    #define __const_             const
//    #define __const_static_      const static

//    #define __register_          register
//    
//    #define __asm_               __asm
    
#elif defined(__IAR_SYSTEMS_ICC__)

#elif defined(__GNUC__)

#elif defined(__TI_COMPILER_VERSION__)

#else
    #error  unsupported tool chain
#endif

    

/*
 * @数据类型定义
 */
#if defined(__CC_ARM) || defined(__CLANG_ARM)

	typedef unsigned char         uchar;
	typedef unsigned char         u8;
	typedef unsigned short        u16;
	typedef unsigned int          u32;
	typedef unsigned long long    u64;

	typedef          char         schar;
	typedef signed   char         s8;
	typedef signed   short        s16;
	typedef signed   int          s32;
	typedef signed   long long    s64;

    typedef          float        f32;
    typedef          long double  f64;

    typedef unsigned int          ptr_t;
#elif defined(__IAR_SYSTEMS_ICC__)

#elif defined(__GNUC__)

#elif defined(__TI_COMPILER_VERSION__)

#else
    #error  unsupported tool chain
#endif


/*
 * @数据类型最小值
 */
#define S8_MIN                    (-128)
#define S16_MIN                   (-32768)
#define S32_MIN                   (~0x7FFFFFFF)
#define S64_MIN                   (~0x7FFFFFFFFFFFFFFF)

#define U8_MIN                    (0)
#define U16_MIN                   (0)
#define U32_MIN                   (0)
#define U64_MIN                   (0)

#define FLOAT_MIN                 (1.175494e-38f)
#define DOUBLE_MIN                ()

/*
 * @数据类型最大值
 */
#define S8_MAX                    (0x7F)
#define S16_MAX                   (0x7FFF)
#define S32_MAX                   (0x7FFFFFFF)
#define S64_MAX                   (0x7FFFFFFFFFFFFFFF)

#define U8_MAX                    (0xFFU)
#define U16_MAX                   (0xFFFFU)
#define U32_MAX                   (0xFFFFFFFFU)
#define U64_MAX                   (0xFFFFFFFFFFFFFFFFULL)

#define FLOAT_MAX                 (3.402823e+38f)
#define DOUBLE_MAX                ()

/*
 * @无效值
 */
__INLINE_STATIC_ u32 Loop(u32 x)  {while(x){}; return 0;}
#define INVALUE                   Loop(1)


/*
 * @空值
 */
#ifndef NULL
#define NULL                      0U
#endif


/*
 * @空指针
 */
#ifndef PNULL
#define PNULL                     ((void *)NULL)
#endif


/*
 * @进制
 */
#define HEX                       16
#define DEC                       10
#define OCT                       8
#define BIN                       2


typedef enum
{
    Base_BIN = 2,
    Base_OCT = 8,
    Base_DEC = 10,
    Base_HEX = 16,
}Base_Enum;


/*
 * @布尔量
 */
typedef enum
{
    False = 0U,                   /* 假 */
    True = 1U                     /* 真 */
}Bool;


/*
 * @三态值
 */
typedef enum
{
    Negative = -1,                /* 负 */
    Tristate_L = -1,
    
    Zero = 0,                     /* 零 */
    Tristate_Z = 0,
    
    Positive = 1,                 /* 正 */
    Tristate_H = 1,
}Tristate;


/**
 * @状态标识
 */
typedef enum
{
    Disable = 0,
    Funstate_DI = 0,              /* 失能 */
    
    Enable = 1,
    Funstate_EN = 1,              /* 使能 */
}Funstate;


/*
 * @发送or接收
 */
typedef enum
{
    TOR_TX = 0,
    TOR_RX = 1,
}TOR;


/*
 * @读写
 */
typedef enum
{
    RW_Read = 0,
    RW_Write = 1,
}RWMode, RWM;


/*
 * @传输模式
 */
typedef enum
{
    TRM_UNK = 0,                  /* 未知 */
    TRM_POL = 1,                  /* 轮询 */
    TRM_INT = 2,                  /* 中断 */
    TRM_DMA = 3,                  /* DMA */
}TRMode, TRM;


/*
 * @关键字类型
 */
typedef enum
{
    Key_Int = 0,                  /* 整型 */
    Key_String,                   /* 字符串 */
    Key_Float,                    /* 单精度浮点型 */
    Key_Double,                   /* 双精度浮点型 */
}Key_Enum;


/*
 * @Modem执行状态
 */
typedef enum
{
    Modem_State_Cancel     = -5,  //发送方取消
    Modem_State_ResendErr  = -4,  //重发超限
    Modem_State_TimeoutErr = -3,  //接收超时
    Modem_State_FileErr    = -2,  //文件读写出错
    Modem_State_DataErr    = -1,  //数据错误
    
    Modem_State_OK = 0,           //协议执行完成
    Modem_State_Start = 1,        //处理起始帧
    Modem_State_Data,             //处理数据帧
    Modem_State_EOT,              //文件传输完成
    Modem_State_End,              //处理结束帧
}Modem_State_Enum;


/*
 * @常用的宏定义
 */
#ifndef OFF
#define OFF            0U         /* 关 */
#endif

#ifndef ON
#define ON             1U         /* 开 */
#endif

#ifndef LEVEL_L
#define LEVEL_L        0U         /* 高电平 */
#endif

#ifndef LEVEL_H
#define LEVEL_H        1U         /* 高电平 */
#endif

#ifndef NEGATIVE 
#define NEGATIVE       (-1)       /* 负 */
#endif

#ifndef HIZ
#define HIZ            0          /* 高阻 */
#endif

#ifndef POSITIVE
#define POSITIVE       1          /* 正 */
#endif

#ifndef LSB
#define LSB            0U         /* 低位 */
#endif

#ifndef MSB
#define MSB            1U         /* 高位 */
#endif

#define ENDIAN_LITTLE  0U         /* 小端模式 */
#define ENDIAN_BIG     1U         /* 大段模式 */

#define SOFTWARE       0U         /* 软件 */
#define HARDWARE       1U         /* 硬件 */

#define STATE_OK       0U         /* 状态正常 */
#define STATE_ERR      1U         /* 状态错误 */
                                  /* 可以继续添加其它状态 */

/*
 * @无穷小常量
 */
#define EPS_1_         (1e-1)
#define EPS_2_         (1e-2)
#define EPS_3_         (1e-3)
#define EPS_4_         (1e-4)
#define EPS_5_         (1e-5)
#define EPS_6_         (1e-6)
#define EPS_7_         (1e-7)
#define EPS_8_         (1e-8)
#define EPS_9_         (1e-9)
#define EPS_10_        (1e-10)
#define EPS_N(n)       EPS_##n##_
#define EPS            EPS_N(6)   /* 默认的无穷小量 */
                                  /* 可以继续添加其它状态 */

#ifndef PI
#define PI             (3.1415926f)
#endif


/*
 * @定义一块存储区域(内部RAM或内部Flash)，作为用户访问空间
 */
#define MPA_DEFINE(start_addr, size)\
    const u8 MPA_Array[size]  __AT_(start_addr) = {0}


/*
 * @获取数组的长度
 */
#define ARRAY_LENGTH(array)  (sizeof(array) / sizeof(array[0]))
#define AL_GET(array)        (sizeof(array) / sizeof(array[0]))

#define Arrayof(array)       (sizeof(array) / sizeof(array[0]))

/*
 * @日志级别，DEBUG的优先级最高，表示此时会输出所有LOG信息
 */
#define LOG_LEVEL_ERROR           0                        /* 错误日志，最低优先级日志级别 */
#define LOG_LEVEL_WARN            1                        /* 警告日志 */
#define LOG_LEVEL_INFO            2                        /* 信息日志 */
#define LOG_LEVEL_DEBUG           3                        /* 调试日志 */


/*
 * @CRC定义
 */
#define CRC_METHOD_LT             0                        /* 查表法 */
#define CRC_METHOD_CAL            1                        /* 计算法 */

#define CRC32                     0
#define CRC32_MPEG2               1

#define CRC16_IBM                 0
#define CRC16_MAXIM               1
#define CRC16_USB                 2
#define CRC16_MODBUS              3
#define CRC16_CCITT               4
#define CRC16_CCITT_FALSE         5
#define CRC16_X25                 6
#define CRC16_XMODEM              7
#define CRC16_DNP                 8

#define CRC8                      0
#define CRC8_ITU                  1
#define CRC8_ROHC                 2
#define CRC8_MAXIM                3


#ifdef __cplusplus
}
#endif

#endif
