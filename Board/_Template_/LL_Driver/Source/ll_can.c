#include "ll_include.h"

#include "fw_can.h"
#include "fw_gpio.h"
#include "fw_print.h"

#include "ll_system.h"


#define CANx(name)(\
(Isdev(name, "can0")) ? CAN0 :\
(Isdev(name, "can1")) ? CAN1 :\
(u32)INVALUE)


extern FW_CAN_Type CAN0_Device;
extern FW_CAN_Type CAN1_Device;


__INLINE_STATIC_ void CAN_IO_Init(FW_CAN_Type *dev)
{
    u32 can = (u32)dev->CANx;
    
    u16 tx_pin = dev->TX_Pin;
    u16 rx_pin = dev->RX_Pin;
    
    FW_GPIO_Init(rx_pin, FW_GPIO_Mode_IPU, FW_GPIO_Speed_High);
    FW_GPIO_Init(tx_pin, FW_GPIO_Mode_AF_Out_PPD, FW_GPIO_Speed_High);
    
    if(can == CAN0)
    {
        if(rx_pin == PA11 && tx_pin == PA12){}
        else if(rx_pin == PD0 && tx_pin == PD1)
        {
            gpio_pin_remap_config(GPIO_CAN_FULL_REMAP, ENABLE);
        }
        else if(rx_pin == PB8 && tx_pin == PB9)
        {
            gpio_pin_remap_config(GPIO_CAN_PARTIAL_REMAP, ENABLE);
        }
        else{}
    }
    else if(can == CAN1)
    {
        /* 叙癖喘噐札選侏 */
        if(rx_pin == PB12 && tx_pin == PB13){}
        else if(rx_pin == PB5 && tx_pin == PB6)
        {
            gpio_pin_remap_config(GPIO_CAN_FULL_REMAP, ENABLE);
        }
        else{}
    }
    else
    {
        while(1);
    }
}


#ifdef GP32F30X_CL
#define RCU_CANx(can)(\
(can == CAN0) ? RCU_CAN0 :\
(can == CAN1) ? RCU_CAN1 :\
(rcu_periph_enum)INVALUE)
#else
#define RCU_CANx(can)(\
(can == CAN0) ? RCU_CAN0 :\
(rcu_periph_enum)INVALUE)
#endif

#ifdef GP32F30X_CL
#define CANx_IRQn(can, tr)(\
(can == CAN0 && tr == TOR_TX) ? CAN0_TX_IRQn :\
(can == CAN0 && tr == TOR_RX) ? CAN0_RX1_IRQn :\
(can == CAN1 && tr == TOR_TX) ? CAN1_TX_IRQn :\
(can == CAN1 && tr == TOR_RX) ? CAN1_RX1_IRQn :\
(IRQn_Type)INVALUE)
#else
#define CANx_IRQn(can, tr)(\
(can == CAN0 && tr == TOR_TX) ? USBD_HP_CAN0_TX_IRQn :\
(can == CAN0 && tr == TOR_RX) ? CAN0_RX1_IRQn :\
(IRQn_Type)INVALUE)
#endif


__INLINE_STATIC_ void CAN_DeInit(FW_CAN_Type *dev)
{
    char *name = FW_Device_GetName(dev);
    u32 can = CANx(name);
    
    FW_CAN_SetPort(dev, (void *)can);
    
    can_deinit(can);
}

