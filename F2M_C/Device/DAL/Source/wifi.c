#include "wifi.h"



void WiFi_Init(WiFi_Type *dev)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Init(dev);
}

Bool WiFi_Reset(WiFi_Type *dev, u8 hs)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Reset(dev, hs);
}

Bool WiFi_Restore(WiFi_Type *dev)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Restore(dev);
}

Bool WiFi_Enter_LPM(WiFi_Type *dev, WiFi_LPM_Enum lpm)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Enter_LPM(dev, lpm);
}

Bool WiFi_Exit_LPM(WiFi_Type *dev)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Exit_LPM(dev);
}

Bool WiFi_Set_Mode(WiFi_Type *dev, WiFi_Mode_Enum mode)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Set_Mode(dev, mode);
}

u8   WiFi_Get_Mode(WiFi_Type *dev)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Get_Mode(dev);
}

Bool WiFi_Set_LocalInfo(WiFi_Type *dev, WiFi_Local_Info_Type *info)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Set_LocalInfo(dev, info);
}

Bool WiFi_Get_LocalInfo(WiFi_Type *dev, WiFi_Local_Info_Type *info)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    if(drv->Get_LocalInfo(dev, info) != True)
    {
        return False;
    }
    else
    {
        memcpy(&dev->Local_Info, info, sizeof(WiFi_Local_Info_Type));
        return True;
    }
}

Bool WiFi_AP_Probe(WiFi_Type *dev, const char *ssid)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->AP_Probe(dev, ssid);
}

u8   WiFi_AP_Link(WiFi_Type *dev, WiFi_AP_Info_Type *info)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->AP_Link(dev, info);
}

Bool WiFi_AP_Unlink(WiFi_Type *dev)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->AP_Unlink(dev);
}

Bool WiFi_AP_AutoLink(WiFi_Type *dev, u8 state)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->AP_AutoLink(dev, state);
}

Bool WiFi_Get_APInfo(WiFi_Type *dev, WiFi_AP_Info_Type *info)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    if(drv->Get_APInfo(dev, info) != True)
    {
        return False;
    }
    else
    {
        memcpy(&dev->AP_Info, info, sizeof(WiFi_AP_Info_Type));
        return True;
    }
}

Bool WiFi_Server_Link(WiFi_Type *dev, WiFi_Server_Info_Type *info)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Server_Link(dev, info);
}

Bool WiFi_Get_ServerInfo(WiFi_Type *dev, WiFi_Server_Info_Type *info)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    if(drv->Get_ServerInfo(dev, info) != True)
    {
        return False;
    }
    else
    {
        memcpy(&dev->Server_Info, info, sizeof(WiFi_Server_Info_Type));
        return True;
    }
}

u8   WiFi_Get_LinkState(WiFi_Type *dev)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Get_LinkState(dev);
}

Bool WiFi_Set_TRPT(WiFi_Type *dev, u8 state)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Set_TRPT(dev, state);
}

u32  WiFi_Write(WiFi_Type *dev, const u8 *pdata, u32 num)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Write(dev, pdata, num);
}

u32  WiFi_Read(WiFi_Type *dev, u8 *pdata, u32 num)
{
    WiFi_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Read(dev, pdata, num);
}

