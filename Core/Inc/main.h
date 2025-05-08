/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/


//WIFI ENABLE GPIO

#define WIFI_EN_Pin GPIO_PIN_5       //GPIO_PIN_12 is S03 WIFI :GPIO_PIN_4 is s04w
#define WIFI_EN_GPIO_Port GPIOB



//GPIO CTL

#define RELAY_Pin                  GPIO_PIN_0//GPIO_PIN_15
#define RELAY_GPIO_Port            GPIOB//GPIOC


#define PLASMA_Pin 							GPIO_PIN_5//GPIO_PIN_15
#define PLASMA_GPIO_Port 					GPIOA
//BUZZER GPIO
#define BUZZER_PWM_Pin 						GPIO_PIN_1
#define BUZZER_PWM_GPIO_Port 				GPIOB

//TM1639 GPIO
#define MCU_DIO_Pin 					   GPIO_PIN_15//GPIO_PIN_10
#define MCU_DIO_GPIO_Port                  GPIOA//GPIOB

#define MCU_STB_Pin 						GPIO_PIN_12//GPIO_PIN_11
#define MCU_STB_GPIO_Port 					GPIOA//GPIOB

#define MCU_CLK_Pin 						GPIO_PIN_11//GPIO_PIN_12
#define MCU_CLK_GPIO_Port 					GPIOA//GPIOB

//PWM CTL
#define ULTRA_PWM_Pin GPIO_PIN_8
#define ULTRA_PWM_GPIO_Port GPIOA

//TAPE LED CTL
#define LED_TAPE_CTL_Pin 				GPIO_PIN_2//GPIO_PIN_11
#define LED_TAPE_CTL_GPIO_Port 			GPIOB//GPIOA




//DHT11 GPIO
#define TEMP_SENSOR_Pin 					GPIO_PIN_13
#define TEMP_SENSOR_GPIO_Port 				GPIOB

//FAN GPIO

#define FAN_CCW_Pin                GPIO_PIN_6
#define FAN_CCW_GPIO_Port           GPIOA

#define FAN_CW_Pin               GPIO_PIN_7
#define FAN_CW_GPIO_Port         GPIOA






/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
