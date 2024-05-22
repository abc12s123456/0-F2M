#include "esp8266.h"

#include "wifi.h"

#include "fw_uart.h"
#include "fw_gpio.h"
#include "fw_delay.h"


#define SEND_TIMEOUT         1000      //发送超时时间，单位：ms
#define SEND_WAIT_TIME       10        //发送等待时间，单位：ms
#define SEND_CNT_MAX         50        //发送次数
#define RESEND_CNT_MAX       2         //重发次数


extern void FW_Lock(void);
extern void FW_Unlock(void);


#define ESP_LOCK             FW_Lock
#define ESP_UNLOCK           FW_Unlock


typedef enum
{
    ESP8266_State_OK = 0,
    
    ESP8266_State_Uninit       = 0x30,
    ESP8266_State_Init         = 0x31,
    ESP8266_State_LinkAP       = 0x32,
    ESP8266_State_LinkServer   = 0x33,
    ESP8266_State_UnlinkAP     = 0x34,
    ESP8266_State_UnlinkServer = 0x35,
    
    ESP8266_State_Err,
}ESP8266_State_Enum;


typedef enum
{
    ESP8266_JAP_OK      = 0,
    ESP8266_JAP_Timeout = 0x31,
    ESP8266_JAP_SkeyErr = 0x32,
    ESP8266_JAP_NOAP    = 0x33,
    ESP8266_JAP_FAIL    = 0x34,
    ESP8266_JAP_NOACK   = 0x35,
}ESP8266_JAP_Enum;


__INLINE_STATIC_ void ESP8266_Clear_RBuff(ESP8266_Type *esp)
{
    FW_UART_Type *uart = FW_Device_GetParent(esp);
    FW_UART_ClearFIFO(uart, TOR_RX);
}

//__INLINE_STATIC_ void ESP8266_Fill_RBuff(ESP8266_Type *esp, u8 value)
//{
//    FW_UART_Type *uart = FW_Device_GetParent(esp);
//    FW_UART_FillFIFO(uart, value, TOR_RX);
//}

__INLINE_STATIC_ void ESP8266_Write_Cmd(ESP8266_Type *esp, const char *cmd)
{
    FW_UART_Type *uart = FW_Device_GetParent(esp);
    FW_UART_Write(uart, 0, (u8 *)cmd, strlen(cmd));
}

__INLINE_STATIC_ void ESP8266_Write_Msg(ESP8266_Type *esp, const u8 *pdata, u32 num)
{
    FW_UART_Type *uart = FW_Device_GetParent(esp);
    FW_UART_Write(uart, 0, pdata, num);
}

__INLINE_STATIC_ u32  ESP8266_Read_Msg(ESP8266_Type *esp, u8 *pdata, u32 num)
{
    FW_UART_Type *uart = FW_Device_GetParent(esp);
    return FW_UART_Read(uart, 0, pdata, num);
}

__INLINE_STATIC_ u32  ESP8266_Read_First(ESP8266_Type *esp, const char *s, u8 *pdata, u32 num)
{
    FW_UART_Type *uart = FW_Device_GetParent(esp);
    return FW_UART_ReadFirst(uart, s, pdata, num);
}

__INLINE_STATIC_ u32  ESP8266_Read_Inter(ESP8266_Type *esp, const char *s1, const char *s2, u8 *pdata, u32 cntout)
{
    FW_UART_Type *uart = FW_Device_GetParent(esp);
    u8 len = FW_UART_ReadInter(uart, s1, s2, pdata, cntout);
    u8 s1_len = strlen(s1);
    u8 s2_len = strlen(s2);
    
    if(len)
    {
        len = len - s1_len - s2_len;
        memmove(pdata, pdata + s1_len, len);
        return len;
    }
    
    return 0;
}

__INLINE_STATIC_ Bool ESP8266_String_IsExist(ESP8266_Type *esp, const char *s)
{
    FW_UART_Type *uart = FW_Device_GetParent(esp);
    return FW_UART_StringIsExist(uart, s);
}


