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
 
#include "time.h"
#include <string.h>


/**
@功能: 判断给定的年份是不是闰年
@参数: year, 给定的年份
@返回: True, 闰年
       False, 平年
@备注: 1, 普通年能被4整除且不能被100整除的为闰年。
       2, 世纪年能被400整除的是闰年
       3, 对于数值很大的年份,这年如果能整除3200,并且能整除172800则是闰年。
	      如172800年是闰年，86400年不是闰年
		  置闰法则:四年一闰;百年不闰,四百年再闰
*/
Bool Time_IsLeapYear(u16 year)
{
    if(year % 4 == 0)                             //闰年必须能被4整除
    { 
        if(year % 100 == 0) 					  //世纪年
        { 
            if(year % 400 == 0)  return True;     //如果以00结尾,还要能被400整除 	   
            else return False;   
        }
        else return True;   
    }
    else return False;  
}
/**/

/**
@功能: 根据给定的年月日，判断这一天是周几
@参数: year, 年份
       month, 月份
       date, 日期
@返回: 给定日期为周几数
@备注: 该函数是通过蔡勒（Zeller）公式来计算的，可以计算一个任意给定的日期是
       星期几。网上有原理推导，可以查阅。
       计算公式：w=(y + [y/4] + [c/4] - 2c + [26(m+1)/10] + d -1) % 7,其中：
       w:星期数，余数是几就是星期几，当w=0时表示周日
	   y:年数，例如2014年则y=14
	   c:世纪数-1，例如2014年则c=21-1=20
	   m:月份，例如2014.12.10则m=12;比较特殊的是1,2月份要看作去年的13,14月份来计
	     算，例如2015.1.2需要看作2014.13.2来计算
	   d:日期，例如2014.12.10则d=10
*/
u8 Time_GetWeek(u16 year, u8 month, u8 day)
{
    u8 week;
    u8 yearH, yearL;

    if(month == 1 || month == 2)  
    {
        month += 12;
        year -= 1;
    }

    yearH = year / 100;
    yearL = year % 100;

    week = (yearL + yearL/4 + yearH/4 - 2*yearH + 26*(month+1)/10 + day -1) % 7;//蔡勒（Zeller）公式
    return ((week == 0) ? 7 : week);              //Sunday = 7
}
/**/

