/*
 * F2M
 * Copyright (C) 2024 abc12s123456 382797263@qq.com.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://github.com/abc12s123456/F2M
 *
 */
 
#ifndef _WIFI_H_
#define _WIFI_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


/* WiFi名称、秘钥的最大长度，包含‘\0’ */
#define WIFI_SSID_LEN_MAX              33
#define WIFI_SKEY_LEN_MAX              17

#define WIFI_MAC_LEN_MAX               18

#define WIFI_IP_LEN_MAX                16
#define WIFI_PORT_LEN_MAX              6

#define WIFI_MASK_LEN_MAX              16
#define WIFI_GATEWAY_LEN_MAX           16


typedef enum
{
    WiFi_Mode_Unknown = 0,
    WiFi_Mode_Station = 1,
    WiFi_Mode_AP,
    WiFi_Mode_Station_AP,
}WiFi_Mode_Enum;


typedef enum
{
    Server_Link_TCP,
    Server_Link_UDP,
    Server_Link_SSL,
    Server_Link_Unknown,
}Server_Link_Enum;


typedef enum
{
    WiFi_LPM_None = 0,
    WiFi_LPM_Light,
    WiFi_LPM_Deep,
}WiFi_LPM_Enum;


typedef enum
{
    WiFi_State_Init = 0,               //初始状态
    WiFi_State_LinkAP,                 //AP连接
    WiFi_State_LinkServer,             //服务器连接
    WiFi_State_UnlinkAP,               //AP断开
    WiFi_State_UnlinkServer,           //服务器断开
    WiFi_State_Err,
}WiFi_State_Enum;


typedef enum
{
    WiFi_JAP_OK = 0,                   //AP加入成功
    WiFi_JAP_Timeout,                  //加入超时
    WiFi_JAP_SkeyErr,                  //秘钥错误
    WiFi_JAP_NOAP,                     //无AP
    WiFi_JAP_FAIL,                     //AP加入失败
    WiFi_JAP_NOACK,                    //AP无应答
}WiFi_JAP_Enum;


/* 模组自身信息 */
typedef struct
{
    char IP[WIFI_IP_LEN_MAX];          //
    char Subnet_Mask[WIFI_MASK_LEN_MAX];
    char Gateway[WIFI_GATEWAY_LEN_MAX];
    char MAC[WIFI_MAC_LEN_MAX];        //
    
    u8   DHCP_EN : 1;                  //若模组为AP模式，表示IP的分配；若模组为Station模式，表示自身IP为静态或动态
    
    u8   : 7;
}WiFi_Local_Info_Type;


/* 模组连接的无线路由器信息 */
typedef struct
{
    char SSID[WIFI_SSID_LEN_MAX];      //名称
    char Skey[WIFI_SKEY_LEN_MAX];      //秘钥
    char IP[WIFI_IP_LEN_MAX];          //IP
    char MAC[WIFI_MAC_LEN_MAX];        //MAC
    u8   Channel;                      //Channel，模组与路由的连接通道号
    s8   RSSI;                         //模组检测与路由的连接信号强度

    u8   DHCP_EN : 1;
    
    u8   : 7;
}WiFi_AP_Info_Type;


/* 服务器信息 */
typedef struct
{
    char IP[WIFI_IP_LEN_MAX];
    char Port[WIFI_PORT_LEN_MAX];
    u32  Timeout : 16;
    Server_Link_Enum Link_Mode;
}WiFi_Server_Info_Type;


typedef struct
{
    FW_Device_Type Device;
    
    u32 Mode : 2;                      //工作模式，参照WiFi_Mode_Enum
    u32 TRPT_EN : 1;                   //透传模式，ON:打开，OFF:关闭
    
    u32 : 29;
    
    WiFi_Local_Info_Type  Local_Info;
    WiFi_AP_Info_Type     AP_Info;
    WiFi_Server_Info_Type Server_Info;
}WiFi_Type;


typedef struct
{
    void (*Init)(WiFi_Type *dev);
    
    Bool (*Reset)(WiFi_Type *dev, u8 hs);
    Bool (*Restore)(WiFi_Type *dev);
    
    Bool (*Enter_LPM)(WiFi_Type *dev, WiFi_LPM_Enum lpm);
    Bool (*Exit_LPM)(WiFi_Type *dev);
    
    Bool (*Set_Mode)(WiFi_Type *dev, WiFi_Mode_Enum mode);
    u8   (*Get_Mode)(WiFi_Type *dev);
    
    Bool (*Set_LocalInfo)(WiFi_Type *dev, WiFi_Local_Info_Type *info);
    Bool (*Get_LocalInfo)(WiFi_Type *dev, WiFi_Local_Info_Type *info);
    
    Bool (*AP_Probe)(WiFi_Type *dev, const char *ssid);
    u8   (*AP_Link)(WiFi_Type *dev, WiFi_AP_Info_Type *ap);
    Bool (*AP_Unlink)(WiFi_Type *dev);
    Bool (*AP_AutoLink)(WiFi_Type *dev, u8 state);
    Bool (*Get_APInfo)(WiFi_Type *dev, WiFi_AP_Info_Type *info);
    
    Bool (*Server_Link)(WiFi_Type *dev, WiFi_Server_Info_Type *server);
    u8   (*Get_ServerInfo)(WiFi_Type *dev, WiFi_Server_Info_Type *info);
    
    u8   (*Get_LinkState)(WiFi_Type *dev);
    
    Bool (*Set_TRPT)(WiFi_Type *dev, u8 state);                                 //设置透传模式
    
    u32  (*Write)(WiFi_Type *dev, const u8 *pdata, u32 num);
    u32  (*Read)(WiFi_Type *dev, u8 *pdata, u32 num);
}WiFi_Driver_Type;


void WiFi_Init(WiFi_Type *dev);

Bool WiFi_Reset(WiFi_Type *dev, u8 hs);
Bool WiFi_Restore(WiFi_Type *dev);

Bool WiFi_Enter_LPM(WiFi_Type *dev, WiFi_LPM_Enum lpm);
Bool WiFi_Exit_LPM(WiFi_Type *dev);

Bool WiFi_Set_Mode(WiFi_Type *dev, WiFi_Mode_Enum mode);
u8   WiFi_Get_Mode(WiFi_Type *dev);

Bool WiFi_Set_LocalInfo(WiFi_Type *dev, WiFi_Local_Info_Type *info);
Bool WiFi_Get_LocalInfo(WiFi_Type *dev, WiFi_Local_Info_Type *info);

Bool WiFi_AP_Probe(WiFi_Type *dev, const char *ssid);
u8   WiFi_AP_Link(WiFi_Type *dev, WiFi_AP_Info_Type *info);
Bool WiFi_AP_Unlink(WiFi_Type *dev);
Bool WiFi_AP_AutoLink(WiFi_Type *dev, u8 state);
Bool WiFi_Get_APInfo(WiFi_Type *dev, WiFi_AP_Info_Type *info);

Bool WiFi_Server_Link(WiFi_Type *dev, WiFi_Server_Info_Type *info);
Bool WiFi_Get_ServerInfo(WiFi_Type *dev, WiFi_Server_Info_Type *info);

u8   WiFi_Get_LinkState(WiFi_Type *dev);

Bool WiFi_Set_TRPT(WiFi_Type *dev, u8 state);

u32  WiFi_Write(WiFi_Type *dev, const u8 *pdata, u32 num);
u32  WiFi_Read(WiFi_Type *dev, u8 *pdata, u32 num);


#ifdef __cplusplus
}
#endif

#endif

