//#include "ll_include.h"

//#include "fw_bus.h"
//#include "fw_gpio.h"


//#define EXMC_D0     PD14
//#define EXMC_D1     PD15
//#define EXMC_D2     PD0
//#define EXMC_D3     PD1
//#define EXMC_D4     PE7
//#define EXMC_D5     PE8
//#define EXMC_D6     PE9
//#define EXMC_D7     PE10
//#define EXMC_D8     PE11
//#define EXMC_D9     PE12
//#define EXMC_D10    PE13
//#define EXMC_D11    PE14
//#define EXMC_D12    PE15
//#define EXMC_D13    PD8
//#define EXMC_D14    PD9
//#define EXMC_D15    PD10

//#define EXMC_RD     PD4                           //NOE
//#define EXMC_WR     PD5                           //NWE
//#define EXMC_CS     PD7                           //NCE2
//#define EXMC_RS     PD11                          //RS，命令/数据标志(0:读写命令，1:读写数据)


//#define EXMC_A0_A25          //地址线A[0:25]，对应地址范围[0:0x4000000(2^26 = 64MB)]

///* PC Card引脚 */
//#define EXMC_CD              //PC卡存在检测信号，高有效
//#define EXMC_NREG            //决定访问通用空间还是属性空间
//#define EXMC_NIOS16          //仅适合16位传输的IO空间的数据传输宽度(必须接地)
//#define EXMC_NIOWR           //IO空间写使能
//#define EXMC_NIORD           //熊空间输出使能
//#define EXMC_NCE3_0          //NCE3_[0:1]，片选
//#define EXMC_NCE3_1
//#define EXMC_INTR            //PC卡中断输入信号

///* NAND引脚 */
//#define EXMC_INT1            //INT[1:2]，就绪/忙输入信号
//#define EXMC_INT2
//#define EXMC_NCE1            //NCE[1:2]，片选
//#define EXMC_NCE2

///* NOR/PSRAM/NAND共享引脚 */
//#define EXMC_NOE             //读使能
//#define EXMC_NWE             //写使能
//#define EXMC_NWAIT           //等待输入信号

///* NOR/PSRAM引脚 */
//#define EXMC_CLK             //同步时钟信号
//#define EXMC_NE0             //NE[0:3]，片选
//#define EXMC_NE1
//#define EXMC_NE2
//#define EXMC_NE3
//#define EXMC_NADV            //NL，地址有效/地址锁存信号

///* PSARM引脚 */
//#define EXMC_NBL0            //低字节使能
//#define EXMC_NBL1            //高字节使能



//__INLINE_STATIC_ void Bus_Data_IO_Init(void)
//{
//    //IO Init
//    FW_GPIO_Init(EXMC_D0, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D1, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D2, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D3, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D4, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D5, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D6, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D7, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D8, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D9, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D10, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D11, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D12, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D13, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D14, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_D15, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//}




///* 系统外部扩展总线 */
//static FW_Bus_Type EX_Bus;
//static void EX_Bus_Config(void *dev)
//{
//    /* EX_Bus作为一个共享访问的设备对象存在 */
//}
//FW_DEVICE_STATIC_REGIST("exbus", &EX_Bus, EX_Bus_Config, EX_Bus);




///* LCD_Bus：用来驱动LCD的总线 */
//__INLINE_STATIC_ void LCD_Bus_Init(FW_Bus_LCD_Type *dev)
//{
//    exmc_norsram_parameter_struct sram_init_struct;
//    exmc_norsram_timing_parameter_struct sram_timing_init_struct;
//    u32 EXMC_NOR_DATABUS_WIDTH;
//    
//    /* 相关IO初始化 */
//    Bus_Data_IO_Init();
//    
//    FW_GPIO_Init(EXMC_RD, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_WR, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_CS, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    FW_GPIO_Init(EXMC_RS, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    
//    if(dev->Bus_Width == FW_Bus_Width_8Bits)
//    {
//        EXMC_NOR_DATABUS_WIDTH = EXMC_NOR_DATABUS_WIDTH_8B;
//    }
//    else
//    {
//        EXMC_NOR_DATABUS_WIDTH = EXMC_NOR_DATABUS_WIDTH_16B;
//    }
//    
//    /* EXMC clock enable */
//    rcu_periph_clock_enable(RCU_EXMC);
//    
//    sram_timing_init_struct.asyn_access_mode       = EXMC_ACCESS_MODE_A;
//    sram_timing_init_struct.syn_data_latency       = EXMC_DATALAT_2_CLK;
//    sram_timing_init_struct.syn_clk_division       = EXMC_SYN_CLOCK_RATIO_DISABLE;
//    sram_timing_init_struct.bus_latency            = 2;
//    sram_timing_init_struct.asyn_data_setuptime    = 10;
//    sram_timing_init_struct.asyn_address_holdtime  = 2;
//    sram_timing_init_struct.asyn_address_setuptime = 5;

