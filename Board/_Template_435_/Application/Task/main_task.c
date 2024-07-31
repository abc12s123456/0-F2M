#include "display.h"

#include "board.h"

#include "qencoder.h"

#include "fw_gpio.h"
#include "fw_delay.h"

#include "fw_uart.h"


#if 1

void Task_Entry(void)
{
    FW_UART_PrintInit(PA9, 9600);
    
    printf("hello world\r\n");
    
    while(1);
}

#else


#define PAGE_OFFSET  10


void Task_Entry(void)
{
    QEncoder_Type *e1, *e2;
    s32 ev1, ev2;
    s8 page_num = 0;
    s8 s = 0, g = 0;
    Bool change_flag = False;
    u8 pic_num;
    
    u16 VCC_EN = PD1;
    
    Disp_Init();
    
    e1 = FW_Device_Find(QENCODER1_NAME);
    QEncoder_Init(e1);
    while(!e1);
    
    e2 = FW_Device_Find(QENCODER2_NAME);
    while(!e2);
    QEncoder_Init(e2);
    
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
    FW_GPIO_SET(VCC_EN);
    
    Disp_Page(page_num);
    
    while(1)
    {
        ev1 = QEncoder_Get(e1);
        ev2 = QEncoder_Get(e2);
        
        if(ev1 || ev2)
        {
            change_flag = True;
            
            if(ev1)
            {
                s += ev1;
                if(s > 2)  s = 0;
                else  if(s < 0)  s = 2;
                else  s = s;
//                page_num = s * PAGE_OFFSET;
                page_num = 0;
                g = 0;
            }
            
//            if(ev2)
//            {
//                g += ev2;
//                if(g > 9)  g = 0;
//                else if(g < 0)  g = 9;
//                else g = g;
//                page_num = s * PAGE_OFFSET + g;
//                
//            }
            
            if(ev2)
            {
                pic_num = 10;
            
                g += ev2;
                if(g > pic_num)  g = 0;
                else if(g < 0)  g = pic_num;
                else g = g;
                page_num = g;
            }
        }
        
        if(change_flag == True)
        {
            Disp_Page(page_num);
            change_flag = False;
        }
        
        FW_Delay_Ms(20);
    }
}

#endif

