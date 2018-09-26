
#ifndef __PLATFORM_H
#define __PLATFORM_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

#define SLAVEID          199

#define FACTORYID1       0x00
#define FACTORYID2       0x41

#define HVERSION        0x01
#define SVERSION        0x01

#define MODETYPE1       MODETYPE_DOOR_MAIN
#define MODETYPE2       MODETYPE_DOOR_CLOSE2DOOR


#define MODETYPE_DOOR_MAIN   0x01
#define MODETYPE_DOOR_CLOSE2DOOR 0x0e
#define MODETYPE_DOOR_CLOSE1DOOR 0x0f
#define MODETYPE_DOOR_OPEN2DOOR 0x10
#define MODETYPE_DOOR_OPEN1DOOR 0x11

#define DOORRELEASETIME1  3
#define DOORRELEASETIME2  5
#define DOORMONITORTIME1  10
#define DOORMONITORTIME2  15
#define DOORMONITORTIME3  20
#define DOORMONITORTIME4  30
#endif
void flashinit(void);
uint8_t flashread(uint8_t Address);
void flashwrite(uint8_t Address,uint8_t data);
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
