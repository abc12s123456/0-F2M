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
 
#include "lib_bsc.h"
#include <stdlib.h>
#include <string.h>

#include <ctype.h>


u32  Rand(u32 min, u32 max)
{
    u32 tick;
    
    extern u32 FW_Delay_GetTick(void);
    
    tick = FW_Delay_GetTick();
    
    srand(tick);
    
    return (rand() % (max - min + 1) + min);
}

u32  Pow(u32 x, u32 y)
{
    u32 rv;
    
    /* 0的任何次幂都为0，除过0的任何数的0次幂都为1 */
    if(x == 0)  return 0;
    if(y == 0)  return 1;
    
    rv = 1;
    
    while(y--)  rv *= x;
    
    return rv;
}

s32  Round(double value)
{
    return (((value + 0.5) > (s32)value) ? (s32)value + 1 : (s32)value);
}

//s32  Atoi(const char *pstr, Base_Enum base, u8 len)
//{
//	int value = 0;
//    char p[11];                        //32位处理器中，最大数值为FFFFFFFF/4294967295，共计10位
//	u8 i = 0;
//	
//	memset(p, 0, 11);
//	
//	memcpy(p, pstr, len);
//	
//	if(base == DEC)  value = atoi(p);  //按10进制转换
//	
//	else if(base == HEX)               //按16进制转换
//	{
//        if(p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))  i += 2; //16进制数的前缀0X/0x

//        while(p[i])
//        {
//            /* 从字符串的最高位开始转换,每循环一次，最高位就要向左偏移一个进制数 */
//            value <<= 4;                  

//            if('0' <= p[i] && p[i] <= '9')
//            {
//                value += p[i] - '0';
//            }
//            else if('a' <= p[i] && p[i] <= 'f')
//            {
//                value += *p - 'a' + 10;
//            }
//            else if('A' <= p[i] && p[i] <= 'F')
//            {
//                value += p[i] - 'A' + 10;
//            }
//            else  
//            {
//                /* 当无效字符不为0时，前面value会多做左移运算，在这里进行右移运算恢复 */
//                value >>= 4;                
//                break;
//            }

//            i++;
//        }
//	}
//	
//	return  value;
//}

//u8   Itoa(const s32 value, Base_Enum base, char *pstr)
//{
//    char alpha_tab[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
//    u8  len = 0;
//    s32 pd;
//    u8  i = 0, j, k;

//    if(base <= 1 || base > 16)  return 0;    //

//    pd = value;

//    if(pd & 0x80000000)                     //判断是否为负数
//    {
//        pstr[0] = '-';                      //第一个字符为“-”
//        len++;

//        pd = ~(value - 1);                  //将负数转换为正数

//        i = 1;
//    }

//    do
//    {
//        pstr[len++] = alpha_tab[pd % base]; //从个位开始转换
//        pd /= base;                         //通过进制移位判断数据的位数
//    }while(pd);

//    j = i ^ 1;
//    k = i;

//    /* 将字符串进行倒序排列 */
//    for(i = i; i < (len + k) >> 1; i++)
//    {
//        pstr[i]           = pstr[i] ^ pstr[len - i - j];
//        pstr[len - i - j] = pstr[i] ^ pstr[len - i - j];
//        pstr[i]           = pstr[i] ^ pstr[len - i - j];
//    }

//    return  len;
//}

//int  Atoi(const char *npstr)
//{
//    int c;              /* current char */
//    long total;         /* current total */
//    int sign;           /* if '-', then negative, otherwise positive */

//    /* skip whitespace */
//    while ( isspace((int)(u8)*npstr) )  ++npstr;

//    c = (int)(u8)*npstr++;
//    sign = c;                               /* save sign indication */
//    if (c == '-' || c == '+')
//    c = (int)(u8)*npstr++;                  /* skip sign */

//    total = 0;

//    while (isdigit(c)) 
//    {
//        total = 10 * total + (c - '0');     /* accumulate digit */
//        c = (int)(u8)*npstr++;              /* get next char */
//    }

//    if(sign == '-')  return -total;
//    else  return total;                     /* return result, negated if necessary */
//}












