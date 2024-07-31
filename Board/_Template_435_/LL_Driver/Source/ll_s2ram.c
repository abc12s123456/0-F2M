#include "fw_debug.h"
#include "fw_s2ram.h"


#if defined (IRAM2_BASE) && defined (IRAM2_SIZE)
#if IRAM2_SIZE
/*
1、IRAM2作为备份寄存器使用时，ll_bkp与ll_s2ram二选其一
2、IRAM2作为S2RAM使用时，则PAL中使用fw_s2ram
3、IRAM2复位后数据不会被清除，但掉电后会被清除
4、在使用IRAM2时，应在第一次初始化时在固定位置写入一个标识(例如0地址0xA5A5A5A5),
   并对IRAM2区域进行初始化
*/
volatile static u8 LL_S2RAM[IRAM2_SIZE] __AT_(IRAM2_BASE);


__INLINE_STATIC_ void S2RAM_Init(FW_S2RAM_Type *dev)
{
    dev->Base = IRAM2_BASE;
    dev->Size = IRAM2_SIZE;
}

__INLINE_STATIC_ void S2RAM_Clear(FW_S2RAM_Type *dev)
{
    memset((void *)LL_S2RAM, 0, IRAM2_SIZE);
}

__INLINE_STATIC_ u32  S2RAM_Write(FW_S2RAM_Type *dev, u32 addr, const void *pdata, u32 num)
{
    u8 *p = (u8 *)addr;
    memcpy(p, pdata, num);
    return num;
}

__INLINE_STATIC_ u32  S2RAM_Read(FW_S2RAM_Type *dev, u32 addr, void *pdata, u32 num)
{
    u8 *p = (u8 *)addr;
    memcpy(pdata, p, num);
    return num;
}




/* S2RAM0驱动 */
__CONST_STATIC_ FW_S2RAM_Driver_Type S2RAM_Driver =
{
    .Init  = S2RAM_Init,
    .Clear = S2RAM_Clear,
    .Write = S2RAM_Write,
    .Read  = S2RAM_Read,
};
FW_DRIVER_REGIST("ll->s2ram", &S2RAM_Driver, S2RAM);

#endif  /* #if IRAM2_SIZE */
#endif  /* #if defined (IRAM2_BASE) && defined (IRAM2_SIZE) */




#include "project_debug.h"
#if MODULE_TEST && S2RAM_TEST
#include "fw_system.h"
#include "fw_delay.h"
#include "fw_uart.h"
#include "fw_gpio.h"


#define S2RAM_DEV_NAME       "s2ram"
static FW_S2RAM_Type S2RAM;
static void S2RAM_Config(void *pdata)
{
    FW_S2RAM_Driver_Type *drv = FW_Driver_Find(S2RAM_DRV_ID);
    FW_S2RAM_Type *dev = pdata;
    
    FW_Device_SetDriver(dev, drv);
    FW_Device_SetDID(dev, S2RAM_DRV_ID);
}
FW_DEVICE_STATIC_REGIST(S2RAM_DEV_NAME, &S2RAM, S2RAM_Config, S2RAM_Device);


void Test(void)
{
    FW_UART_Type *log;
    FW_S2RAM_Type *ram;
    u32 cnt = 0;
    u32 addr = 0;
    
    log = FW_Device_Find("uart0");
    if(log == NULL)  while(1);
    
    ram = FW_Device_Find("s2ram");
    if(ram == NULL)  while(1);
    
    log->Baudrate = 9600;
    log->TX_Pin = PA9;
    FW_UART_Init(log);
    FW_UART_PrintInit(log);
    
    FW_S2RAM_Init(ram);
    
    FW_S2RAM_GetBase(ram);
    
    FW_S2RAM_Read(ram, addr, &cnt, sizeof(cnt));
    if((cnt >> 16) != 0xA5A5)
    {
        FW_S2RAM_Clear(ram);
        cnt = 0xA5A50000;
    }
    printf("\r\ncnt = %d\r\n", cnt & 0xFFFF);
    
    cnt = (++cnt > 0xA5A5FFFF) ? 0 : cnt;
    FW_S2RAM_Write(ram, addr, &cnt, sizeof(cnt));
    
    FW_System_SoftReset();
    
    while(1);
}


#endif