static void ESP8266_Init(WiFi_Type *dev)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    FW_UART_Type *uart = FW_Device_GetParent(esp);
    
    FW_UART_Init(uart);
    
    if(esp->RST_VL == LEVEL_H)
    {
        FW_GPIO_Init(esp->RST_Pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
        FW_GPIO_CLR(esp->RST_Pin);
    }
    else
    {
        FW_GPIO_Init(esp->RST_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
        FW_GPIO_SET(esp->RST_Pin);
    }
    
    if(esp->EN_Pin == LEVEL_H)
    {
        FW_GPIO_Init(esp->EN_Pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
        FW_GPIO_CLR(esp->EN_Pin);
    }
    else
    {
        FW_GPIO_Init(esp->EN_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
        FW_GPIO_SET(esp->EN_Pin);
    }
}

__INLINE_STATIC_ Bool AT_Send(ESP8266_Type *esp, const char *cmd, const char *rcmp)
{
    u8 resend = 0;
    u8 cnt = 0;
    
    Send:
    ESP8266_Clear_RBuff(esp);
    
    ESP8266_Write_Cmd(esp, cmd);
    
    while(ESP8266_String_IsExist(esp, rcmp) == False)
    {
        FW_Delay_Ms(SEND_WAIT_TIME);
        if(cnt++ > SEND_CNT_MAX)
        {
            cnt = 0;
            if(resend++ > RESEND_CNT_MAX)
            {
                return False;
            }
            goto Send;
        }
    }
    
    return True;
}

static Bool ESP8266_Reset(WiFi_Type *dev, u8 hs)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    if(hs)
    {
        if(esp->RST_Pin)
        {
            FW_GPIO_Write(esp->RST_Pin, !esp->RST_VL);
            FW_Delay_Ms(10);
            FW_GPIO_Write(esp->RST_Pin, esp->RST_VL);
            FW_Delay_Ms(100);
            FW_GPIO_Write(esp->RST_Pin, !esp->RST_VL);
            return True;
        }
        return False;
    }
    else
    {
        return AT_Send(esp, "AT+RST\r\n", "OK");
    }
}

static Bool ESP8266_Restore(WiFi_Type *dev)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    return AT_Send(esp, "AT+RESTORE\r\n", "OK");
}

static Bool ESP8266_Enter_LPM(WiFi_Type *dev, WiFi_LPM_Enum lpm)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    if(esp->EN_Pin)
    {
        FW_GPIO_Write(esp->EN_Pin, !esp->EN_VL);
        return True;
    }
    return False;
}

static Bool ESP8266_Exit_LPM(WiFi_Type *dev)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    if(esp->EN_Pin)
    {
        FW_GPIO_Write(esp->EN_Pin, esp->EN_VL);
        return True;
    }
    return False;
}

__INLINE_STATIC_ char *ESP8266_Get_RBuff(ESP8266_Type *esp)
{
    FW_UART_Type *uart = FW_Device_GetParent(esp);
    return (char *)FW_UART_GetFIFOBBase(uart, TOR_RX);
}

__INLINE_STATIC_ u32  ESP8266_Get_RDL(ESP8266_Type *esp)
{
    FW_UART_Type *uart = FW_Device_GetParent(esp);
    return FW_UART_GetDataLength(uart, TOR_RX);
}

static Bool ESP8266_Set_Mode(WiFi_Type *dev, WiFi_Mode_Enum mode)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    char emode = mode + 0x30;
    char msg[20] = {0};
    
    strcat(msg, "AT+CWMODE=");
    strcat(msg, &emode);
    strcat(msg, "\r\n");
    
    return AT_Send(esp, msg, "OK");
}

static u8   ESP8266_Get_Mode(WiFi_Type *dev)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    u8 msg[10];
    
    if(AT_Send(esp, "AT+CWMODE?\r\n", "OK") != True)
    {
        dev->Mode = WiFi_Mode_Unknown;
        return WiFi_Mode_Unknown;
    }
    
    if(ESP8266_Read_First(esp, "+CWMODE:", msg, 1) == 0)
    {
        dev->Mode = WiFi_Mode_Unknown;
        return WiFi_Mode_Unknown;
    }
    
    dev->Mode = msg[8] - 0x30;
    
    return dev->Mode;
}

static Bool ESP8266_Set_LocalInfo(WiFi_Type *dev, WiFi_Local_Info_Type *info)
{
    return True;
}

