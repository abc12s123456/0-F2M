#include "dtube.h"

#include "fw_config.h"
#include "fw_delay.h"

#include "mm.h"


/*
@@@@A@@@@
@       @
F       B
@       @
@@@@G@@@@
@       @
E       C
@       @ @DP
@@@@D@@@@ 
*/


#ifndef DTUBE_USR_CONF
#define DTUBE_A    0x80
#define DTUBE_B    0x40
#define DTUBE_C    0x20
#define DTUBE_D    0x10
#define DTUBE_E    0x08
#define DTUBE_F    0x04
#define DTUBE_G    0x02
#define DTUBE_DP   0x01
#endif

#define a   DTUBE_A
#define b   DTUBE_B
#define c   DTUBE_C
#define d   DTUBE_D
#define e   DTUBE_E
#define f   DTUBE_F
#define g   DTUBE_G
#define dp  DTUBE_DP


__CONST_STATIC_ u8 DTube_Digit_CV_Table[] =
{
    a+b+c+d+e+f,  //0
    b+c,          //1
    a+b+d+e+g,    //2
    a+b+c+d+g,    //3
    b+c+f+g,      //4
    a+c+d+f+g,    //5
    a+c+d+e+f+g,  //6
    a+b+c,        //7
    a+b+c+d+e+f+g,//8
    a+b+c+f+g,    //9	
    0x00,
};

__CONST_STATIC_ u8 DTube_Alpha_CV_Table[26] =
{
    a+b+c+e+f+g,  //A,有些字母用数码管无法完整表示，只能取一个类似的表示码值
    c+d+e+f+g,    //b
    a+d+e+f,      //C
    b+c+d+e+g,    //d
    a+e+f+g,      //F
    a+b+c+d+f+g,  //g
    b+c+e+f+g,    //H
    f+e,          //I
    b+c+d,        //J
    a+c+e+f+g,    //k
    d+e+f,        //L
    a+b+c+d+g,    //M,用"倒E"来表示M
    c+e+g,        //n
    c+d+e+g,      //o
    a+b+e+f+g,    //P
    a+b+c+f+g,    //q
    a+e+f,        //r
    a+c+d+f+g,    //S,与数字5码值一样，使用时需注意区分
    a+d+e+f+g,    //t
    b+c+d+e+f,    //U
    b+f+g,        //v
    b+c+d+e+f+g,  //W,相当于两个v上下组合
    a+b+c+d+f+g,  //X,用一个类似“？”的码值表示
    b+c+d+f+g,    //y
    a+b+d+e+g     //Z,与数字2码值一样，使用时需注意区分
};

__CONST_STATIC_ u8 DTube_Sign_CV_Table[] =
{
    dp,                      //.
    g,                       //-
    d+g,                     //=
    0x00,
};


#undef a
#undef b
#undef c
#undef d
#undef e
#undef f
#undef g
#undef dp

#define DTUBE_CLR_NUM        0x0A

#define LUMEN_VAL_DEF        200
#define COMP_VAL_DEF         15


/*
display cached data structure
     N1    N2   ...   Nn
    [D1]  [D1]  [D1] [D1]
    [D2]  [D2]  [D2] [D2]
    ...
    [Dn]  [Dn]  [Dn] [Dn]
*/


static u8  DTube_Get_BC(u8 *buff, u8 num)
{
    u32 cnt = 0;
    u8 i;
    
    for(i = 0; i < num; i++)
    {
        if(buff[i] == DTube_Digit_CV_Table[DTUBE_CLR_NUM])  cnt++;
    }
    
    return cnt;
}

static u8   DTube_Get_BCNull(u8 *buff, u8 num){return 0;}

static void DTube_BC_Handle(DTube_Type *dev, u8 comp_num)
{
    FW_Delay_Us(dev->Lumen - comp_num * dev->Comp_Value);
}

static void DTube_BC_HandleNull(DTube_Type *dev, u8 comp_num){}


void DTube_Init(DTube_Type *dev)
{
    if(dev->buffer == NULL)
    {
        dev->buffer = MM_Malloc(dev->Number * dev->Digits_Max);
        if(dev->buffer == NULL)
        {
            
        }
    }
    
    if(dev->BC_EN == ON)
    {
        dev->Get_BC = DTube_Get_BC;
        dev->BC_Handle = DTube_BC_Handle;
    }
    else
    {
        dev->Get_BC = DTube_Get_BCNull;
        dev->BC_Handle = DTube_BC_HandleNull;
    }
    
    if(dev->Lumen == 0)  dev->Lumen = LUMEN_VAL_DEF;
    if(dev->Comp_Value == 0)  dev->Comp_Value = COMP_VAL_DEF;
}

static void DTube_Fill_Buffer(DTube_Type *dev, u8 value, u8 num, u8 digit)
{
    u8 *p = &dev->buffer[digit];
    
    if(value <= 9)
    {
        p[num] = DTube_Digit_CV_Table[value];
    }
    else if((value >= 'a' && value <= 'z') ||
            (value >= 'A' && value <= 'Z'))
    {
        p[num] = DTube_Alpha_CV_Table[value];
    }
    else if(value == '.' || value == '-' || value == '=')
    {
        p[num] = DTube_Sign_CV_Table[value];
    }
    else
    {
        p[num] = DTube_Digit_CV_Table[DTUBE_CLR_NUM];
    }
}

void DTube_Set_BC(DTube_Type *dev, u8 state)
{
    if(state)
    {
        dev->BC_EN = ON;
        dev->Get_BC = DTube_Get_BC;
    }
    else
    {
        dev->BC_EN = OFF;
        dev->Get_BC = DTube_Get_BCNull;
    }
}

void DTube_Refresh(DTube_Type *dev, u32 times)
{
    u16 cnt;
    u8 i, j;
    u8 *p;
    u16 comp_num;
    
    DTube_Digit_Off(dev);
    
    while(times--)
    {
        for(i = 0; i < dev->Digits_Max; i++)
        {
            p = &dev->buffer[i];
            
            comp_num = dev->Get_BC(p, dev->Number);
            
            dev->Port_Write(dev, p, dev->Number);
            
            dev->Digit_CTL(dev, i, Enable);
            
            /* brightness-compensation */
            dev->BC_Handle(dev, comp_num);
            
            dev->Digit_CTL(dev, i, Disable);
        }
    }
    
    DTube_Digit_Off(dev);
}

void DTube_Clear(DTube_Type *dev)
{
    u8 value = DTube_Alpha_CV_Table[DTUBE_CLR_NUM];
    memset(dev->buffer, value, dev->Digits_Max * dev->Number);
}

void DTube_Digit_Off(DTube_Type *dev)
{
    u8 i;
    
    for(i = 0; i < dev->Digits_Max; i++)
    {
        dev->Digit_CTL(dev, i, Disable);
    }
}

void DTube_Post(DTube_Type *dev)
{
    
}