__INLINE_STATIC_ void CAN_Init(FW_CAN_Type *dev)
{
    can_parameter_struct can_parameter;
    can_filter_parameter_struct can_filter;
    
    char *name = FW_Device_GetName(dev);
    u32 can = CANx(name);
    
    u8 mode, filter;
    
    u32 tmp;
    u16 prescaler;
    u8 sync, bs1, bs2;
    
    u32 can_id = dev->ID;
    
    u8 trm;
    
    
    FW_CAN_SetPort(dev, (void *)can);
    
    can_deinit(can);
    
    /* clock */
    rcu_periph_clock_enable(RCU_CANx(can));
    
    /* IO */
    CAN_IO_Init(dev);
    
    /* working mode */
    if(dev->Mode == FW_CAN_Mode_Normal)
        mode = CAN_NORMAL_MODE;
    else if(dev->Mode == FW_CAN_Mode_LoopBack)
        mode = CAN_LOOPBACK_MODE;
    else if(dev->Mode == FW_CAN_Mode_Silent)
        mode = CAN_SILENT_MODE;
    else
        mode = CAN_SILENT_LOOPBACK_MODE;
    
    /*
                     LL_Clocks.APB1Clk 
    baudrate = ！！！！！！！！！！！！！！！！！！！！！！！！！！！！
               (bs1 + bs2 + 3) * prescaler
    凪嶄
    bs1 [1~16]
    bs2 [1~8]
    prescaler [1~512]
    */
    tmp = LL_Clocks.APB1Clk / dev->Baudrate;
    prescaler = 1;
    sync = 1;
    while(1)
    {
        if((tmp / prescaler) > 27)  
        {
            prescaler++;
        }
        else
        {
            tmp = tmp / prescaler - 3;
            bs1 = 1;
            while(1)
            {
                if((tmp - bs1) > 8)
                {
                    bs1++;
                }
                else
                {
                    bs2 = tmp - bs1;
                    break;
                }
            }
            break;
        }
    }
    
    /* initialize */
    can_parameter.time_triggered        = DISABLE;
    can_parameter.auto_bus_off_recovery = DISABLE;
    can_parameter.auto_wake_up          = DISABLE;
    can_parameter.auto_retrans          = DISABLE;
    can_parameter.rec_fifo_overwrite    = DISABLE;
    can_parameter.trans_fifo_order      = DISABLE;
    can_parameter.working_mode          = mode;
    can_parameter.resync_jump_width     = sync - 1;
    can_parameter.time_segment_1        = bs1 - 1;
    can_parameter.time_segment_2        = bs2 - 2;
    can_parameter.prescaler             = prescaler - 1;
    can_init(can, &can_parameter);
    
    if(dev->Fiter == FW_CAN_Filter_List)
    {
        filter = CAN_FILTERMODE_LIST;
    }
    else
    {
        filter = CAN_FILTERMODE_MASK;
    }
    
    /* filter */  
    can_filter.filter_number      = 0;  
    can_filter.filter_mode        = filter;
    can_filter.filter_bits        = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high   = can_id >> 16;
    can_filter.filter_list_low    = can_id & 0xFF;
    can_filter.filter_mask_high   = can_id >> 16;
    can_filter.filter_mask_low    = can_id & 0xFF;  
    can_filter.filter_fifo_number = CAN_FIFO0;
    can_filter.filter_enable      = ENABLE;
    can_filter_init(&can_filter);
    
    trm = FW_CAN_GetTRM(dev, TOR_TX);
    if(trm == TRM_INT)
    {
        nvic_irq_enable(CANx_IRQn(can, TOR_TX), 3, 3);
        can_interrupt_disable(can, CAN_INT_TME);
    }
    else
    {
        
    }
    
    trm = FW_CAN_GetTRM(dev, TOR_RX);
    if(trm == TRM_INT)
    {
        nvic_irq_enable(CANx_IRQn(can, TOR_RX), 3, 3);
        can_interrupt_enable(can, CAN_INT_RFNE0);
    }
    else
    {
        
    }
}

__INLINE_STATIC_ void CAN_FT_CTL(FW_CAN_Type *dev, u8 state)
{
    
}

__INLINE_STATIC_ void CAN_TX_CTL(FW_CAN_Type *dev, u8 state)
{
    u32 can = (u32)dev->CANx;
    if(state)
        can_interrupt_enable(can, CAN_INT_TME);
    else
        can_interrupt_disable(can, CAN_INT_TME);
}

