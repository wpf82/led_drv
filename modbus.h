
#ifndef __MODBUS_H
#define __MODBUS_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"


#define MOTOR_PORT       GPIOB
#define MOTOR_PIN        GPIO_PIN_6





#define MB_DEVICE_ADDR          1
#define _35T_TIMEOUT            1
#define MB_TIMEOUT_NUMMAX       16
#define MB_GETDATA_FREE		1
#define MB_GETDATA_GETTING      2
#define MB_GETDATA_FINSH        3
#define MB_SENDING_DATA		4

#define MB_SLAVEID_ADDR         32769

#define BROADCASTADDR1         253
#define BROADCASTADDR2         200

#define MB_SETSN_FUNC	       0x44
#define EXTERSION_RESERVE_LEN   0x20

#define SN_MODETYPE_ADDR        9


#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
