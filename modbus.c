#include "modbus.h"
#include "config.h"
#include "platform.h"


uint8_t MB_get_data_status_master = MB_GETDATA_FREE;
uint8_t MB_rx_buff_master[24];  //for get data form master rx
uint8_t MB_tx_buff_master[64];  //modbus 数据最长125*2 字节
uint8_t MB_rx_count_master;
uint8_t MB_timeout_35T_master;
uint8_t MB_send_data_master;
uint8_t MB_tx_data_offset_master = 3;
uint8_t MBus_device_addr = SLAVEID;
uint8_t MBus_timeout_num = 0;
uint16_t Led_timeout_num = 0;
uint8_t modetype1 = MODETYPE1; // 模式类型 高位
uint8_t modetype2 = MODETYPE2; // 模式类型 低位



#ifdef FORKUNLUN
uint8_t modemonitortime = 20;        // 检测报警时间
uint8_t modereleasetime = 10;        // 门磁释放时间
#else
uint8_t modemonitortime = 3;        // 检测报警时间
uint8_t modereleasetime = 2;        // 门磁释放时间
#endif
uint16_t modestates; // 模组 监控状态
extern uint8_t keypassflag ;        // 0 开门 1 关门

extern uint16_t testtimecount ;
extern uint16_t releasetimecount ;

extern uint8_t keypassflag ;        // 0 

union
{
  struct
  {
  uint8_t s1:1;
  uint8_t s2:1;
  uint8_t s3:1;
  uint8_t s4:1;
  uint8_t s5:1;
  uint8_t s6:1;
  uint8_t s7:1;
  uint8_t s8:1;
  }x;
  uint8_t state;
}states2;

union
{
  struct
  {
  uint8_t s1:1;
  uint8_t s2:1;
  uint8_t s3:1;
  uint8_t s4:1;
  uint8_t s5:1;
  uint8_t s6:1;
  uint8_t s7:1;
  uint8_t s8:1;
  }x;
  uint8_t state;
}states1;


void timer1_init(void);
void Enable_Tim1(void);
void Disable_Tim1(void);

void MB_send_data_master_finish();
void MB_decoder_reg_form_master(uint16_t regaddr, uint8_t reglen);
void MB_SendData(uint8_t *SendBuffer,uint8_t BufferSize);
void MB_decoder_write_reg_from_master(uint16_t regaddr,uint16_t writedata);

uint8_t MB_SNFUNC(void);
void MB_SNFUNC_response(void);

extern uint8_t doorstates1;
extern uint8_t doorstates2;
extern uint8_t doorerror1;
extern uint8_t doorerror2;
extern uint8_t doorucerror1;
extern uint8_t doorucerror2;
extern uint8_t flashread(uint8_t Address);
extern void flashwrite(uint8_t Address,uint8_t data);
extern void ledset(uint8_t lednum,uint8_t ledstates);

// CRC 高位字节值表    
const uint8_t auchCRCHi[] = { 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
} ; 
// CRC低位字节值表
const uint8_t auchCRCLo[] = { 
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
} ;



/*****************************************************************************************************************************************************
** 函数名称: GetCRC16()
** 功能描述: 计算CRC16校验
** 输　入:   *uchMsg  -- 需要校验的字符串  
**           usDataLen-- 数据长度 
*****************************************************************************************************************************************************/
uint16_t GetCRC16(uint8_t *puchMsg, uint16_t usDataLen) 
{ 
    uint16_t uchCRCHi = 0xFF ;                            //高CRC字节初始化
    uint16_t uchCRCLo = 0xFF ;                            //低CRC 字节初始化 
    uint32_t uIndex ;                                     //CRC循环中的索引
    while (usDataLen--)                                   //传输消息缓冲区
    { 
        uIndex = uchCRCHi ^ *puchMsg++ ;                    //计算CRC 
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex]; 
        uchCRCLo = auchCRCLo[uIndex] ; 
    } 
    return (uchCRCHi << 8 | uchCRCLo); 
}


void timer1_init(void)
{
    TIM1_DeInit();
    TIM1_TimeBaseInit(2000, TIM1_COUNTERMODE_UP,1,0);// 1ms
    TIM1_ARRPreloadConfig(ENABLE);
    TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
    TIM1_Cmd(ENABLE);

}

void Enable_Tim1(void)
{
 //   TIM1_Cmd(ENABLE);
    MBus_timeout_num = 1;
}

void Disable_Tim1(void)
{
    MBus_timeout_num = 0;
//    TIM1_Cmd(DISABLE);
}

