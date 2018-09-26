#include "platform.h"
#include "stm8s.h"
#include "stm8s_gpio.h"
#include "config.h"

extern uint16_t timecout ;
extern uint16_t ledstate ;

extern uint8_t MBus_device_addr;
extern uint8_t modetype1;
extern uint8_t modetype2;
extern uint8_t modereleasetime;
extern uint8_t modemonitortime;
void EEPROM_Init();
typedef enum{
  #if defined(STM8S103) || defined(STM8S003) ||  defined(STM8S903)
  Block_0=0x4000,
  Block_1=0x4040,
  Block_2=0x4080,
  Block_3=0x40C0,
  Block_4=0x4100,
  Block_5=0x4140,
  Block_6=0x4180,
  Block_7=0x41C0,
  Block_8=0x4200,
  Block_9=0x4240
  #endif
}BlockStartAddress_TypeDef;


void EEPROM_Init()
{
  FLASH_DeInit();
  FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_TPROG);
  // 注意顺序： FLASH_DUKR = 0xAE为Key2 ，FLASH_DUKR = 0x56;为Key1
   FLASH_Unlock( FLASH_MEMTYPE_DATA);
   //如果第二个秘钥正确，FLASH_IAPSR_DUL=1；
   //直到FLASH_IAPSR_DUL由硬件置位，才跳出while循环
  while( (FLASH->IAPSR & FLASH_IAPSR_DUL) == 0);//等待DATA EEPROM区解锁
}

void flashwrite(uint8_t Address,uint8_t data)
{
  FLASH_EraseByte(Block_0+Address);  // 擦除某地址的一个字节
  FLASH_ProgramByte(Block_0+Address,data); //写入一个字节
  while((FLASH->IAPSR  & 0x04) != 0x00);   //直到EOP=1，EEPROM编程结束
}

uint8_t flashread(uint8_t Address)
{
  return  FLASH_ReadByte(Block_0+Address);
}

void flashinit(void)
{
  EEPROM_Init();
  timecout = flashread(0)<<8;
  timecout += flashread(1);
  ledstate = flashread(2);

}