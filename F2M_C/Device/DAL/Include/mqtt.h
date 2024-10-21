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
#ifndef _MQTT_H_
#define _MQTT_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef struct
{
    FW_Device_Type Device;
    
    char *Client_Link;
    char *Sever;
    
    char *Publish_Theme;
    char *Publish_Payload;
    
    char *Subscribe_Theme;
    char *Subscribe_Payload;
}MQTT_Type;


typedef struct
{
    void (*Init)(MQTT_Type *dev);
    
    Bool (*Reset)(MQTT_Type *dev, u8 hs);
    Bool (*Restore)(MQTT_Type *dev);
    
    Bool (*Enter_LPM)(MQTT_Type *dev, u8 mode);
    Bool (*Exit_LPM)(MQTT_Type *dev);
    
    Bool (*Connect)(MQTT_Type *dev);
    Bool (*Disconnect)(MQTT_Type *dev);
    
    Bool (*Subscribe)(MQTT_Type *dev);
    Bool (*Unsubscribe)(MQTT_Type *dev);
    
    Bool (*Publish)(MQTT_Type *dev);
}MQTT_Driver_Type;


#ifdef __cplusplus
}
#endif

#endif

