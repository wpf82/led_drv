/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���  ��i2c_ee.c
 * ����    ��i2c EEPROM(AT24C02)Ӧ�ú�����         
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ�-----------------
 *          |                 |
 *          |  PB6-I2C1_SCL		|
 *          |  PB7-I2C1_SDA   |
 *          |                 |
 *           -----------------
 * ��汾  ��ST3.0.0
 *
 * ����    ��fire  QQ: 313303034 
 * ����    ��firestm32.blog.chinaunix.net
**********************************************************************************/
#include "i2c_ee.h"
#include "stm8s.h"
#include "stm8s_gpio.h"

#define I2C_Speed              10000
#define I2C1_SLAVE_ADDRESS7    0xDE
#define I2C_PageSize           8			/* AT24C02ÿҳ��8���ֽ� */

u16 EEPROM_ADDRESS;

/*
 * ��������I2C_GPIO_Config
 * ����  ��I2C1 I/O����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void I2C_GPIO_Config(void)
{
  GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_FAST);//PB4 I2C_SCL �������ţ����ߵ�ʱ�ӽţ���Ϊ���ٿ�©���������
  GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_FAST);//PB5 I2C_SDA �������ţ����ߵ����ݽţ���Ϊ���ٿ�©�������
}

/*
 * ��������I2C_Configuration
 * ����  ��I2C ����ģʽ����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void I2C_Mode_Configu(void)
{
   I2C_DeInit();

  /* Initialize the I2C */
  I2C_Init(100000, 0xDE,I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);

  /* Configuration of interrupts only */
  I2C_ITConfig(I2C_IT_EVT, ENABLE);
  enableInterrupts();
}

/*
 * ��������I2C_EE_Init
 * ����  ��I2C ����(EEPROM)��ʼ��
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void I2C_EE_Init(void)
{

  I2C_GPIO_Config(); 
 
  I2C_Mode_Configu();

}

/*
 * ��������I2C_EE_BufferWrite
 * ����  �����������е�����д��I2C EEPROM��
 * ����  ��-pBuffer ������ָ��
 *         -WriteAddr �������ݵ�EEPROM�ĵ�ַ
 *         -NumByteToWrite Ҫд��EEPROM���ֽ���
 * ���  ����
 * ����  ����
 * ����  ���ⲿ����
 */
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % I2C_PageSize;
  count = I2C_PageSize - Addr;
  NumOfPage =  NumByteToWrite / I2C_PageSize;
  NumOfSingle = NumByteToWrite % I2C_PageSize;
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize); 
    	I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }

      if(NumOfSingle!=0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }  
}

/*
 * ��������I2C_EE_ByteWrite
 * ����  ��дһ���ֽڵ�I2C EEPROM��
 * ����  ��-pBuffer ������ָ��
 *         -WriteAddr �������ݵ�EEPROM�ĵ�ַ 
 * ���  ����
 * ����  ����
 * ����  ���ⲿ����
 */
void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr)
{
  /* Send STRAT condition */
  I2C_GenerateSTART(ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));  

  /* Send EEPROM address for write */
  I2C_Send7bitAddress( EEPROM_ADDRESS, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
      
  /* Send the EEPROM's internal address to write to */
  I2C_SendData( WriteAddr);
  
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send the byte to be written */
  I2C_SendData( *pBuffer); 
   
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STOP condition */
  I2C_GenerateSTOP( ENABLE);
}

/*
 * ��������I2C_EE_PageWrite
 * ����  ����EEPROM��һ��дѭ���п���д����ֽڣ���һ��д����ֽ���
 *         ���ܳ���EEPROMҳ�Ĵ�С��AT24C02ÿҳ��8���ֽڡ�
 * ����  ��-pBuffer ������ָ��
 *         -WriteAddr �������ݵ�EEPROM�ĵ�ַ 
 *         -NumByteToWrite Ҫд��EEPROM���ֽ���
 * ���  ����
 * ����  ����
 * ����  ���ⲿ����
 */
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
    while(I2C_GetFlagStatus( I2C_FLAG_BUSY)); // Added by Najoua 27/08/2008
    
  /* Send START condition */
  I2C_GenerateSTART( ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_MODE_SELECT)); 
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress( EEPROM_ADDRESS, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  

  /* Send the EEPROM's internal address to write to */    
  I2C_SendData( WriteAddr);  

  /* Test on EV8 and clear it */
  while(! I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* While there is data to be written */
  while(NumByteToWrite--)  
  {
    /* Send the current byte */
    I2C_SendData( *pBuffer); 

    /* Point to the next byte to be written */
    pBuffer++; 
  
    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  }

  /* Send STOP condition */
  I2C_GenerateSTOP( ENABLE);
}


/*
 * ��������I2C_EE_BufferRead
 * ����  ����EEPROM�����ȡһ�����ݡ� 
 * ����  ��-pBuffer ��Ŵ�EEPROM��ȡ�����ݵĻ�����ָ�롣
 *         -WriteAddr �������ݵ�EEPROM�ĵ�ַ�� 
 *         -NumByteToWrite Ҫ��EEPROM��ȡ���ֽ�����
 * ���  ����
 * ����  ����
 * ����  ���ⲿ����
 */
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{  
  //*((u8 *)0x4001080c) |=0x80; 
    while(I2C_GetFlagStatus( I2C_FLAG_BUSY)); // Added by Najoua 27/08/2008
    
    
  /* Send START condition */
  I2C_GenerateSTART( ENABLE);
  //*((u8 *)0x4001080c) &=~0x80;
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));

  /* Send EEPROM address for write */
  I2C_Send7bitAddress( EEPROM_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd( ENABLE);

  /* Send the EEPROM's internal address to write to */
  I2C_SendData( ReadAddr);  

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART( ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Send EEPROM address for read */
  I2C_Send7bitAddress( EEPROM_ADDRESS, I2C_Direction_Receiver);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  
  /* While there is data to be read */
  while(NumByteToRead)  
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig( DISABLE);
      
      /* Send STOP Condition */
      I2C_GenerateSTOP( ENABLE);
    }

    /* Test on EV7 and clear it */
    if(I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_RECEIVED))  
    {      
      /* Read a byte from the EEPROM */
      *pBuffer = I2C_ReceiveData();

      /* Point to the next location where the byte read will be saved */
      pBuffer++; 
      
      /* Decrement the read bytes counter */
      NumByteToRead--;        
    }   
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig( ENABLE);
}


/*
 * ��������I2C_EE_WaitEepromStandbyState
 * ����  ��Wait for EEPROM Standby state 
 * ����  ����
 * ���  ����
 * ����  ����
 * ����  �� 
 */
void I2C_EE_WaitEepromStandbyState(void)      
{
  __O u16 SR1_Tmp = 0;

  do
  {
    /* Send START condition */
    I2C_GenerateSTART( ENABLE);
    /* Read I2C1 SR1 register */
    SR1_Tmp = I2C_ReadRegister( );
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_Direction_Transmitter);
  }while(!(I2C_ReadRegister() & 0x0002));
  
  /* Clear AF flag */
  I2C_ClearFlag( I2C_FLAG_AF);
    /* STOP condition */    
    I2C_GenerateSTOP( ENABLE); // Added by Najoua 27/08/2008
}

/******************** (C) COPYRIGHT 2011 ����Ƕ��ʽ���������� ********************/
