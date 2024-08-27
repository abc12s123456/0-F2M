#include "ll_include.h"

#include "fw_bkp.h"
#include "mblock.h"


__INLINE_STATIC_ void BKP_Init(void)
{
    rcu_periph_clock_enable(RCU_BKPI);
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();
    rcu_bkp_reset_disable();                      //禁止BKP复位，否则系统复位后BKP的值会清除
}

__INLINE_STATIC_ u32 BKP_Get_Base(void)
{
    return 0;
}

__INLINE_STATIC_ u32 BKP_Get_Size(void)
{
    return 42 * 2;
}

__INLINE_STATIC_ u32  BKP_Write_Gran(u32 addr, const void *pdata)
{
    addr = (addr >> 1) + 1;
    bkp_write_data((bkp_data_register_enum)addr, *(u16 *)pdata);
    return sizeof(u16);
}

__INLINE_STATIC_ u32  BKP_Read_Gran(u32 addr, void *pdata)
{
    addr = (addr >> 1) + 1;
    *(u16 *)pdata = bkp_read_data((bkp_data_register_enum)addr);
    return sizeof(u16);
}

__INLINE_STATIC_ u32  BKP_Write(u32 addr, const void *pdata, u32 num)
{
    MBlock_Type mb;
    u16 value;
    
    mb.Addr = addr;
    mb.Pdata = (void *)pdata;
    mb.Num = num;
    
    mb.Buffer = (u8 *)&value;
    mb.Gran_Size = sizeof(u16);
    
    mb.Write_Gran = BKP_Write_Gran;
    mb.Read_Gran = BKP_Read_Gran;
    
    return MBlock_Write_SelfAlign(&mb);
}

__INLINE_STATIC_ u32  BKP_Read(u32 addr, void *pdata, u32 num)
{
    MBlock_Type mb;
    u16 value;
        
    mb.Addr = addr;
    mb.Pdata = (void *)pdata;
    mb.Num = num;
    
    mb.Buffer = (u8 *)&value;
    mb.Gran_Size = sizeof(u16);
    
    mb.Write_Gran = BKP_Write_Gran;
    mb.Read_Gran = BKP_Read_Gran;
    
    return MBlock_Read_SelfAlign(&mb);
}




/* bkp driver */
__CONST_STATIC_ FW_BKP_Driver_Type Driver =
{
    .Init = BKP_Init,
    .Get_Base = BKP_Get_Base,
    .Get_Size = BKP_Get_Size,
    .Write = BKP_Write,
    .Read = BKP_Read,
};
FW_DRIVER_REGIST("ll->bkp", &Driver, BKP);




#include "project_debug.h"
#if MODULE_TEST && BKP_TEST
#include "fw_system.h"


#define SIZE  5


void Test(void)
{
    const u32 msg[10] = {0x15555555, 0x26666666, 0x37777777, 0x48888888, 0x59999999};
    u8 tmp[sizeof(msg)];
    u8 addr = 1;
    
    FW_System_Init();
    
    FW_BKP_Init();
    
    memset(tmp, 0, sizeof(msg));
    FW_BKP_Read(addr, tmp, sizeof(msg));
    
    FW_BKP_Write(addr, msg, sizeof(msg));
    
    memset(tmp, 0, sizeof(msg));
    FW_BKP_Read(addr, tmp, sizeof(msg));
    
    FW_BKP_Read(addr + 1, tmp + 1, sizeof(msg));
    
    while(1);
}


#endif

