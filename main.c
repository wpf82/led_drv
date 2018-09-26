#include "stm8s.h"
#include "stm8s_gpio.h"
#include "config.h"
#include "modbus.h"
#include "platform.h"
#include "isl1208.h"  
#include "i2c_ee.h"  

//uint8_t doorstates1 = 0;        // 0 开门 1 关门
//uint8_t doorstates2 = 0;
//uint8_t doorerror1 = 0;         //1报警
//uint8_t doorerror2 = 0;
//uint8_t doorucerror1 = 0;         //1报警
//uint8_t doorucerror2= 0;

uint16_t testtimecount = 0;
uint16_t releasetimecount = 0;

uint8_t keypassnum1 = 0;        // 0 
uint8_t keypassnum2 = 0;        // 0 
uint8_t keypassnum3 = 0;        // 0 
uint8_t keypassflag = 0;        // 0 
uint16_t protectflag = 0;

uint16_t timecout = 0;
uint16_t ledstate = 0;
uint16_t minunte = 0;
uint16_t warnstate = 0;
uint16_t bepstate = 0;

void door_ir_init(void);
void sys_io_init(void);
void mainpro();
void sysinit(void);
void doorstateschange(void);
void keyfunc(void);
void keyexfunc(void);
void ledset(uint8_t lednum,uint8_t ledstates);
void testtimecountp(uint16_t num);

extern uint8_t modetype2;
extern void flashinit(void);
extern void MB_polling_master (void);
extern void modbusinit(void);
int main( void )
{
  uint32_t clk;

  //CLK_ClockSwitchCmd(ENABLE); //切换使能
 // CLK_ClockSwitchConfig (CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI,DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE );
  //CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV8);
  // 默认2m
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
  sysinit();  
  flashinit();
  sys_io_init();
     
  mainpro();
  return 0;
}

void sysinit(void)
{
  MY_TIME mytime;
  
  
  enableInterrupts();

//  TIM1_TimeBaseInit( 1, TIM1_COUNTERMODE_UP, 50, 0 );  // 1000us
  
  TIM1_TimeBaseInit( 0x0f, TIM1_COUNTERMODE_UP, 100, 0 );  // 1000us
  TIM1_OC4Init( TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_ENABLE,
//启用互补输出比较1，脉宽50%，输出比较极性高电平，
 45, TIM1_OCPOLARITY_LOW,
//互补输出比较极性高电平，输出比较空闲状态复位，互补输出比较空闲状态复//位）
 TIM1_OCIDLESTATE_RESET );

  TIM1_CCxCmd(TIM1_CHANNEL_4, ENABLE);  
  TIM1_OC4PreloadConfig(ENABLE);  
 
  TIM1_Cmd(ENABLE);
  TIM1_CtrlPWMOutputs(ENABLE);
  
  TIM2_TimeBaseInit( TIM2_PRESCALER_16, 10000 );                // 100ms
  TIM2_ITConfig( TIM2_IT_UPDATE , ENABLE);
  
  TIM2_SetCounter(0x0000); 
  TIM2_Cmd(ENABLE);

  
}