void Uart1_Initial(void)
{     
    UART1_DeInit();
    UART1_Cmd(ENABLE);
    UART1_Init((u32)2400, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, \
               UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
    UART1_ITConfig(UART1_IT_TXE, DISABLE);//关串口开始发送中断  
    UART1_ITConfig(UART1_IT_TC, DISABLE);
    UART1_ITConfig(UART1_IT_RXNE, ENABLE);
    
    enableInterrupts();
}

void modbusinit()
{
    timer1_init();
    Uart1_Initial();
    GPIO_WriteHigh(MB_485_DE_PORT,MB_485_DE_PIN) ;
}

uint8_t MB_ReceiveData(void)
{
    return UART1_ReceiveData8();
}
void MB_SendData(uint8_t *SendBuffer,uint8_t BufferSize)
{	 
    uint32_t vCounter = 0;
    GPIO_WriteLow(MB_485_DE_PORT,MB_485_DE_PIN) ;

    while(vCounter < BufferSize)
    {
        UART1_SendData8(SendBuffer[vCounter++]);
        while(!UART1_GetFlagStatus(UART1_FLAG_TXE)); 
    }
}
void MB_get_data_from_master(void)
{
    if(MB_get_data_status_master == MB_GETDATA_FREE)  //接收处于空闲状态 准备接收第一桢数据
    {

        MB_rx_buff_master[MB_rx_count_master] = MB_ReceiveData();
        
        if((MB_rx_buff_master[0] == MBus_device_addr)||(MB_rx_buff_master[0] == BROADCASTADDR1)||(MB_rx_buff_master[0] == BROADCASTADDR2))  //判断设备地址 
        {
            MB_get_data_status_master = MB_GETDATA_GETTING;
            MB_rx_count_master++;
            Enable_Tim1();  //开启tmr 
            MB_timeout_35T_master = 0;
        }
        else  //不是设备地址 
        {
            Disable_Tim1();
            MB_rx_count_master = 0;
            MB_timeout_35T_master = 0;
        }    
    }
    else if(MB_get_data_status_master == MB_GETDATA_GETTING)
    {
        Disable_Tim1();
        MB_rx_buff_master[MB_rx_count_master] = MB_ReceiveData();
        MB_rx_count_master++;
        MB_timeout_35T_master = 0;
        Enable_Tim1();  //reset timer 
    }    
}
/*******************************************************************************

  poll从主机得到的数据 




********************************************************************************/	

void MB_polling_master (void)
{
    uint16_t i,q,c=0;
    uint16_t CRCdata;
    uint16_t MB_regaddr;	
    uint16_t MB_read_lens;
    uint16_t WriteData;

    if(MB_timeout_35T_master == _35T_TIMEOUT)
    {
        Disable_Tim1();  
        MB_timeout_35T_master = 0;
        UART1_ITConfig(UART1_IT_RXNE, DISABLE);
        MB_get_data_status_master = MB_GETDATA_FINSH;
    }

    if(MB_get_data_status_master == MB_GETDATA_FINSH)
    {

        if(MB_rx_buff_master[1] == MB_SETSN_FUNC)
        {
            CRCdata = GetCRC16(MB_rx_buff_master,MB_rx_count_master-2 );
            if((CRCdata>>8 != MB_rx_buff_master[MB_rx_count_master-2] ) || ((CRCdata&0xff) != MB_rx_buff_master[MB_rx_count_master-1] ))
                goto  error_POLL;
            
            if(MB_SNFUNC()==0)
              goto  error_POLL;
        }
        else
        {
            if(MB_rx_count_master<6)
              goto  error_POLL;
            CRCdata = GetCRC16(MB_rx_buff_master,MB_rx_count_master-2 );
            if((CRCdata>>8 != MB_rx_buff_master[MB_rx_count_master-2] ) || ((CRCdata&0xff) != MB_rx_buff_master[MB_rx_count_master-1] ))
                goto  error_POLL;
            
            ledset(LEDCOM_NUM,LEDON);
            Led_timeout_num = 1;
            
            MB_regaddr = (uint16_t)(MB_rx_buff_master[2] <<8 | MB_rx_buff_master[3]);
            
            if(MB_rx_buff_master[1] == 0x03)  //read 
            {
                MB_read_lens = (uint16_t)(MB_rx_buff_master[4]<<8)| MB_rx_buff_master[5];  //读取read 长度  主机发送的word长度 从机回btye长度 MB_read_lens*2 
                MB_decoder_reg_form_master(MB_regaddr,MB_read_lens);
            
            }
            else if(MB_rx_buff_master[1] == 0x06)  //write 
            {
            
                WriteData = (uint16_t)(MB_rx_buff_master[4]<<8) | MB_rx_buff_master[5];
                MB_decoder_write_reg_from_master(MB_regaddr,WriteData);
            
            }
            else  //cmd error 
            {
                goto  error_POLL;
                    
            }
        }
    }
    if(MB_send_data_master == 1) //数据准备完成 准备发送
    {
        MB_SendData(MB_tx_buff_master,MB_tx_data_offset_master);
        for(q=0;q<2000;q++)
        {
        c++;
        }
        memset(MB_rx_buff_master,0,24);
        MB_send_data_master_finish();	
        
    }
    
    return;

    error_POLL:
    MB_send_data_master = 0;
    MB_get_data_status_master = MB_GETDATA_FREE;
    MB_rx_count_master = 0;
    MB_tx_data_offset_master = 3;
            
    UART1_ITConfig(UART1_IT_RXNE, ENABLE);


    return; 
}
void MB_send_data_master_finish()        
{
    MB_send_data_master = 0;
    MB_get_data_status_master = MB_GETDATA_FREE;
    MB_rx_count_master = 0;
    MB_tx_data_offset_master = 3;
    GPIO_WriteHigh(MB_485_DE_PORT,MB_485_DE_PIN) ;
    UART1_ITConfig(UART1_IT_RXNE, ENABLE);
    
}

void MB_decoder_reg_form_master(uint16_t regaddr, uint8_t reglen)
{
	
    uint16_t crcdata;
    uint16_t i;
    uint16_t regaddr_data ;
    uint16_t count;

    MB_tx_buff_master[2] = reglen*2;  //长度回btye 


    if(regaddr == MB_SLAVEID_ADDR)  //地址修改地址 
    {
        MB_tx_buff_master[MB_tx_data_offset_master++] = (uint8_t)(MBus_device_addr >>8);
        MB_tx_buff_master[MB_tx_data_offset_master++] = (uint8_t)(MBus_device_addr &0xff);		
        goto Send_data;				
    }

    if(regaddr < EXTERSION_RESERVE_LEN)
    {
        regaddr_data = regaddr;
        if(reglen > EXTERSION_RESERVE_LEN - regaddr_data)
            count = EXTERSION_RESERVE_LEN - regaddr_data;
        else
            count = reglen;
        
        for(i =0; i< count; i++)
        {
        
            switch(regaddr)
            {
                case 0:  //test
                    MB_tx_buff_master[MB_tx_data_offset_master++] = 0xAA;
                    MB_tx_buff_master[MB_tx_data_offset_master++] = 0x55;
                break;
                
                case 1:  //factory id
                    MB_tx_buff_master[MB_tx_data_offset_master++] = flashread(1*2+0);
                    MB_tx_buff_master[MB_tx_data_offset_master++] = flashread(1*2+1);
                break;				
                
                case 2:  //mode type
                    MB_tx_buff_master[MB_tx_data_offset_master++] = modetype1;
                    MB_tx_buff_master[MB_tx_data_offset_master++] = modetype2;
                break;					
                
                case 3:  //sn1
                    MB_tx_buff_master[MB_tx_data_offset_master++] = flashread(3*2+0);
                    MB_tx_buff_master[MB_tx_data_offset_master++] = flashread(3*2+1);
                break;					
                case 4:  //sn2
                    MB_tx_buff_master[MB_tx_data_offset_master++] = flashread(4*2+0);
                    MB_tx_buff_master[MB_tx_data_offset_master++] = flashread(4*2+1);
                break;					
                
                case 5:  //sn3
                    MB_tx_buff_master[MB_tx_data_offset_master++] = flashread(5*2+0);
                    MB_tx_buff_master[MB_tx_data_offset_master++] = flashread(5*2+1);
                break;		
                
                case 6:  //version
                    MB_tx_buff_master[MB_tx_data_offset_master++] = flashread(6*2+0);
                    MB_tx_buff_master[MB_tx_data_offset_master++] = flashread(6*2+1);
                break;	

                default:
                    MB_tx_buff_master[MB_tx_data_offset_master++] = 0x00;
                    MB_tx_buff_master[MB_tx_data_offset_master++] = 0x00;
                break;
            }
            regaddr++;
            reglen--;	
        }
    }
    
    Send_data:
	MB_send_data_master = 1;
	MB_tx_buff_master[0] = MB_rx_buff_master[0];  //addr 
	MB_tx_buff_master[1] = 0x03;
	crcdata = GetCRC16(MB_tx_buff_master,MB_tx_data_offset_master );
	MB_tx_buff_master[MB_tx_data_offset_master++] = crcdata>>8;
	MB_tx_buff_master[MB_tx_data_offset_master++] = crcdata&0xff;
	
	return;
}

void MB_decoder_write_reg_from_master(uint16_t regaddr,uint16_t writedata)
{


    if(regaddr == MB_SLAVEID_ADDR)  //地址修改地址 
    {
        MBus_device_addr = writedata;
        flashwrite(0,(uint8_t)(writedata>>8));	
        flashwrite(0*2+1,(uint8_t)(writedata&0xff));	
        goto Send_data;				
    }


    if(regaddr < EXTERSION_RESERVE_LEN)
    {
        switch(regaddr)
        {
                        
            case 1:  
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                flashwrite(regaddr*2,(uint8_t)(writedata>>8));	
                flashwrite(regaddr*2+1,(uint8_t)(writedata&0xff));	
                if(regaddr==2)
                {
                  modetype1 = (uint8_t)(writedata>>8);
                  modetype2 = (uint8_t)(writedata&0xff);
                }
            break;				
        case 20:
            if(writedata==1)
            {
              GPIO_WriteLow(LED1_PORT,LED1_PIN) ;
            }
            else
            {
              GPIO_WriteHigh(LED1_PORT,LED1_PIN) ;
            }
             break;
        case 21:
            if(writedata==1)
            {
              GPIO_WriteLow(LED2_PORT,LED2_PIN) ;
            }
            else
            {
              GPIO_WriteHigh(LED2_PORT,LED2_PIN) ;
            }
             break;
         case 22:
            if(writedata==1)
            {
              GPIO_WriteLow(LED3_PORT,LED3_PIN) ;
            }
            else
            {
              GPIO_WriteHigh(LED3_PORT,LED3_PIN) ;
            }
             break;
         case 23:
            if(writedata==1)
            {
              GPIO_WriteLow(LED4_PORT,LED4_PIN) ;
            }
            else
            {
              GPIO_WriteHigh(LED4_PORT,LED4_PIN) ;
            }
             break;
          case 24:
            if(writedata==1)
            {
              GPIO_WriteLow(LED5_PORT,LED5_PIN) ;
            }
            else
            {
              GPIO_WriteHigh(LED5_PORT,LED5_PIN) ;
            }
             break;
          case 25:
            if(writedata==1)
            {
              GPIO_WriteLow(LED6_PORT,LED6_PIN) ;
            }
            else
            {
              GPIO_WriteHigh(LED6_PORT,LED6_PIN) ;
            }
             break;
            default:
            
            break;
        }		
    }
	
Send_data:
	   
    MB_send_data_master = 1;
    MB_tx_buff_master[0] = MB_rx_buff_master[0];  //addr 
    MB_tx_buff_master[1] = MB_rx_buff_master[1]; 
    MB_tx_buff_master[2] = MB_rx_buff_master[2];
    MB_tx_buff_master[3] = MB_rx_buff_master[3];	
    MB_tx_buff_master[4] = MB_rx_buff_master[4];
    MB_tx_buff_master[5] = MB_rx_buff_master[5];
    MB_tx_buff_master[6] = MB_rx_buff_master[6];
    MB_tx_buff_master[7] = MB_rx_buff_master[7];	
    MB_tx_data_offset_master = 8;
	
}


uint8_t MB_SNFUNC(void)
{
    uint8_t addrtmp;
    addrtmp = SN_MODETYPE_ADDR;
    if(MB_rx_buff_master[addrtmp++]!=modetype1)
      return 0;
    if(MB_rx_buff_master[addrtmp++]!=modetype2)
      return 0;
    if(MB_rx_buff_master[addrtmp++]!=flashread(3*2+0))
      return 0;
    if(MB_rx_buff_master[addrtmp++]!=flashread(3*2+1))
      return 0;
    if(MB_rx_buff_master[addrtmp++]!=flashread(4*2+0))
      return 0;
    if(MB_rx_buff_master[addrtmp++]!=flashread(4*2+1))
      return 0;
    if(MB_rx_buff_master[addrtmp++]!=flashread(5*2+0))
      return 0;
    if(MB_rx_buff_master[addrtmp++]!=flashread(5*2+1))
      return 0;
    
    MBus_device_addr = MB_rx_buff_master[addrtmp++];
    MBus_device_addr = MB_rx_buff_master[addrtmp++];
    //flashwrite(0,(uint8_t)(MBus_device_addr>>8));	
    flashwrite(0*2+1,(uint8_t)(MBus_device_addr&0xff));
    
    MB_SNFUNC_response();
    
    return 1;
}
void MB_SNFUNC_response(void)
{
    uint16_t CRCdata;	

    MB_tx_buff_master[0]=MB_rx_buff_master[0];//MBus_device_addr;
    MB_tx_buff_master[1]=0x44;
    MB_tx_buff_master[2]=MB_tx_buff_master[2];
    MB_tx_buff_master[3]=MB_tx_buff_master[3];
    CRCdata = GetCRC16(MB_tx_buff_master,4 );
    MB_tx_buff_master[4] = (uint8_t)(CRCdata>>8);
    MB_tx_buff_master[5] = (uint8_t)(CRCdata&0xff);
    MB_tx_data_offset_master = 6;
    //MB_SendData(MB_tx_buff_master,MB_tx_data_offset_master);

    MB_get_data_status_master = MB_SENDING_DATA;
    MB_send_data_master = 1;
}
