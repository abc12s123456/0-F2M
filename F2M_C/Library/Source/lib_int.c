#include "lib_int.h"


/**
@功能: 比较两个整数并返回较大值
@参数: num1, 参与比较的数值
       num2, 参与比较的数值
@返回: 较大值
@备注: @无
*/
s32 Int_Max(s32 num1, s32 num2)
{
    return ((num1 >= num2) ? num1 : num2);
}
/**/

/**
@功能: 比较两个整数并返回较小值
@参数: num1, 参与比较的数值
       num2, 参与比较的数值
@返回: 较小值
@备注: @无
*/
s32 Int_Min(s32 num1, s32 num2)
{
    return ((num1 >= num2) ? num2 : num1);
}
/**/

/**
@功能: 数字以10进制方式转换为ASCII字符串
@参数: value, 待转换的数字，有符号
       pdata, 存储转换后的字符串
@返回: 转换后的字符串长度
@备注: @转换结果包含末尾的'\0'
       @用户应保证pdata足够保存转换的的字符串，否则会导致内存访问越界
*/
u8  Itoa(s32 value, char *pdata)
{
    char alpha_tab[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    u8 length = 0;
    s32 temp;
    u8 i = 0, j, k;
    
    temp = value;
    
    if(temp & 0x80000000)                         //待转换值为负数
    {
        pdata[0] = '-';                           //则第一个字符为负号
        length++;
        temp = ~(value - 1);                      //转换为正数
        i = 1;
    }
    
    do
    {
        pdata[length++] = alpha_tab[temp % 10];   //从个位开始转换
        temp /= 10;                               //进制移位，判断位数
    }while(temp);
    
    j = i ^ 1;                                    //调整字符串长度
    k = (length + i) >> 1;
    
    for(; i < k; i++)                             //将转换后的字符串倒序(不包含"-")
    {
        pdata[i]              = pdata[i] ^ pdata[length - i - j];
        pdata[length - i - j] = pdata[i] ^ pdata[length - i - j];
        pdata[i]              = pdata[i] ^ pdata[length - i - j];
    }
    
    pdata[length] = '\0';
    
    return (length + 1);
}
/**/

/**
@功能: 数字以16进制方式转换为ASCII字符串
@参数: value, 待转换的数字，有符号
       pdata, 存储转换后的字符串
@返回: 转换后的字符串长度
@备注: @转换结果包含末尾的'\0'
       @用户应保证pdata足够保存转换的的字符串，否则会导致内存访问越界
*/
u8 Htoa(u32 value, char *pdata)
{
    char alpha_tab[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    u8 length = 0;
    u8 i;
    
    do
    {
        pdata[length++] = alpha_tab[value % 16];   //从最低位开始转换
        value >>= 4;
    }while(value);
    
    for(i = 0; i < length / 2; i++)
    {
        pdata[i]          = pdata[i] ^ pdata[length - 1 - i];
        pdata[length - 1 - i] = pdata[i] ^ pdata[length - 1 - i];
        pdata[i]          = pdata[i] ^ pdata[length - 1 - i];
    }
    
    pdata[length] = '\0';
    
    return (length + 1);
}

/**
@功能: 获取给定数值的bit流中的0/1的数量
@参数: num, 给定的数值
       value, 0/1
@返回: bit流中0/1的数量
@备注: 无
*/
u8  Int_GetBits(u32 num, u8 value)
{
    num = (num & 0x55555555) + ((num >> 1) & 0x55555555);
    num = (num & 0x33333333) + ((num >> 2) & 0x33333333);
    num = (num & 0x0F0F0F0F) + ((num >> 4) & 0x0F0F0F0F);
    num = (num & 0x00FF00FF) + ((num >> 8) & 0x00FF00FF);
    num = (num & 0x0000FFFF) + ((num >> 16) & 0x0000FFFF);
    return (value) ? num : (32 - num);
}
/**/

/**
@功能: 改变数值的大小端
@参数: value, 待转换的数值
@返回: 转换后的数值
@备注: 无
*/
u32 Int32_EndianExchange(u32 value)
{
    u8 b0 = (u8)value;
    u8 b1 = (u8)(value >> 8);
    u8 b2 = (u8)(value >> 16);
    u8 b3 = (u8)(value >> 24);
    return (u32)(b3 | (b2 << 8) | (b1 << 16) | (b0 << 24));
}

u16 Int16_EndianExchange(u16 value)
{
    u8 b0 = (u8)value;
    u8 b1 = (u8)(value >> 8);
    return (u16)(b1 | (b0 << 8)); 
}
/**/

/**
@功能: 以指定数据类型读取缓存中的数值
@参数: pdata, 数据缓存
       index, 读取下标
       width, 数据类型长度(sizeof(u8/u16/u32))
@返回: 读取到的数值
@备注: 无
*/
u32 Int_ReadBuffer(void *pdata, u32 index, u8 width)
{
    if(width == sizeof(u32))  return (u32)((u32 *)pdata)[index];
    if(width == sizeof(u16))  return (u32)((u16 *)pdata)[index];
    return (u32)((u8 *)pdata)[index];
}
/**/

/**
@功能: 读取缓存中的数值
@参数: pdata, 数据缓存
       index, 读取下标
@返回: 读取到的数值
@备注: @index的值不应该超出数组的长度(以相应的数据类型计算)，但由于是只读，所以不会导致
        系统异常，但可能得到异常值
*/
u32 Int32_ReadBuffer(void *pdata, u32 index)
{
    return (u32)((u32 *)pdata)[index];
}

u32 Int16_ReadBuffer(void *pdata, u32 index)
{
    return (u32)((u16 *)pdata)[index];
}

u32 Int8_ReadBuffer(void *pdata, u32 index)
{
    return (u32)((u8 *)pdata)[index];
}
/**/

/**
@功能: 获取一组数据的异或值
@参数: pdata, 数据缓存
       start_value, 起始值
       num, 数组长度, 单位：byte
@返回: 异或值计算结果
@备注: @若不需要起始值，则起始值应设为0x00(异或值计算：相同为0，不同为1)
*/
u8  Int8_GetXor(void *pdata, u8 start_value, u32 num)
{
    u8 *p = (u8 *)pdata;
    
    while(num--)
    {
        start_value = start_value ^ *p++;
    }
    
    return start_value;
}

/**
@功能: 获取一组数据的和值
@参数: pdata, 数据缓存
       start_value, 起始值
       num, 数组长度, 单位：byte
@返回: 和值计算结果
@备注: @若不需要起始值，则起始值应设为0x00
*/
u8  Int8_GetSum(void *pdata, u8 start_value, u32 num)
{
    u8 *p = (u8 *)pdata;
    
    while(num--)
    {
        start_value += *p++;
    }
    
    return start_value;
}
/**/


void Int_Average_Init(Int_Average_Type *ave, u16 times)
{
    ave->Times = times;
    ave->Value = 0;
    ave->Sum = 0;
    ave->Count = 0;
}

u32  Int_Average_Get(Int_Average_Type *ave, u32 value)
{
    ave->Sum += value;
    
    if(++ave->Count >= ave->Times)
    {
        ave->Value = ave->Sum / ave->Times;
        ave->Sum = 0;
        ave->Count = 0;
    }
    
    return ave->Value;
}

