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
  // ע��˳�� FLASH_DUKR = 0xAEΪKey2 ��FLASH_DUKR = 0x56;ΪKey1
   FLASH_Unlock( FLASH_MEMTYPE_DATA);
   //����ڶ�����Կ��ȷ��FLASH_IAPSR_DUL=1��
   //ֱ��FLASH_IAPSR_DUL��Ӳ����λ��������whileѭ��
  while( (FLASH->IAPSR & FLASH_IAPSR_DUL) == 0);//�ȴ�DATA EEPROM������
}

void flashwrite(uint8_t Address,uint8_t data)
{
  FLASH_EraseByte(Block_0+Address);  // ����ĳ��ַ��һ���ֽ�
  FLASH_ProgramByte(Block_0+Address,data); //д��һ���ֽ�
  while((FLASH->IAPSR  & 0x04) != 0x00);   //ֱ��EOP=1��EEPROM��̽���
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