static Bool ESP8266_Get_LocalInof(WiFi_Type *dev, WiFi_Local_Info_Type *info)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    char *pstr;
    u8 len;
    
    if(AT_Send(esp, "AT+CIFSR\r\n", "OK") != True)
    {
        return False;
    }
    
    pstr = (char *)"+CIFSR:STAIP,\"";
    if(ESP8266_String_IsExist(esp, pstr))
    {
        len = ESP8266_Read_Inter(esp, pstr, "\"", (u8 *)info->IP, WIFI_IP_LEN_MAX);
        if(len == 0)  return False;
    }
    
    pstr = (char *)"+CIFSR:STAMAC,\"";
    if(ESP8266_String_IsExist(esp, pstr))
    {
        len = ESP8266_Read_Inter(esp, pstr, "\"", (u8 *)info->MAC, WIFI_MAC_LEN_MAX);
        if(len == 0)  return False;
    }
    
    return True;
}

static Bool ESP8266_AP_Probe(WiFi_Type *dev, const char *ssid)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    u8 cnt = 0;
    
    ESP8266_Clear_RBuff(esp);
    
    ESP_LOCK();
    ESP8266_Write_Cmd(esp, "AT+CWLAP=");
    ESP8266_Write_Cmd(esp, "\"");
    ESP8266_Write_Cmd(esp, ssid);
    ESP8266_Write_Cmd(esp, "\"");
    ESP_UNLOCK();
    
    FW_Delay_Ms(1000);
    
    while(ESP8266_String_IsExist(esp, "+CWLAP:") != True)
    {
        if(ESP8266_String_IsExist(esp, "OK") == True)
        {
            /* 指定SSID的AP不存在 */
            return False;
        }
        
        FW_Delay_Ms(SEND_TIMEOUT * 10);
        
        if(cnt++ > SEND_CNT_MAX)
        {
            /* 模组超时未回复(硬件可能有问题) */
            return False;
        }
    }
    
    return True;
}

static u8   ESP8266_AP_Link(WiFi_Type *dev, WiFi_AP_Info_Type *info)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    char *buffer, *pstr;
    u16 cnt = 0;
    u8 resend = 0;
    u8 tmp;
    
    SEND:
    ESP8266_Clear_RBuff(esp);
    
    ESP_LOCK();
    ESP8266_Write_Cmd(esp, "AT+CWJAP=");
    ESP8266_Write_Cmd(esp, "\"");
    ESP8266_Write_Cmd(esp, info->SSID);
    ESP8266_Write_Cmd(esp, "\"");
    ESP8266_Write_Cmd(esp, ",");
    ESP8266_Write_Cmd(esp, "\"");
    ESP8266_Write_Cmd(esp, info->Skey);
    ESP8266_Write_Cmd(esp, "\"");
    ESP8266_Write_Cmd(esp, "\r\n");
    ESP_UNLOCK();
    
    FW_Delay_Ms(2500);
    
    while(ESP8266_String_IsExist(esp, "FAIL") != True ||
          ESP8266_String_IsExist(esp, "OK") != True)
    {
        FW_Delay_Ms(SEND_TIMEOUT * 5);
        if(cnt++ > SEND_CNT_MAX * 5)
        {
            cnt = 0;
            if(resend++ > RESEND_CNT_MAX)
            {
                return WiFi_JAP_NOACK;
            }
            goto SEND;
        }
    }
    
    buffer = ESP8266_Get_RBuff(esp);
    
    pstr = (char *)"+CWJAP:";
    buffer = strstr(buffer, pstr);
    if(buffer)
    {
        buffer += strlen(pstr);
        tmp = *buffer + 0x30;
        if(tmp == ESP8266_JAP_NOAP || tmp == ESP8266_JAP_SkeyErr)
        {
            /* 针对ESP8266某些固件存在的Bug */
            ESP8266_Write_Cmd(esp, "AT+CWJAP=\"esp8266's fb\",\"esp8266's fb\"\r\n");
            FW_Delay_Ms(15000);
        }
        return tmp;
    }
    
    return WiFi_JAP_OK;
}

static Bool ESP8266_AP_Unlink(WiFi_Type *dev)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    ESP8266_Write_Cmd(esp, "AT+CWQAP\r\n");
    FW_Delay_Ms(100);
    return True;
}

static Bool ESP8266_AP_AutoLink(WiFi_Type *dev, u8 state)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    if(state)
    {
        return AT_Send(esp, "AT+CWAUTOCONN=1\r\n", "OK");
    }
    else
    {
        return AT_Send(esp, "AT+CWAUTOCONN=0\r\n", "OK");
    }
}

