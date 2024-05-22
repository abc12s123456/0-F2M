#ifndef _FW_BUS_H_
#define _FW_BUS_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"
    

#define BUS_DRV_LCD_NUM      0
#define BUS_DRV_SRAM_NUM     1
#define BUS_DRV_SDRAM_NUM    2
#define BUS_DRV_NAND_NUM     3


/* 总线驱动类型 */
typedef enum
{
    FW_Bus_Device_SRAM,                //Nor Flash, PSRAM, LCD等
    FW_Bus_Device_SDRAM,               //SDRAM
    FW_Bus_Device_Nor,                 //Nor Flash
    FW_Bus_Device_Nand,                //Nand Flash
    FW_Bus_Device_Card,                //PC卡
}FW_Bus_Device_Enum;


/* 总线访问数据宽度 */
typedef enum
{
    FW_Bus_Width_32Bits = 1,
    FW_Bus_Width_16Bits,
    FW_Bus_Width_8Bits,
}FW_Bus_Width_Enum;


/* 总线地址控制IO(通过写入的数值自动区分是数据还是地址) */
typedef enum
{
    FW_Bus_IOC_A0 = 0,
    FW_Bus_IOC_A1,
    FW_Bus_IOC_A2,
    FW_Bus_IOC_A3,
    FW_Bus_IOC_A4,
    FW_Bus_IOC_A5,
    FW_Bus_IOC_A6,
    FW_Bus_IOC_A7,
    FW_Bus_IOC_A8,
    FW_Bus_IOC_A9,
    FW_Bus_IOC_A10,
    FW_Bus_IOC_A11,
    FW_Bus_IOC_A12,
    FW_Bus_IOC_A13,
    FW_Bus_IOC_A14,
    FW_Bus_IOC_A15,
    FW_Bus_IOC_A16,
    FW_Bus_IOC_A17,
    FW_Bus_IOC_A18,
    FW_Bus_IOC_A19,
    FW_Bus_IOC_A20,
    FW_Bus_IOC_A21,
    FW_Bus_IOC_A22,
    FW_Bus_IOC_A23,
    FW_Bus_IOC_A24,
    FW_Bus_IOC_A25,
    FW_Bus_IOC_A26,
    FW_Bus_IOC_A27,
    FW_Bus_IOC_A28,
    FW_Bus_IOC_A29,
    FW_Bus_IOC_A30,
    FW_Bus_IOC_A31,
}FW_Bus_IOC_Enum;


/* 总线访问对象 */
typedef enum
{
    FW_Bus_Access_Reg  = 0,
    FW_Bus_Access_Data = 1,
    FW_Bus_Access_RAM  = FW_Bus_Access_Data,
    FW_Bus_Access_Cmd  = 2,
    FW_Bus_Access_Addr = 3,
}FW_Bus_Access_Enum;


typedef struct FW_Bus FW_Bus_Type;
typedef void (*Bus_PF)(FW_Bus_Type *dev, void *pdata);


/* 
外部扩展总线，在硬件层面上只有一组通讯线路，所有挂载的模块分时复用。但从软件层
面上来看，内部不同的模块在使用总线时，是可以单独执行各自的时序(模块功能不冲突)。
所以这里对总线进行设备定义时，以单独的功能模块作为一个设备对象。
*/
struct FW_Bus
{
    FW_Device_Type Device;
    
    u32 Base;                //总线基地址
    u32 Range;               //总线访问范围
    u32 Size;                //挂载设备的存储大小
    
    u32 BD_Type : 3;         //挂载设备的类型，参照FW_Bus_Device_Enum
    u32 Width : 3;           //总线访问数据宽度
    
    u32 IOC_RS : 6;
    u32 IOC_ALE : 6;
    u32 IOC_CLE : 6;
    
    u32 Offset : 3;
    
    u32 : 5;
    
    Bus_PF WriteData;
    Bus_PF WriteReg;
    Bus_PF WriteAddr;
    Bus_PF WriteCmd;
    
    Bus_PF ReadData;
    Bus_PF ReadReg;
    Bus_PF ReadAddr;
    Bus_PF ReadCmd;
};


typedef struct FW_Bus_LCD  FW_Bus_LCD_Type;

struct FW_Bus_LCD
{
    FW_Device_Type Device;
    
    u32 Base;                //基地址
    
    u32 Bus_Device : 3;      //挂载设备的类型，参照FW_Bus_Device_Enum
    u32 Bus_Width : 3;       //总线数据位宽，参照FW_Bus_Width_Enum

    u32 IOC_RS : 6;
    
    u32 : 20;
        
    void (*Write_Cmd)(FW_Bus_LCD_Type *dev, const void *cmd);
    void (*Write_Data)(FW_Bus_LCD_Type *dev, const void *pdata);
    void (*Read_Data)(FW_Bus_LCD_Type *dev, void *pdata);
};


typedef struct
{
    void (*Init)(FW_Bus_LCD_Type *dev);
}Bus_LCD_Driver_Type;





///*  */
//typedef struct
//{
//    void (*Init)(FW_Bus_Type *dev);
//}FW_Bus_Driver_Type;


//void FW_Bus_WriteReg(FW_Bus_Type *dev, void *pdata);
//void FW_Bus_ReadReg(FW_Bus_Type *dev, void *pdata);

//void FW_Bus_WriteData(FW_Bus_Type *dev, void *pdata, u32 num);
//void FW_Bus_ReadData(FW_Bus_Type *dev, void *pdata, u32 num);

//void FW_Bus_WriteAddr(FW_Bus_Type *dev, void *pdata, u32 num);
//void FW_Bus_ReadAddr(FW_Bus_Type *dev, void *pdata, u32 num);

//void FW_Bus_WriteCmd(FW_Bus_Type *dev, void *pdata, u32 num);
//void FW_Bus_ReadCmd(FW_Bus_Type *dev, void *pdata, u32 num);

#ifdef __cplusplus
}
#endif

#endif

