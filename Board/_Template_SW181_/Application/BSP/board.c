#include "board.h"
#include "fw_gpio.h"


#if 0

/* 旋转编码器 */
#include "qencoder.h"

#define QENCODER_DRV_ID      FW_DRIVER_ID(FW_Device_Encoder, QENCODER_DRV_IO_NUM)

static QEncoder_Type QEncoder1, QEncoder2;

static void QEncoder1_Config(void *dev)
{
    QEncoder_Type *e = dev;
    QEncoder_Config_Type *cfg = &e->Config;
    QEncoder_Driver_Type *drv = FW_Driver_Find("io->encoder");
    
    cfg->Pin_A = PC6;
    cfg->Pin_B = PC7;
    
    cfg->VL_A = LEVEL_L;
    cfg->VL_B = LEVEL_L;
    
    FW_Device_SetDriver(e, drv);
}
FW_DEVICE_STATIC_REGIST("qedr1", &QEncoder1, QEncoder1_Config, QEncoder1);

static void QEncoder2_Config(void *dev)
{
    QEncoder_Type *e = dev;
    QEncoder_Config_Type *cfg = &e->Config;
    QEncoder_Driver_Type *drv = FW_Driver_Find("io->encoder");
    
    cfg->Pin_A = PE9;
    cfg->Pin_B = PE11;
    
    cfg->VL_A = LEVEL_L;
    cfg->VL_B = LEVEL_L;
    
    FW_Device_SetDriver(e, drv);
}
FW_DEVICE_STATIC_REGIST("qedr2", &QEncoder2, QEncoder2_Config, QEncoder2);


#include "fw_uart.h"

static void Board_Init(void *pdata)
{
    FW_UART_Type *uart;
    
    uart = FW_Device_Find(DISP_DD_NAME);
    if(uart == NULL)  while(1);
    
    uart->Baudrate = 115200;
    uart->TX_Pin = PD5;
    uart->RX_Pin = PD6;
    uart->Config.RX_Mode = TRM_INT;
    uart->RX_RB_Size = 64;
}
FW_BOARD_INIT(Board_Init, NULL);


#include "fw_device.h"
#include "fw_bus.h"
#include "lcd.h"

static FW_Bus_LCD_Type LCD_Bus;
static void LCD_Bus_Config(void *dev)
{
    FW_Bus_LCD_Type *lcd_bus = dev;
    FW_Bus_Type *parent = FW_Device_Find("exbus");
    
    FW_Device_SetParent(dev, parent);
    FW_Device_SetDriver(dev, FW_Driver_Find("ll->bus_sram"));
    
    lcd_bus->Base = 0x60000000;
}
FW_DEVICE_STATIC_REGIST(LCD_BUS_NAME, &LCD_Bus, LCD_Bus_Config, LCD_Bus);

static LCD8_Type ILI9341;
static void ILI9341_Config(void *dev)
{
    LCD8_Type *lcd = dev;
    FW_Bus_LCD_Type *parent = FW_Device_Find(LCD_BUS_NAME);
    
    FW_Device_SetParent(dev, parent);
    FW_Device_SetDriver(dev, FW_Driver_Find("bus->lcd8"));
    
    lcd->Hori_Pixel = 240;
    lcd->Vert_Pixel = 320;
    
    lcd->Data_Width = LCD_Data_Width_16Bits;
    lcd->IOC_RS = FW_Bus_IOC_A16;
    
    lcd->RST_Pin = PC12;
    lcd->BL_Pin = PA1;
}
FW_DEVICE_STATIC_REGIST(LCD_HW_NAME, &ILI9341, ILI9341_Config, ILI9341);

static LCD_Type LCD;
static void LCD_Config(void *dev)
{
    LCD8_Type *parent = FW_Device_Find(LCD_HW_NAME);
    
    FW_Device_SetParent(dev, parent);
    FW_Device_SetDriver(dev, FW_Driver_Find("ili9341->al_lcd"));
}
FW_DEVICE_STATIC_REGIST(LCD_DEV_NAME, &LCD, LCD_Config, LCD);

#endif
