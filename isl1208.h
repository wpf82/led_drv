//ISL1208.h  
#ifndef _ISL1208_H_  
#define _ISL1208_H_  

#include "stm8s.h"

  
typedef struct My_Time  
{  
    u8 _year;  
    u8 _month;  
    u8 _day;  
    u8 _hour;  
    u8 _min;  
    u8 _sec;  
    u8 _week;  
} MY_TIME,*pMY_TIME;  
  
extern MY_TIME sMyTime;  
void ISL128Init(void);  
void SetTime(MY_TIME _mytime);  
void GetTime(MY_TIME *_mytime);  
void Auto_Time_Set(void);  
#endif //_ISL1208_H_