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