static Bool ESP8266_Get_APInfo(WiFi_Type *dev, WiFi_AP_Info_Type *info)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    
    char *buffer;
    char *pstr;
    
    if(AT_Send(esp, "AT+CWJAP?\r\n", "OK") != True)
    {
        return False;
    }
    
    buffer = ESP8266_Get_RBuff(esp);
    
    pstr = (char *)"+CWJAP:";
    buffer = strstr(buffer, pstr);
    if(buffer)
    {
        buffer += strlen(pstr);
        
        pstr = strtok(buffer, ",");
        if(pstr)
        {
            strcpy(info->SSID, pstr + 1);
        }
        else
        {
            return False;
        }
        
        pstr = strtok(NULL, ",");
        if(pstr)
        {
            strcpy(info->MAC, pstr + 1);
        }
        else
        {
            return False;
        }
        
        pstr = strtok(NULL, ",");
        if(pstr)
        {
            info->RSSI = atoi(pstr);
        }
        else
        {
            return False;
        }
    }
    else
    {
        return False;
    }
    
    return True;
}

static u8   Itoa(u32 value, char *pdata)
{
    const char alpha_tab[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    u8 len = 0;
    u32 tmp = value;
    u8 i, j;
    
    do
    {
        pdata[len++] = alpha_tab[tmp % 10];
        tmp /= 10;
    }while(tmp);
    
    j = len >> 1;
    
    for(i = 0; i < j; i++)                             //将转换后的字符串倒序(不包含"-")
    {
        pdata[i]       = pdata[i] ^ pdata[len - i];
        pdata[len - i] = pdata[i] ^ pdata[len - i];
        pdata[i]       = pdata[i] ^ pdata[len - i];
    }
    
    pdata[len] = '\0';
    
    return (len + 1);
}

static Bool ESP8266_Server_Link(WiFi_Type *dev, WiFi_Server_Info_Type *info)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    u8 resend = 0;
    u8 cnt = 0;
    char msg[10];
    
    SEND:
    ESP8266_Clear_RBuff(esp);
    
    ESP8266_Write_Cmd(esp, "AT_CIPSTART=");
    ESP8266_Write_Cmd(esp, "\"");
    if(info->Link_Mode == Server_Link_TCP)
    {
        ESP8266_Write_Cmd(esp, "TCP");
    }
    else if(info->Link_Mode == Server_Link_UDP)
    {
        ESP8266_Write_Cmd(esp, "UDP");
    }
    else if(info->Link_Mode == Server_Link_SSL)
    {
        ESP8266_Write_Cmd(esp, "SSL");
    }
    else
    {}
    ESP8266_Write_Cmd(esp, "\"");
    ESP8266_Write_Cmd(esp, ",");
    ESP8266_Write_Cmd(esp, "\"");
    ESP8266_Write_Cmd(esp, info->IP);
    ESP8266_Write_Cmd(esp, "\"");
    ESP8266_Write_Cmd(esp, ",");
    ESP8266_Write_Cmd(esp, "\"");
    ESP8266_Write_Cmd(esp, info->Port);
    ESP8266_Write_Cmd(esp, "\"");
    ESP8266_Write_Cmd(esp, ",");
    if(info->Timeout == 0)
    {
        ESP8266_Write_Cmd(esp, "7200");
    }
    else
    {
        Itoa(info->Timeout, msg);
        ESP8266_Write_Cmd(esp, msg);
    }
    ESP8266_Write_Cmd(esp, "\r\n");
    
    while(ESP8266_String_IsExist(esp, "OK") != True)
    {
        if(ESP8266_String_IsExist(esp, "ERROR") == True)
        {
            return False;
        }
        
        FW_Delay_Ms(SEND_WAIT_TIME * 10);
        if(cnt++ > SEND_CNT_MAX)
        {
            cnt = 0;
            if(resend++ > RESEND_CNT_MAX)
            {
                return False;
            }
            goto SEND;
        }
    }
    
    if(ESP8266_String_IsExist(esp, "no ip") == True ||
       ESP8266_String_IsExist(esp, "CLOSED") == True)
    {
        return False;
    }
    
    return True;
}

