
#ifndef __CONFIG_H
#define __CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"



//out

#define LEDG1_PORT       GPIOC
#define LEDG1_PIN        GPIO_PIN_7

#define LEDG2_PORT       GPIOD
#define LEDG2_PIN        GPIO_PIN_2

#define LEDG3_PORT       GPIOD
#define LEDG3_PIN        GPIO_PIN_3

#define LEDG4_PORT       GPIOD
#define LEDG4_PIN        GPIO_PIN_5

#define LEDG5_PORT       GPIOD
#define LEDG5_PIN        GPIO_PIN_7

#define LEDR1_PORT       GPIOD
#define LEDR1_PIN        GPIO_PIN_0

#define LEDR2_PORT       GPIOD
#define LEDR2_PIN        GPIO_PIN_4

#define LEDR3_PORT       GPIOD
#define LEDR3_PIN        GPIO_PIN_6


#define BUZZER_PORT       GPIOB
#define BUZZER_PIN        GPIO_PIN_0


#define RT8511_PWM_PORT   GPIOC
#define RT8511_PWM_PIN    GPIO_PIN_4

#define RT8511_EN_PORT   GPIOC
#define RT8511_EN_PIN    GPIO_PIN_5


#define RTC_IRQ_PORT    GPIOB
#define RTC_IRQ_PIN     GPIO_PIN_3

// in
#define KEY1_PORT       GPIOA
#define KEY1_PIN        GPIO_PIN_3

#define KEY2_PORT       GPIOF
#define KEY2_PIN        GPIO_PIN_4

#define KEY3_PORT       GPIOB
#define KEY3_PIN        GPIO_PIN_7


#define LEDCOM_NUM      0
#define LEDERROR_NUM    1            

#define LEDON           1
#define LEDOFF          0
#define LED_TIMEOUT_NUMMAX      1000    // 1000ms
#define LED_SHANSHUO_TIME       1000     //1000ms  500ms ¿ª 500ms ¹Ø

#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