void sys_io_init(void)
{
  MY_TIME mytime;
  
  uint16_t i=0;
  uint16_t q=0;
  uint16_t c=0;
 // io out 
  GPIO_Init(LEDG1_PORT,LEDG1_PIN,GPIO_MODE_OUT_PP_HIGH_SLOW) ;
  GPIO_Init(LEDG2_PORT,LEDG2_PIN,GPIO_MODE_OUT_PP_HIGH_SLOW) ;
  GPIO_Init(LEDG3_PORT,LEDG3_PIN,GPIO_MODE_OUT_PP_HIGH_SLOW) ;
  GPIO_Init(LEDG4_PORT,LEDG4_PIN,GPIO_MODE_OUT_PP_HIGH_SLOW) ;
  GPIO_Init(LEDG5_PORT,LEDG5_PIN,GPIO_MODE_OUT_PP_HIGH_SLOW) ;
  GPIO_Init(LEDR1_PORT,LEDR1_PIN,GPIO_MODE_OUT_PP_HIGH_SLOW) ;
  GPIO_Init(LEDR2_PORT,LEDR2_PIN,GPIO_MODE_OUT_PP_HIGH_SLOW) ;
  GPIO_Init(LEDR3_PORT,LEDR3_PIN,GPIO_MODE_OUT_PP_HIGH_SLOW) ;

  
 // GPIO_Init(BUZZER_PORT,BUZZER_PIN,GPIO_MODE_OUT_PP_HIGH_SLOW) ; //modified by wpf  
  
  
  GPIO_WriteLow(LEDG1_PORT,LEDG1_PIN) ;
  GPIO_WriteLow(LEDG2_PORT,LEDG2_PIN) ;
  GPIO_WriteLow(LEDG3_PORT,LEDG3_PIN) ;
  GPIO_WriteLow(LEDG4_PORT,LEDG4_PIN) ;
  GPIO_WriteLow(LEDG5_PORT,LEDG5_PIN) ;
  GPIO_WriteLow(LEDR1_PORT,LEDR1_PIN) ;
  GPIO_WriteLow(LEDR2_PORT,LEDR2_PIN) ;
  GPIO_WriteLow(LEDR3_PORT,LEDR3_PIN) ;
  
 // GPIO_WriteHigh(BUZZER_PORT,BUZZER_PIN) ;
  for(i=0;i<2000;i++)
  {
    for(q=0;q<200;q++)
    {
      c++;
    }
  }
// GPIO_WriteLow(BUZZER_PORT,BUZZER_PIN) ;     //modified by wpf 
  
 // GPIO_WriteHigh(LEDG1_PORT,LEDG1_PIN) ;
 // GPIO_WriteHigh(LEDG2_PORT,LEDG2_PIN) ;
 // GPIO_WriteHigh(LEDG3_PORT,LEDG3_PIN) ;
 // GPIO_WriteHigh(LEDG4_PORT,LEDG4_PIN) ;
 // GPIO_WriteHigh(LEDG5_PORT,LEDG5_PIN) ;
  GPIO_WriteHigh(LEDR1_PORT,LEDR1_PIN) ;
  GPIO_WriteHigh(LEDR2_PORT,LEDR2_PIN) ;
  GPIO_WriteHigh(LEDR3_PORT,LEDR3_PIN) ;
  
  

  GPIO_Init(KEY1_PORT,KEY1_PIN,GPIO_MODE_IN_FL_NO_IT) ;
  GPIO_Init(KEY2_PORT,KEY2_PIN,GPIO_MODE_IN_FL_NO_IT) ;
  GPIO_Init(KEY3_PORT,KEY3_PIN,GPIO_MODE_IN_FL_NO_IT) ;
  

      for(i=1;i<(ledstate+1);i++)
      {
        ledset(i,0);
      }
  if(ledstate==5)
    {
      ledset(6,1);
      ledset(7,1);
      ledset(8,1);
      warnstate = 1;
    }
 //GetTime(&mytime);

  
}