#define ESP_STATE_TO_WIFI(estate)(\
(estate == ESP8266_State_Init) ? WiFi_State_Init :\
(estate == ESP8266_State_LinkAP) ? WiFi_State_LinkAP :\
(estate == ESP8266_State_LinkServer) ? WiFi_State_LinkServer :\
(estate == ESP8266_State_UnlinkAP) ? WiFi_State_UnlinkAP :\
(estate == ESP8266_State_UnlinkServer) ? WiFi_State_UnlinkServer :\
WiFi_State_Err)

static u8   ESP8266_Get_ServerInfo(WiFi_Type *dev, WiFi_Server_Info_Type *info)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    u8 msg[64] = {0};
    u8 len;
    
    ESP8266_State_Enum estate;
//    WiFi_Server_Info_Type *info = &dev->Server_Info;
    char *buffer, *pstr;
    
    if(AT_Send(esp, "AT+CIPSTATUS\r\n", "OK") != True)
    {
        return WiFi_State_Err;
    }
    
    /*  */
    ESP_LOCK();
    
    len = ESP8266_Read_Inter(esp, "\r\nSTATUS:", "\r\n", msg, sizeof(msg));
    if(len == 1)
    {
        estate = (ESP8266_State_Enum)msg[0];
    }
    else
    {
        ESP_UNLOCK();
        return WiFi_State_Err;
    }
    
    if(estate == ESP8266_State_LinkServer)
    {
        buffer = ESP8266_Get_RBuff(esp);
        len = ESP8266_Get_RDL(esp);
        
        pstr = (char *)"+CIPSTATUS:";
        buffer = strstr(buffer, pstr);
        if(buffer)
        {
            buffer += strlen(pstr);
            
            pstr = strtok(buffer, ",");
            if(!pstr)
            {
                ESP_UNLOCK();
                return WiFi_State_Err;
            }
            
            pstr = strtok(NULL, ",");
            if(pstr)
            {
                if(strcmp(pstr + 1, "TCP") == 0)
                {
                    info->Link_Mode = Server_Link_TCP;
                }
                else if(strcmp(pstr + 1, "UDP") == 0)
                {
                    info->Link_Mode = Server_Link_UDP;
                }
                else
                {
                    info->Link_Mode = Server_Link_Unknown;
                }
            }
            else
            {
                ESP_UNLOCK();
                return WiFi_State_Err;
            }
            
            pstr = strtok(NULL, ",");
            if(pstr)
            {
                memcpy(info->IP, pstr + 1, strlen(pstr) - 2);
            }
            else
            {
                ESP_UNLOCK();
                return WiFi_State_Err;
            }
            
            pstr = strtok(NULL, ",");
            if(pstr)
            {
                memcpy(info->Port, pstr, strlen(pstr));
            }
            else
            {
                ESP_UNLOCK();
                return WiFi_State_Err;
            }
        }
    }
    
    ESP_UNLOCK();
    return ESP_STATE_TO_WIFI(estate);
}

static u8   ESP8266_Get_LinkState(WiFi_Type *dev)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    u8 msg[64] = {0};
    u8 len;
    
    ESP8266_State_Enum estate;
    
    if(AT_Send(esp, "AT+CIPSTATUS\r\n", "OK") != True)
    {
        return WiFi_State_Err;
    }
    
    /*  */
    ESP_LOCK();
    
    len = ESP8266_Read_Inter(esp, "\r\nSTATUS:", "\r\n", msg, sizeof(msg));
    if(len == 1)
    {
        estate = (ESP8266_State_Enum)msg[0];
    }
    else
    {
        ESP_UNLOCK();
        return WiFi_State_Err;
    }
    
    ESP_UNLOCK();
    return ESP_STATE_TO_WIFI(estate);
}

static Bool ESP8266_Set_TRPT(WiFi_Type *dev, u8 state)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    
    if(state)
    {
        if(AT_Send(esp, "AT+CIPMODE=1\r\n", "OK") != True)
        {
            return False;
        }
        
        if(AT_Send(esp, "AT+CIPSEND\r\n", ">") != True)
        {
            return False;
        }
        
        return True;
    }
    else
    {
        ESP8266_Write_Cmd(esp, "+++\r\n");
        FW_Delay_Ms(100);
        return True;
    }
}

