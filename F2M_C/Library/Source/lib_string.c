#include "lib_string.h"

#include "libc.h"


/**
@功能: 目标字符串中查找指定字符串
@参数: dst, 目标字符串，可以不包含末尾的'\0'
       s, 指定字符串
       num, 目标字符串的长度
@返回: NULL, 目标字符串中不存在指定的字符串
       其它, 指定字符串在目标字符串中的起始地址
@修改: @指定了比较长度，防止内存越界
       @20210119，修复，(cp - dst) > num - 1 调整为 (cp - dst) >= num - 1
*/
char *StrFind(const char *dst, const char *s, u32 num)
{
    if(*s == '\0')  return (char *)dst;
    if(num == 0)  return NULL;
    
    char *cp = (char *)dst;
    char *s1, *s2;
    
    while(*cp)
    {
        s1 = cp;
        s2 = (char *)s;
        
        while(*s1 && *s2 && !(*s1 - *s2))
        {
            s1++;
            s2++;
        }
        
        if(*s2 == '\0')  return (cp);
        if((cp - dst) >= num - 1)  return NULL;
        cp++;
    }
    
    return NULL;
}
/**/

/**
@功能: 字符串比较
@参数: src, 用于比较的左侧字符串
       dst, 用于比较的右侧字符串
@返回: -1， src < dst
       0, src = dst
       1, src > dst
@备注: @指定了比较长度，防止内存越界
*/
s8 Strcmp(const char *src, const char *dst, u32 num)
{
    if(num == 0)  return 0;
    s8 ret = 0;
    u32 cnt = 0;
    while(!(*(u8 *)src - *(u8 *)dst) && *dst)
    {
        ret = *(u8 *)src - *(u8 *)dst;
        if(ret != 0)  break;
        src++, dst++, cnt++;
        if(cnt >= num)  return 0;
    }
    if(ret < 0)  ret = -1;
    else if(ret > 0)  ret = 1;
    return ret;
}
/**/

/**
@功能: 将字符串分解为一组小字符串
@参数: src, 需要被分解的字符串
       delim, 分隔符字符串
       pstr, 保存分割后的字符串
@返回: 分割后的小字符串数量
@备注: 
*/
u32 StrSplit(char *src, const char *delim, const char **pstr)
{
    char *p;
    u32 count = 0;
    
    if(src == NULL || strlen(src) || delim == NULL)  return 0;
    
    p = strtok((char *)src, delim);
    while(p)
    {
        *pstr++ = p;
        count++;
        p = strtok(NULL, delim);
    }
    
    return count;
}
/**/

/**
@功能: 字符串复制
@参数: dst, 目的地址指针
       src, 源地址指针
       num, 目的空间允许复制的长度，单位：char
@返回: 分割后的小字符串数量
@备注: 
*/
void Strcpy(char *dst, const char *src, u32 num)
{
    if(num >= strlen(src))
    {
        strcpy(dst, src);
    }
    else
    {
        memcpy(dst, src, num);
        dst[num - 1] = '\0';
    }
}
/**/

/**
@功能: 将一组字符串转换成10进制整数
@参数: pstr, 指定的字符串
       num, 指定的转换长度
@返回: 0, 执行结果或没有执行有效的转换
       其它, 转换后的整数
@备注：返回结果若为0，需要用户自行判断是否有效结果
@记录: 2021-01-20，Bug，修复了++count在一个区间判断完未清0问题
       2021-03-11, Bug, 修复了count计数错误
*/
s32 Atoi(const char *pstr, u8 num)
{
    s32 c;
    s32 total;
    s32 sign;
    u8 count = 0;
    
    while(isspace((s32)(u8)*pstr))  
    {
        ++pstr; 
        if(++count > num)  return 0;
    }
    
    count = 0;                                    //20210120
    c = (s32)(u8)*pstr++;  
//    count++;                                    //20210311屏蔽
    sign = c;
    if(c == '-' || c == '+')  
    {
        c = (s32)(u8)*pstr++;
        if(++count > num)  return 0;
    }
    
    total = 0;
//    count = 0;                                  //20210120添加，20210311屏蔽
    
    while(isdigit(c))
    {
        total = 10 * total + (c - '0');
        c = (s32)(u8)*pstr++;
        if(++count >= num)  break;                //20210311修改，指定的转换长度完成，返回当前值而不是0
    }
    
    if(sign == '-')  return -total;
    else  return total;
}
/**/

/**
@功能: 将一组字符串转换成16进制整数
@参数: pstr, 指定的字符串
       num, 指定的转换长度
@返回: 0, 没有执行有效的转换
       其它, 转换后的整数
@备注: 返回结果为0，需要用户自行判断结果是否有效
*/
u32 Atoh(const char *pstr, u8 num)
{
    u32 c;
    u32 total;
    u8 count = 0;
    u8 *p;
    
    while(isspace((s32)(u8)*pstr))
    {
        ++pstr; 
        if(++count > num)  return 0;
    }
    
    count = 0;
    p = (u8 *)pstr;
    
    if(p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
    {
        pstr += 2;
        count += 2;
        if(count >= num)  return 0;
    }
    
    c = (s32)(u8)*pstr++;
    total = 0;
    
    while(isxdigit(c))
    {
        total <<= 4;                              //从字符串的最高位开始转换
        
        if('0' <= c && c <= '9')
        {
            total += c - '0';
        }
        else if('a' <= c && c <= 'f')
        {
            total += c - 'a' + 10;
        }
        else if('A' <= c && c <= 'F')
        {
            total += c - 'A' + 10;
        }
        else
        {
            total >>= 4;                          //无效字符
            break;
        }
        
        c = (s32)(u8)*pstr++;
        if(++count >= num)  break;                //指定的转换长度完成
    }
    
    return total;
}
/**/