void testtimecountp(uint16_t num)
{
 
}      
 
      
void ledset(uint8_t lednum,uint8_t ledstates)
{
  if(lednum==1)
  {
    if(ledstates==LEDOFF)
       GPIO_WriteHigh(LEDG1_PORT,LEDG1_PIN) ;
    else
       GPIO_WriteLow(LEDG1_PORT,LEDG1_PIN) ;
  }
  else if(lednum==2)
  {
    if(ledstates==LEDOFF)
       GPIO_WriteHigh(LEDG2_PORT,LEDG2_PIN) ;
    else
       GPIO_WriteLow(LEDG2_PORT,LEDG2_PIN) ;
  }
  else if(lednum==3)
  {
    if(ledstates==LEDOFF)
       GPIO_WriteHigh(LEDG3_PORT,LEDG3_PIN) ;
    else
       GPIO_WriteLow(LEDG3_PORT,LEDG3_PIN) ;
  }
   else if(lednum==4)
  {
    if(ledstates==LEDOFF)
       GPIO_WriteHigh(LEDG4_PORT,LEDG4_PIN) ;
    else
       GPIO_WriteLow(LEDG4_PORT,LEDG4_PIN) ;
  }
   else if(lednum==5)
  {
    if(ledstates==LEDOFF)
       GPIO_WriteHigh(LEDG5_PORT,LEDG5_PIN) ;
    else
       GPIO_WriteLow(LEDG5_PORT,LEDG5_PIN) ;
  }
   else if(lednum==6)
  {
    if(ledstates==LEDOFF)
       GPIO_WriteHigh(LEDR1_PORT,LEDR1_PIN) ;
    else
       GPIO_WriteLow(LEDR1_PORT,LEDR1_PIN) ;
  }
   else if(lednum==7)
  {
    if(ledstates==LEDOFF)
       GPIO_WriteHigh(LEDR2_PORT,LEDR2_PIN) ;
    else
       GPIO_WriteLow(LEDR2_PORT,LEDR2_PIN) ;
  }
   else if(lednum==8)
  {
    if(ledstates==LEDOFF)
       GPIO_WriteHigh(LEDR3_PORT,LEDR3_PIN) ;
    else
       GPIO_WriteLow(LEDR3_PORT,LEDR3_PIN) ;
  }
}

void keyfunc(void)
{
  if((GPIO_ReadInputData(KEY1_PORT)&KEY1_PIN )== 0)
  {
    keypassnum1++;
    if(keypassnum1==5)
    {
     keypassnum1 = 0;

      //TIM1_SetCompare4(15);
      TIM1_SetCompare4(60);               //modified by wpf      
    }
  }
  else
  {
    keypassnum1 = 0;
  }
  if((GPIO_ReadInputData(KEY2_PORT)&KEY2_PIN )== 0)
  {
    keypassnum2++;
    if(keypassnum2==5)
    {
      keypassnum2 = 0;

//     TIM1_SetCompare4(25);
       TIM1_SetCompare4(45);
    }
  }
  else
  {
    keypassnum2 = 0;
  }
  if((GPIO_ReadInputData(KEY3_PORT)&KEY3_PIN )== 0)
  {
    keypassnum3++;
    if(keypassnum3==5)
    {
      keypassnum3 = 0;
 //     TIM1_SetCompare4(40);
      TIM1_SetCompare4(20);
    }
  }
  else
  {
    
  }
}



void enterhaltmode(void)
{
  
  
}
void exithaltmode(void)
{
  
  
}
void mainpro(void)
{
  while(1)
  {
    keyfunc();
    if(timecout>(180*4*30))  // 180 ==15s
    {
      TIM2_SetCounter(0x0000); 
      timecout = 0;
      ledstate++;
      if(ledstate<6)
      {
        ledset(ledstate,0);
        if(ledstate==5)
        {
          ledset(6,1);
          ledset(7,1);
          ledset(8,1);
          warnstate = 1;
        }
      }
      else
      {
        ledset(6,0);
        ledset(7,0);
        ledset(8,0);
        warnstate = 0;
        GPIO_WriteHigh(BUZZER_PORT,BUZZER_PIN) ;
      }
      
    }
    if(warnstate == 1)
    {
      if((timecout%(180*2))==0)           //间隔30秒
      {
        GPIO_WriteHigh(BUZZER_PORT,BUZZER_PIN) ;
      }
      else if((timecout%(180*2))>12)     //1s报警
      {
        GPIO_WriteLow(BUZZER_PORT,BUZZER_PIN) ;
      }
    }
    if(timecout%(180*4)==0)
    {
      flashwrite(0,timecout>>8);
      flashwrite(1,timecout&0xff);
      flashwrite(2,ledstate);
    }
  
  }
  
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif