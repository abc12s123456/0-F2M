#include "display.h"

#include "board.h"

#include "fw_uart.h"


static FW_UART_Type *Duart;


void Disp_Init(void)
{
    Duart = FW_Device_Find(DISP_DD_NAME);
    if(Duart == NULL)  while(1);
    FW_UART_Init(Duart);
}

void Disp_Page(u8 num)
{
    u8 msg[] = {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x00};
    msg[sizeof(msg) - 1] = num;
    FW_UART_Write(Duart, 0, msg, sizeof(msg));
}