const static u8 Month_Days_Table[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; //平年月份
/**
@功能: 根据给定的年月，获取该月的天数
@参数: year, 年份
       month, 月份
@返回: 当前月份的天数
@备注: @无
*/
u8 Time_GetDays(u16 year, u8 month)
{
    u8 days = Month_Days_Table[month - 1];
    if(Time_IsLeapYear(year) == True && month == 2)  days++;
    return days;
}
/**/

#define BitChar_To_Num(c, carry)  ((c - '0') * carry)
/**
@功能: 获取程序的编译时间
@参数: Time, 时间操作对象
@返回: 无
@备注: @__DATE__，__TIME__为ANSI C中的标准宏
*/
void Time_GetCompile(Time_Type *Time)
{
    const char Month_String_Tab[12][4] =
    {
        "Jan", "Feb", "Mar", "Apr",
        "May", "Jun", "Jul", "Aug",
        "Sep", "Oct", "Nov", "Dec"
    }; 
    const char *date = __DATE__;
    const char *time = __TIME__;
    
    for(u8 i = 0; i < 12; i++)
    {
        if(strstr(date, Month_String_Tab[i]) != NULL)
        {
            Time->Month = i + 1;
            break;
        }
    }
    
    Time->Day = BitChar_To_Num(date[4], 10) + BitChar_To_Num(date[5], 1);
    Time->Year = BitChar_To_Num(date[7], 1000) + 
                 BitChar_To_Num(date[8], 100) +
                 BitChar_To_Num(date[9], 10) +
                 BitChar_To_Num(date[10], 1);
    Time->Week = Time_GetWeek(Time->Year, Time->Month, Time->Day);
    
    Time->Hour = BitChar_To_Num(time[0], 10) + BitChar_To_Num(time[1], 1);
    Time->Minute = BitChar_To_Num(time[3], 10) + BitChar_To_Num(time[4], 1);
    Time->Second = BitChar_To_Num(time[6], 10) + BitChar_To_Num(time[7], 1);
}
/**/

/**
@功能: 由给定的时间及起始元年获取秒计数
@参数: Time, 时间操作对象
       first_year, 起始元年
@返回: 秒计数
@备注: @计数是从计时元年开始计算
*/
u32  Time_GetCounter(Time_Type *Time, u16 first_year)
{
    u32 count;
    u16 t;
    Bool isLearYear;
    
    /* 设置的时间不能在起始元年之前 */
    if(Time->Year < first_year)  return 0;
    
    count = 0;
    isLearYear = Time_IsLeapYear(Time->Year);
    
    /* 所有年份的秒数 */
    for(t = first_year; t < Time->Year; t++)  
    {
        if(Time_IsLeapYear(t) == True)
        {
            count += SECONDS_PER_LEAP_YEAR;
        }
        else
        {
            count += SECONDS_PER_NOMAL_YEAR;
        }
    }
    
    /* 当前年份的月份的秒数 */
    for(t = 0; t < Time->Month - 1; t++)
    {
        count += (u32)(Month_Days_Table[t] * SECONDS_PER_DAY);
        if(isLearYear && t == 1)  count += SECONDS_PER_DAY;   //闰年二月份增加一天
    }
    
    count += (u32)((Time->Day - 1) * SECONDS_PER_DAY);
    count += (u32)(Time->Hour * 3600);
    count += (u32)(Time->Minute * 60);
    count += Time->Second;
    
    return count;
}
/**/

/**
@功能: 由给定的秒计数及起始元年获取时间
@参数: Time, 时间操作对象，保存转换后的时间
       first_year, 起始元年
       counter, 秒计数
@返回: 无
@备注: @
@修改：2020-12-22，修复了多个Time对象同时调用Time_GetTime时，年月日无法更新的问题
*/
void Time_GetTime(Time_Type *Time, u16 first_year, u32 counter)
{
    u32 tmp1, tmp2;
    
    tmp1 = counter / SECONDS_PER_DAY;            //对应的天数
    
    if(Time->Day_Count != tmp1)                  //需要更新年月日
    {
        Time->Day_Count = tmp1;
        tmp2 = first_year;                       //从起始元年开始计算
        while(tmp1 >= 365)
        {
            if(Time_IsLeapYear(tmp2))            //闰年
            {
                if(tmp1 >= 366)
                {
                    tmp1 -= 366;
                }
                else
                {
                    tmp2++;
                    break;
                }
            }
            else                                 //平年
            {
                tmp1 -= 365;
            }
            tmp2++;
        }
        Time->Year = tmp2;                       //得到年份
        
        tmp2 = 0;
        while(tmp1 >= 28)                        //超过一个月
        {
            if(Time_IsLeapYear(Time->Year) && tmp2 == 1)
            {
                if(tmp1 >= 29)
                {
                    tmp1 -= 29;
                }
                else
                {
                    break;
                }
            }
            else
            {
                if(tmp1 >= Month_Days_Table[tmp2])
                {
                    tmp1 -= Month_Days_Table[tmp2];
                }
                else
                {
                    break;
                }
            }
            tmp2++;
        }
        Time->Month = tmp2 + 1;                  //得到月份
        Time->Day = tmp1 + 1;                    //得到天
    }
    
    tmp1 = counter % SECONDS_PER_DAY;
    Time->Hour   = tmp1 / 3600;
    Time->Minute = (tmp1 % 3600) / 60;
    Time->Second = (tmp1 % 3600) % 60;
    Time->Week   = Time_GetWeek(Time->Year, Time->Month, Time->Day);
}
/**/

/**
@功能: 时间参数合法性检验
@参数: year~second, 时间
@返回: True, 参数合法
       False, 参数非法
@备注: 无
*/
Bool Time_Assert(u16 year, u8 month, u8 day, u8 hour, u8 minute, u8 second)
{
    if(year > YEAR_MAX ||
       month > MONTH_MAX || month < MONTH_MIN ||
       day > DAY_MAX || day < DAY_MIN ||
       hour > HOUR_MAX ||
       minute > MINUTE_MAX ||
       second > SECOND_MAX)
    {
        return False;
    }
    
    if(month == 2)
    {
        if(Time_IsLeapYear(year) == True)
        {
            if(day > 29)  return False;
        }
        else
        {
            if(day > 28)  return False;
        }
    }
    
    return True;
}
/**/

/**
@功能: 时间参数合法性检验
@参数: Time, 时间结构体
@返回: True, 参数合法
       False, 参数非法
@备注: 无
*/
Bool Time_StructAssert(Time_Type *Time)
{
    return Time_Assert(Time->Year, Time->Month, Time->Day, Time->Hour, Time->Minute, Time->Second);
}
/**/

/**
@功能: 格林尼治时间转北京时间
@参数: Time, 时间结构体
@返回: 无
@备注: 无
*/
void Time_UTCToBeijing(Time_Type *Time)
{
    u8 days = 0;
    
    if(Time_StructAssert(Time) == False)  return;
    
    days = Month_Days_Table[Time->Month];
    if(Time->Month == 2)
    {
        if(Time_IsLeapYear(Time->Year))  days++;
    }
    
    Time->Hour += 8;
    if(Time->Hour >= 24)                          //跨天
    {
        Time->Hour -= 24;
        Time->Day++;
        if(Time->Day > days)                      //跨月
        {
            Time->Day = 1;
            Time->Month++;
            if(Time->Month > 12)                  //跨年
            {
                Time->Year++;
            }
        }
    }
}
/**/

/**
@功能: 北京时间转格林尼治时间
@参数: Time, 时间结构体
@返回: 无
@备注: 无
*/
void Time_BeijingToUTC(Time_Type *Time)
{
    u8 days = 0;
    
    if(Time_StructAssert(Time) == False)  return;
    
    if(Time->Hour < 8)                            //跨天
    {
        if(Time->Day <= 1)                        //跨月
        {
            if(Time->Month <= 1)                  //跨年
            {
                Time->Year--;
                Time->Month = 12;
            }
            else
            {
                Time->Month--;
            }
            
            days = Month_Days_Table[Time->Month];
            if(Time->Month == 2)
            {
                if(Time_IsLeapYear(Time->Year))  days++;
            }
            
            Time->Day = days;
        }
        else
        {
            Time->Day--;
        }
            
        Time->Hour += 24;
    }
    
    Time->Hour -= 8;
}
/**/

