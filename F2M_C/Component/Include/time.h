#ifndef _TIME_H_
#define _TIME_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"


#define  SECONDS_PER_NOMAL_YEAR  (60 * 60 * 24 * 365UL)
#define  SECONDS_PER_LEAP_YEAR   (60 * 60 * 24 * 366UL)
#define  SECONDS_PER_DAY         (60 * 60 * 24)

#define  YEAR_MAX                (9999)
#define  YEAR_MIN                (2020)

#define  MONTH_MAX               (12)
#define  MONTH_MIN               (1)

#define  DAY_MAX                 (31)
#define  DAY_MIN                 (1)

#define  HOUR_MAX                (23)
#define  HOUR_MIN                (0)

#define  MINUTE_MAX              (59)
#define  MINUTE_MIN              (0)

#define  SECOND_MAX              (59)
#define  SECOND_MIN              (0)


/* 周 */
typedef enum
{
    Monday    = 1,
    Tuesday   = 2,
    Wednesday = 3,
    Thursday  = 4,
    Friday    = 5,
    Saturday  = 6,
    Sunday    = 7,
}Week_Enum;


/* 时间相关的操作 */
typedef struct
{
    u32 Year   : 16;
    u32 Month  : 4 ;
    u32 Day    : 6;
    u32 Hour   : 6;
    
    u32 Minute : 6;
    u32 Second : 6;
    
    u32 Week   : 4;
    
    u32 Day_Count : 16;
}Time_Type;


Bool Time_IsLeapYear(u16 year);
u8   Time_GetWeek(u16 year, u8 month, u8 day);
u8   Time_GetDays(u16 year, u8 month);

void Time_GetCompile(Time_Type *Time);

u32  Time_GetCounter(Time_Type *Time, u16 first_year);
void Time_GetTime(Time_Type *Time, u16 first_year, u32 counter);

Bool Time_Assert(u16 year, u8 month, u8 day, u8 hour, u8 minute, u8 second);
Bool Time_StructAssert(Time_Type *Time);

void Time_UTCToBeijing(Time_Type *Time);
void Time_BeijingToUTC(Time_Type *Time);


#ifdef __cplusplus
}
#endif

#endif

