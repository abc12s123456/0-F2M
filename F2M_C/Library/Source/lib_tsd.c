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
 
#include "lib_tsd.h"



void TSD_Init(TSD_Type *tsd, float max, float min, float range, u16 times)
{
    tsd->Max = max;
    tsd->Min = min;
    tsd->Range = range;
    tsd->Times = times;
    tsd->Increase_Count = 0;
    tsd->Reduce_Count = 0;
    tsd->Sum = 0.0f;
    tsd->Value = 0.0f;
}

float TSD_GetValue(TSD_Type *tsd, float value)
{
    if(value > tsd->Max ||value < tsd->Min)
    {
        return tsd->Value;
    }
    
    if(value > tsd->Value + tsd->Range)
    {
        tsd->Increase_Count++;
        if(tsd->Increase_Count >= tsd->Times)
        {
            tsd->Value = value;
            tsd->Increase_Count = 0;
        }
        tsd->Reduce_Count = 0;
    }
    else if(value < tsd->Value - tsd->Range)
    {
        tsd->Reduce_Count++;
        if(tsd->Reduce_Count >= tsd->Times)
        {
            tsd->Value = value;
            tsd->Reduce_Count = 0;
        }
        tsd->Increase_Count = 0;
    }
    else
    {
        tsd->Increase_Count = 0;
        tsd->Reduce_Count = 0;
    }
    
    return tsd->Value;
}

float TSD_GetAverage(TSD_Type *tsd, float value)
{
    if(value > tsd->Max || value < tsd->Min)
    {
        return tsd->Value;
    }
    
    if(value > tsd->Value + tsd->Range)
    {
        tsd->Increase_Count++;
        tsd->Sum += value;
        if(tsd->Increase_Count >= tsd->Times)
        {
            tsd->Value = tsd->Sum / tsd->Times;
            tsd->Increase_Count = 0;
            tsd->Sum = 0;
        }
        tsd->Reduce_Count = 0;
    }
    else if(value < tsd->Value - tsd->Range)
    {
        tsd->Reduce_Count++;
        tsd->Sum += value;
        if(tsd->Reduce_Count >= tsd->Times)
        {
            tsd->Value = tsd->Sum / tsd->Times;
            tsd->Increase_Count = 0;
            tsd->Sum = 0;
        }
        tsd->Reduce_Count = 0;
    }
    else
    {
        tsd->Increase_Count = 0;
        tsd->Reduce_Count = 0;
        tsd->Sum = 0;
    }
    
    return tsd->Value;
}

