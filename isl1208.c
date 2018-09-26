//ISL128.C  
#include "isl1208.h"  
#include "i2c_ee.h"  
  
const u8 *COMPILED_DATE=__DATE__;//??????  
const u8 *COMPILED_TIME=__TIME__;//??????  
const u8 Month_Tab[12][3]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};   
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //??????   
extern u16 EEPROM_ADDRESS; 
MY_TIME sMyTime={0x13,0x04,0x13,0x14,0x10,0x05,0x03};  
static int IntToBCD(int i);//????BCD  
static int BCDToInt(int bcd); //BCD????  
static u8 RTC_Get_Week(u16 year,u8 month,u8 day);  
  
int IntToBCD(int i) //????BCD  
{  
    return (((i / 10) << 4) + ((i % 10) & 0x0f));  
}  
  
int BCDToInt(int bcd) //BCD????  
{  
    return (0xff & (bcd >> 4)) * 10 + (0xf & bcd);  
}  
  
//??????????????????  
//??:s1,s2?????????;len,????  
//???:1,??;0,???  
u8 my_strcmp(u8*s1,u8*s2,u8 len)  
{  
    u8 i;  
    for(i=0;i<len;i++)if((*s1++)!=*s2++)return 0;  
    return 1;        
}  
  
void ISL128Init()  
{  
    u8 SR_REG_DATA=0x91;  
    u8 INT_REG_DATA=0xca;  
    EEPROM_ADDRESS=0xDE;  
    I2C_EE_ByteWrite(&SR_REG_DATA, 0x07);  
    I2C_EE_ByteWrite(&INT_REG_DATA, 0x08);  
}  
void SetTime(MY_TIME _mytime)  
{  
		
    EEPROM_ADDRESS=0xDE;  
	
		_mytime._sec=IntToBCD(_mytime._sec);  
    _mytime._min=IntToBCD(_mytime._min);  
    _mytime._hour=IntToBCD(_mytime._hour)|0x80; // 
    _mytime._day=IntToBCD(_mytime._day);  
    _mytime._month=IntToBCD(_mytime._month);  
    _mytime._week=IntToBCD(_mytime._week);  
		_mytime._year=IntToBCD(_mytime._year); 
	
    I2C_EE_ByteWrite(&_mytime._sec,  0x00);  
    I2C_EE_ByteWrite(&_mytime._min,  0x01);  
    I2C_EE_ByteWrite(&_mytime._hour, 0x02);  
    I2C_EE_ByteWrite(&_mytime._day,  0x03);  
    I2C_EE_ByteWrite(&_mytime._month,0x04);  
    I2C_EE_ByteWrite(&_mytime._year, 0x05);  
    I2C_EE_ByteWrite(&_mytime._week, 0x06);  
}  
void GetTime(MY_TIME *_mytime)  
{  
    EEPROM_ADDRESS=0xDF;  
	
    I2C_EE_BufferRead(&(_mytime->_sec),  0x00,1);  
    I2C_EE_BufferRead(&_mytime->_min,  0x01,1);  
    I2C_EE_BufferRead(&_mytime->_hour, 0x02,1);  
    I2C_EE_BufferRead(&_mytime->_day,  0x03,1);  
    I2C_EE_BufferRead(&_mytime->_month,0x04,1);  
    I2C_EE_BufferRead(&_mytime->_year, 0x05,1);  
    I2C_EE_BufferRead(&_mytime->_week, 0x06,1);  
      
    _mytime->_sec=BCDToInt(_mytime->_sec);  
    _mytime->_min=BCDToInt(_mytime->_min);  
    _mytime->_hour=BCDToInt(_mytime->_hour&0x7f);  
    _mytime->_day=BCDToInt(_mytime->_day);  
    _mytime->_month=BCDToInt(_mytime->_month);  
    _mytime->_year=BCDToInt(_mytime->_year);  
    _mytime->_week=BCDToInt(_mytime->_week);  
}  
  
void Auto_Time_Set()  
{  
    u8 temp[3];  
    u8 i;  
    u8 mon,date,week;  
    u16 year;  
    u8 sec,min,hour;  
    for(i=0;i<3;i++)temp[i]=COMPILED_DATE[i];     
    for(i=0;i<12;i++)if(my_strcmp((u8*)Month_Tab[i],temp,3))break;     
    mon=i+1;//????  
    if(COMPILED_DATE[4]==' ')date=COMPILED_DATE[5]-'0';   
    else date=10*(COMPILED_DATE[4]-'0')+COMPILED_DATE[5]-'0';    
    year=10*(COMPILED_DATE[9]-'0')+COMPILED_DATE[10]-'0';        
    hour=10*(COMPILED_TIME[0]-'0')+COMPILED_TIME[1]-'0';    
    min=10*(COMPILED_TIME[3]-'0')+COMPILED_TIME[4]-'0';    
    sec=10*(COMPILED_TIME[6]-'0')+COMPILED_TIME[7]-'0';    
    week=RTC_Get_Week(year+2000,mon,date);  
    sMyTime._day=IntToBCD(date);  
    sMyTime._hour=IntToBCD(hour);  
    sMyTime._min=IntToBCD(min);  
    sMyTime._month=IntToBCD(mon);  
    sMyTime._sec=IntToBCD(sec);  
    sMyTime._year=IntToBCD(year);  
    sMyTime._week=IntToBCD(week);  
	//	ISL128Init();
    SetTime(sMyTime);  
}   
  
  
u8 RTC_Get_Week(u16 year,u8 month,u8 day)  
{     
    u16 temp2;  
    u8 yearH,yearL;  
      
    yearH=year/100; yearL=year%100;   
    // ???21??,????100    
    if (yearH>19)yearL+=100;  
    // ???????1900????    
    temp2=yearL+yearL/4;  
    temp2=temp2%7;   
    temp2=temp2+day+table_week[month-1];  
    if (yearL%4==0&&month<3)temp2--;  
    return(temp2%7);  
} 