static u32  ESP8266_Write(WiFi_Type *dev, const u8 *pdata, u32 num)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    char num_str[5] = {0};
    u8 resend = 0;
    u8 cnt = 0;
    
    if(num == 0)  return 0;
    
    if(dev->TRPT_EN)
    {
        ESP8266_Write_Msg(esp, pdata, num);
        return num;
    }
    
    SEND:
    ESP8266_Clear_RBuff(esp);
    
    Itoa(num, num_str);
    
    ESP8266_Write_Cmd(esp, "AT+CIPSEND=");
    ESP8266_Write_Cmd(esp, num_str);
    ESP8266_Write_Cmd(esp, "\r\n");
    
    while(ESP8266_String_IsExist(esp, "OK") != True &&
          ESP8266_String_IsExist(esp, ">") != True)
    {
        FW_Delay_Ms(SEND_WAIT_TIME * 5);
        
        if(cnt++ > SEND_CNT_MAX)
        {
            cnt = 0;
            if(resend++ > RESEND_CNT_MAX)
            {
                return 0;
            }
            goto SEND;
        }
    }
    
    ESP8266_Write_Msg(esp, pdata, num);
    
    cnt = 0, resend = 0;
    while(ESP8266_String_IsExist(esp, "SEND OK") != True)
    {
        if(ESP8266_String_IsExist(esp, "ERROR") == True)
        {
            return 0;
        }
        
        FW_Delay_Ms(SEND_WAIT_TIME);
        
        if(cnt++ > SEND_CNT_MAX)
        {
            return 0;
        }
    }
    
    return num;
}

static u32  ESP8266_Read(WiFi_Type *dev, u8 *pdata, u32 num)
{
    ESP8266_Type *esp = FW_Device_GetParent(dev);
    FW_UART_Type *uart = FW_Device_GetParent(esp);
    char msg[5] = {0};
    u32 len;
    u16 i;
    
    if(num == 0)  return 0;
    
    if(dev->TRPT_EN)
    {
        return ESP8266_Read_Msg(esp, pdata, num);
    }
    
    len = ESP8266_Read_Inter(esp, "+IPD,", ":", (u8 *)msg, sizeof(msg));
    if(len)
    {
        len = atoi(msg);
        while(ESP8266_Get_RDL(esp) < len)
        {
            FW_Delay_Ms(len * 1000 * 8 / uart->Baudrate);
            i++;
            if(i > 1000)  break;
        }
        len = ESP8266_Read_Msg(esp, pdata, len);
    }
    
    return len;
}

__CONST_STATIC_ WiFi_Driver_Type Driver =
{
    .Init           = ESP8266_Init,
    
    .Reset          = ESP8266_Reset,
    .Restore        = ESP8266_Restore,
    
    .Enter_LPM      = ESP8266_Enter_LPM,
    .Exit_LPM       = ESP8266_Exit_LPM,
    
    .Set_Mode       = ESP8266_Set_Mode,
    .Get_Mode       = ESP8266_Get_Mode,
    
    .Set_LocalInfo  = ESP8266_Set_LocalInfo,
    .Get_LocalInfo  = ESP8266_Get_LocalInof,
    
    .AP_Probe       = ESP8266_AP_Probe,
    .AP_Link        = ESP8266_AP_Link,
    .AP_Unlink      = ESP8266_AP_Unlink,
    .AP_AutoLink    = ESP8266_AP_AutoLink,
    .Get_APInfo     = ESP8266_Get_APInfo,
    
    .Server_Link    = ESP8266_Server_Link,
    .Get_ServerInfo = ESP8266_Get_ServerInfo,
    
    .Get_LinkState  = ESP8266_Get_LinkState,
    
    .Set_TRPT       = ESP8266_Set_TRPT,
    
    .Write          = ESP8266_Write,
    .Read           = ESP8266_Read,
};
FW_DRIVER_REGIST("esp8266->wifi", &Driver, ESP8266);




#include "fw_debug.h"
#if MODULE_TEST && WIFI_TEST && ESP8266_TEST


#define WIFI_UART_NAME       "uart3"

#define WIFI_MODULE_NAME     "esp8266"
static  ESP8266_Type         ESP8266;
static  void ESP8266_Config(void *dev)
{
    ESP8266_Type *esp = dev;
    FW_UART_Type *uart = FW_Device_Find(WIFI_UART_NAME);
    
    FW_Device_SetParent(dev, uart);
    FW_Device_SetDriver(dev, FW_Driver_Find("uart->dev"));
}
FW_DEVICE_STATIC_REGIST(WIFI_MODULE_NAME, &ESP8266, ESP8266_Config, ESP8266);


void Test(void)
{
    
}

#endif

