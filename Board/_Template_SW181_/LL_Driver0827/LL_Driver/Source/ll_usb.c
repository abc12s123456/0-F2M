#include "usbd_int.h"
#include "usbd_std.h"
#include "msc_core.h"
#include "bbb_scsi_driver.h"

#include "fw_usb.h"


#define EXTI_USB_WKUP        EXTI_18


usbd_core_handle_struct  usb_device_dev = 
{
    .dev_desc           = (uint8_t *)&device_descripter,
    .config_desc        = (uint8_t *)&configuration_descriptor,
    .strings            = usbd_strings,
    .class_init         = msc_init,
    .class_deinit       = msc_deinit,
    .class_req_handler  = msc_req_handler,
    .class_data_handler = msc_data_handler
};


void USBD_LP_CAN0_RX0_IRQHandler(void)
{
    usbd_isr();
}


__INLINE_STATIC_ void USB_Init(FW_USB_Type *dev)
{
    //USB时钟
    rcu_periph_clock_enable(RCU_PMU);
    rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV2_5); //PLL = 120MHz，USB = 48MHz
    rcu_periph_clock_enable(RCU_USBD);

    //USB协议栈初始化
    usbd_core_init(&usb_device_dev);
    
    //USB中断
    nvic_irq_enable(USBD_LP_CAN0_RX0_IRQn, 3, 3);
    
    usb_device_dev.status = USBD_CONNECTED;
}


/*  */
__CONST_STATIC_ FW_USB_Driver_Type Driver =
{
    .Init = USB_Init,
};
FW_DRIVER_REGIST("ll->usb", &Driver, USB_MSC);