__INLINE_STATIC_ void CAN_RX_CTL(FW_CAN_Type *dev, u8 state)
{
    u32 can = (u32)dev->CANx;
    if(state)
        can_interrupt_enable(can, CAN_INT_RFNE1);
    else
        can_interrupt_disable(can, CAN_INT_RFNE1);
}

__INLINE_STATIC_ u8   CAN_Frame_Write(FW_CAN_Type *dev, FW_CAN_Frame_Type *frame)
{
    can_trasnmit_message_struct msg;
    u32 can = (u32)dev->CANx;
    
    if(frame->Type == FW_CAN_FT_Data)
    {
        msg.tx_ft = CAN_FT_DATA;
    }
    else
    {
        msg.tx_ft = CAN_FT_REMOTE;
    }
    
    if(frame->Format == FW_CAN_FF_STD)
    {
        msg.tx_ff = CAN_FF_STANDARD;
        msg.tx_sfid = frame->ID;
    }
    else
    {
        msg.tx_ff = CAN_FF_EXTENDED;
        msg.tx_efid = frame->ID;
    }
    
    msg.tx_dlen = frame->Length;
    memcpy(msg.tx_data, frame->Data, frame->Length);
    
    dev->Mailbox_Num = can_message_transmit(can, &msg);
    
    return frame->Length;
}

__INLINE_STATIC_ u8   CAN_Frame_Read(FW_CAN_Type *dev, FW_CAN_Frame_Type *frame)
{
    can_receive_message_struct msg;
    u32 can = (u32)dev->CANx;
    
    can_message_receive(can, CAN_FIFO1, &msg);
    
    if(msg.rx_ft == CAN_FT_DATA)
    {
        frame->Type = FW_CAN_FT_Data;
    }
    else
    {
        frame->Type = FW_CAN_FT_Remote;
    }
    
    if(msg.rx_ff == CAN_FF_STANDARD)
    {
        frame->ID = msg.rx_sfid;
        frame->Format = FW_CAN_FF_STD;
    }
    else
    {
        frame->ID = msg.rx_efid;
        frame->Format = FW_CAN_FF_EXT;
    }
    
    frame->Length = msg.rx_dlen;
    memcpy(frame->Data, msg.rx_data, frame->Length);
    
    return frame->Length;
}

__INLINE_STATIC_ u8   CAN_Wait_TC(FW_CAN_Type *dev)
{
    can_transmit_state_enum state = can_transmit_states((u32)dev->CANx, dev->Mailbox_Num);
    if(state == CAN_TRANSMIT_OK)  return FW_CAN_TX_State_OK;
    if(state == CAN_TRANSMIT_FAILED)  return FW_CAN_TX_State_Failed;
    if(state == CAN_TRANSMIT_PENDING)  return FW_CAN_TX_State_Pending;
    if(state == CAN_TRANSMIT_NOMAILBOX)  return FW_CAN_TX_State_NoMailbox;
    return FW_CAN_TX_State_Unknown;
}

__INLINE_STATIC_ u8   CAN_Wait_RC(FW_CAN_Type *dev)
{
    
}




__CONST_STATIC_ FW_CAN_Driver_Type CAN_Driver =
{
    .DeInit = CAN_DeInit,
    .Init   = CAN_Init,
    
    .CTL    = CAN_FT_CTL,
    .TX_CTL = CAN_TX_CTL,
    .RX_CTL = CAN_RX_CTL,
    
    .Frame_Write = CAN_Frame_Write,
    .Frame_Read  = CAN_Frame_Read,
};
FW_DRIVER_REGIST("ll->can", &CAN_Driver, CAN);




static void CAN_Config(void *dev)
{
    FW_Device_SetDriver(dev, &CAN_Driver);
}

static FW_CAN_Type CAN0_Device;
FW_DEVICE_STATIC_REGIST("can0", &CAN0_Device, CAN_Config, CAN0);

static FW_CAN_Type CAN1_Device;
FW_DEVICE_STATIC_REGIST("can1", &CAN1_Device, CAN_Config, CAN1);

