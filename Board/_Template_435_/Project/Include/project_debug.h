#ifndef _PROJECT_DEBUG_H_
#define _PROJECT_DEBUG_H_

#ifdef __cplusplus
extern "C"{
#endif


#ifndef ON
#define ON                        1U
#endif

#ifndef OFF
#define OFF                       0U
#endif


/*
 * @Debug功能，用于实际项目中发布模式与调试模式之间的切换
 */
#define DEBUG_EN                  ON


#define LOG_LEVEL                 LOG_LEVEL_DEBUG                               /* 日志级别 */
#define LOG_COLOR_EN              ON                                            /* 日志显示信息使用彩色功能，需调试工具支持 */


/*
 * @S2RAM功能，可部分模拟BKUP功能
 */
#define S2RAM_EN                  OFF


/*
 * @程序起始地址
 */
#define IROM1_BASE                0x08000000
#define IROM1_SIZE                0x00040000

/* 需要使用S2RAM功能时，可根据具体的存储进行RAM分配；不需要时，将IRAM2_SIZE设置为0即可 */
#if defined(S2RAM_EN) && (S2RAM_EN)

#define IRAM1_BASE                0x20000000
#define IRAM1_SIZE                0x0000B000

#define IRAM2_BASE                0x2000B000
#define IRAM2_SIZE                0x00001000

#else

#define IRAM1_BASE                0x20000000
#define IRAM1_SIZE                0x0000C000

#define IRAM2_BASE                0x2000C000
#define IRAM2_SIZE                0x00000000

#endif  /* #if defined(S2RAM_EN) && (S2RAM_EN) */


#if (DEBUG_EN == OFF)
#define IROM1_OFFSET              0x00009000
#define IRAM1_OFFSET              0x00000000
#define IRAM2_OFFSET              0x00000000
#else
#define IROM1_OFFSET              0x00000000
#define IRAM1_OFFSET              0x00000000
#define IRAM2_OFFSET              0x00000000
#endif


#define IROM1_START_ADDR          (IROM1_BASE + IROM1_OFFSET)
#define IROM1_REMAIN              (IROM1_SIZE - IROM1_OFFSET)

#define IRAM1_START_ADDR          (IRAM1_BASE + IRAM1_OFFSET)
#define IRAM1_REMAIN              (IRAM1_SIZE - IRAM1_OFFSET)

#define IRAM2_START_ADDR          (IRAM2_BASE + IRAM2_OFFSET)
#define IRAM2_REMAIN              (IRAM2_SIZE - IRAM2_OFFSET)


/*
 * @module test
 */
#define MODULE_TEST               ON                                           /* 软件模块测试 */

#define ADC_TEST                  OFF                                           /* ADC模块测试 */
#define BKP_TEST                  OFF                                           /* BKP模块测试 */
#define BUS_TEST                  OFF                                           /* Bus模块测试 */
#define CAN_TEST                  OFF                                           /* CAN模块测试 */
#define DAC_TEST                  OFF                                           /* DAC模块测试 */
#define DELAY_TEST                OFF                                           /* Delay模块测试 */
#define ETH_TEST                  OFF                                           /* ETH模块测试 */
#define EXTI_TEST                 OFF                                           /* EXTI模块测试 */
#define FLASH_TEST                OFF                                           /* 片上Flash模块测试 */
#define GPIO_TEST                 ON                                           /* GPIO模块测试 */
#define I2C_TEST                  OFF		                                    /* I2C模块测试 */
#define KEY_TEST                  OFF                                           /* 按键模块测试 */
#define LCD_TEST                  OFF                                           /* LCD驱动模块测试 */
#define LED_TEST                  OFF                                           /* LED驱动模块测试 */
#define MM_TEST                   OFF                                           /* 内存管理测试 */
#define OS_TEST                   OFF                                           /* OS测试 */
#define PMU_TEST                  OFF                                           /* PWR模块测试 */
#define RTC_TEST                  OFF                                           /* RTC模块测试 */
#define S2RAM_TEST                OFF                                           /* S2RAM模块测试 */
#define SPI_TEST                  OFF                                           /* SPI模块测试 */
#define SYSTEM_TEST               OFF		                                    /* System模块测试 */
#define UART_TEST                 OFF                                           /* UART模块测试 */
#define SUART_TEST                OFF                                           /* 模拟UART模块测试 */
#define USB_TEST                  OFF                                           /* USB模块测试 */
#define WDG_TEST                  OFF                                           /* WDT模块测试 */


/*
 * @Sensor
 */
#define SENSOR_TEST               OFF
#if SENSOR_TEST == ON
#define WF200D_TEST               OFF
#define WF183D_TEST               OFF
#define DS18B20_TEST              ON
#endif  /* Sensor_TEST */


/*
 * @WiFi
 */
#define WIFI_TEST                 OFF
#if WIFI_TEST == ON
#define ESP8266_TEST              ON
#define BW16_TEST                 OFF
#endif  /* WIFI_TEST */


/*
 * @PWM
 */
#define PWM_TEST                  OFF
#if PWM_TEST == ON
#define TIMER_TEST                ON
#endif  /* PWM_TEST */


/*
 * @LIB
 */
#define LIB_TEST                  OFF
#if LIB_TEST == ON
#define PID_TEST                  ON
#endif  /* LIB_TEST */


/*
 * @算法
 */
#define ALGO_TEST                 OFF                                           /* Algorithm模块测试 */
#if ALGO_TEST == ON
#define SORT_TEST                 OFF                                           /* 排序算法测试 */
#define BINARY_SEARCH_TEST        OFF                                           /* 二分查找测试 */
#define AVLTREE_TEST              OFF                                           /* AVL查找树测试 */
#define FFT_TEST                  OFF                                            /* 快速傅里叶变换测试 */
#define RESP_TEST                 ON
#endif  /* ALGO_TEST */


/*
 * @总线设备
 */
#define BUS_DEVICE_TEST           OFF                                           /* 总线驱动设备测试 */
#if BUS_DEVICE_TEST == ON

#endif  /* BUS_DEVICE_TEST */


/*
 * @I2C设备
 */
#define I2C_DEVICE_TEST           OFF                                           /* I2C驱动设备测试 */
#if I2C_DEVICE_TEST == ON
#define TM1727_TEST               OFF                                           /* TM1727模块测试 */
#define SCS1_TEST                 OFF                                           /* 定制段码屏显示模块测试 */
#define I2C_24CXX_TEST            OFF                                           /* 24Cxx系列E2PROM测试 */
#define P24C64_TEST               ON                                            /* P24C64测试 */
#endif  /* I2C_DEVICE_TEST */


/*
 * @IO设备
 */
#define IO_DEVICE_TEST            OFF                                           /* IO驱动设备测试 */
#if IO_DEVICE_TEST == ON
#define HC165_TEST                OFF                                           /* HC165模块测试 */
#define YX3P_TEST                 OFF                                           /* YX3P语音芯片驱动测试 */
#define TM7711_TEST               OFF                                           /* ADC转换芯片驱动测试 */
#define STSPIN220_TEST            OFF                                           /* STSPIN220电机驱动芯片驱动测试 */
#define ILI9341_TEST              OFF                                           /* ILI9341 LCD模块测试 */
#define QENCODER_TEST             OFF                                           /* 正交编码器测试 */
#define GYY0709_TEST              ON
#endif  /* IO_DEVICE_TEST */


/*
 * @SPI设备
 */
#define SPI_DEVICE_TEST           ON                                           /* SPI驱动设备测试 */
#if SPI_DEVICE_TEST == ON
#define GD25X_TEST                OFF                                           /* GD25Q SPI系列外部nor flash测试 */
#define FM17522_TEST              OFF                                           /* RFID测试 */
#define FATFS_TEST                OFF                                           /* FatFs测试 */
#define ST7735_TEST               ON
#endif  /* SPI_DEVICE_TEST */


/*
 * @UART设备
 */
#define UART_DEVICE_TEST          OFF                                           /* UART驱动设备测试 */
#if UART_DEVICE_TEST == ON
#define BO2_TEST                  OFF                                           /* 血氧模块测试 */
#define LORA610_TEST              OFF                                           /* LORA610模块测试 */
#define ESP8266_TEST              OFF                                           /* ESP8266模块测试 */
#define OSC3F_TEST                OFF                                           /* OSC3F传感器模块测试 */
#endif  /* UART_DEVICE_TEST */


/*
 * @通讯协议
 */
#define PROTOCOL_TEST             OFF                                           /* 协议测试 */
#if PROTOCOL_TEST == ON
#define SMODEM_MASTER_TEST        ON
#define SMODEM_SLAVE_TEST         OFF
#endif  /* PROTOCOL_TEST */


/*
 * @文件系统
 */
#define FS_TEST                   OFF
#if FS_TEST == ON
#define FATFS_TEST                OFF
#define LFS_TEST                  ON
#endif  /* FS_TEST */


/*
 * @其它
 */
#define OTHER_TEST                OFF
#if OTHER_TEST == ON


#endif  /* OTHER_TEST */


//#define TASK_LAYER_TEST           OFF
//#define ZS_UI_TEST                OFF
//#define BERN_TEST                 OFF

//#define BLN_LED_TEST              OFF

//#define MOTOR_TEST                OFF

//#define GUI_TEST                  OFF
//#define TC4056_TEST               OFF
//#define FLOW_TEST                 OFF

//#define COLOR_TEST                OFF

//#define RBTREE_TEST               OFF                                           /* 红黑树测试 */

//#define ST77_TEST                 OFF

//#define JSON_TEST                 OFF


#ifdef __cplusplus
}
#endif

#endif