//    sram_init_struct.norsram_region    = EXMC_BANK0_NORSRAM_REGION0;
//    sram_init_struct.write_mode        = EXMC_ASYN_WRITE;
//    sram_init_struct.extended_mode     = DISABLE;
//    sram_init_struct.asyn_wait         = DISABLE;
//    sram_init_struct.nwait_signal      = DISABLE;
//    sram_init_struct.memory_write      = ENABLE;
//    sram_init_struct.nwait_config      = EXMC_NWAIT_CONFIG_BEFORE;
//    sram_init_struct.wrap_burst_mode   = DISABLE;
//    sram_init_struct.nwait_polarity    = EXMC_NWAIT_POLARITY_LOW;
//    sram_init_struct.burst_mode        = DISABLE;
//    sram_init_struct.databus_width     = EXMC_NOR_DATABUS_WIDTH;
//    sram_init_struct.memory_type       = EXMC_MEMORY_TYPE_SRAM;
//    sram_init_struct.address_data_mux  = DISABLE;
//    sram_init_struct.read_write_timing = &sram_timing_init_struct;
//    sram_init_struct.write_timing      = &sram_timing_init_struct;
//    
//    exmc_norsram_init(&sram_init_struct);
//    exmc_norsram_enable(EXMC_BANK0_NORSRAM_REGION0);
//    
//    dev->Base = 0x60000000;
//}




///* bus sram driver */
//__CONST_STATIC_ Bus_LCD_Driver_Type LCD_Driver =
//{
//    .Init = LCD_Bus_Init,
//};
//FW_DRIVER_REGIST("ll->bus_sram", &LCD_Driver, LL_Bus_LCD);



////__INLINE_ static void Bus_SRAM_Init(FW_Bus_Type *dev)
////{
////    exmc_norsram_parameter_struct sram_init_struct;
////    exmc_norsram_timing_parameter_struct sram_timing_init_struct;
////    u32 EXMC_NOR_DATABUS_WIDTH;
////    
////    if(dev->Width == FW_Bus_Width_8Bits)
////    {
////        EXMC_NOR_DATABUS_WIDTH = EXMC_NOR_DATABUS_WIDTH_8B;
////    }
////    else
////    {
////        EXMC_NOR_DATABUS_WIDTH = EXMC_NOR_DATABUS_WIDTH_16B;
////    }
////    
////    /* EXMC clock enable */
////    rcu_periph_clock_enable(RCU_EXMC);
////    
////    sram_timing_init_struct.asyn_access_mode       = EXMC_ACCESS_MODE_A;
////    sram_timing_init_struct.syn_data_latency       = EXMC_DATALAT_2_CLK;
////    sram_timing_init_struct.syn_clk_division       = EXMC_SYN_CLOCK_RATIO_DISABLE;
////    sram_timing_init_struct.bus_latency            = 2;
////    sram_timing_init_struct.asyn_data_setuptime    = 10;
////    sram_timing_init_struct.asyn_address_holdtime  = 2;
////    sram_timing_init_struct.asyn_address_setuptime = 5;

////    sram_init_struct.norsram_region    = EXMC_BANK0_NORSRAM_REGION0;
////    sram_init_struct.write_mode        = EXMC_ASYN_WRITE;
////    sram_init_struct.extended_mode     = DISABLE;
////    sram_init_struct.asyn_wait         = DISABLE;
////    sram_init_struct.nwait_signal      = DISABLE;
////    sram_init_struct.memory_write      = ENABLE;
////    sram_init_struct.nwait_config      = EXMC_NWAIT_CONFIG_BEFORE;
////    sram_init_struct.wrap_burst_mode   = DISABLE;
////    sram_init_struct.nwait_polarity    = EXMC_NWAIT_POLARITY_LOW;
////    sram_init_struct.burst_mode        = DISABLE;
////    sram_init_struct.databus_width     = EXMC_NOR_DATABUS_WIDTH;
////    sram_init_struct.memory_type       = EXMC_MEMORY_TYPE_SRAM;
////    sram_init_struct.address_data_mux  = DISABLE;
////    sram_init_struct.read_write_timing = &sram_timing_init_struct;
////    sram_init_struct.write_timing      = &sram_timing_init_struct;

////    exmc_norsram_init(&sram_init_struct);
////    exmc_norsram_enable(EXMC_BANK0_NORSRAM_REGION0);
////    
////    dev->Base = 0x60000000;
////    dev->Range = 64 * 1024 * 1024;
////}

/////*
////当EXMC分时复用为不同的功能(例如LCD + Nand)时，只要EXMC支持独立的时序控制，那么对于
////不同的功能，只需要在初始化时对EXMC进行一次初始化，而无需每次切换时都要进行初始化。
////*/
////__INLINE_STATIC_ void Bus_Init(FW_Bus_Type *dev)
////{
//////    Bus_IO_Init(dev);
////    
////    switch(dev->BD_Type)
////    {
////        case FW_Bus_Device_SRAM:
////            Bus_SRAM_Init(dev);
////            break;
////            
////        default:
////            break;
////    }
////}

