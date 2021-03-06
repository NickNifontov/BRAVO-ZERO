/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f3xx_hal.h"

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
#define LED_AB_Pin GPIO_PIN_13
#define LED_AB_GPIO_Port GPIOC
#define LED_SD_Pin GPIO_PIN_14
#define LED_SD_GPIO_Port GPIOC
#define LED_VOLT_Pin GPIO_PIN_15
#define LED_VOLT_GPIO_Port GPIOC
#define ADC_V_Pin GPIO_PIN_0
#define ADC_V_GPIO_Port GPIOA
#define ADC_I_Pin GPIO_PIN_1
#define ADC_I_GPIO_Port GPIOA
#define SYNC_AC_Pin GPIO_PIN_2
#define SYNC_AC_GPIO_Port GPIOA
#define ADC_15B_Pin GPIO_PIN_3
#define ADC_15B_GPIO_Port GPIOA
#define DAC_V_Pin GPIO_PIN_4
#define DAC_V_GPIO_Port GPIOA
#define ADC_AB_OPORA_Pin GPIO_PIN_5
#define ADC_AB_OPORA_GPIO_Port GPIOA
#define DAC_I_Pin GPIO_PIN_6
#define DAC_I_GPIO_Port GPIOA
#define COMP_V_Pin GPIO_PIN_7
#define COMP_V_GPIO_Port GPIOA
#define COMP_I_Pin GPIO_PIN_0
#define COMP_I_GPIO_Port GPIOB
#define ADC_15V_Pin GPIO_PIN_1
#define ADC_15V_GPIO_Port GPIOB
#define ADC_AB_Pin GPIO_PIN_2
#define ADC_AB_GPIO_Port GPIOB
#define MCU_IGLA_Pin GPIO_PIN_10
#define MCU_IGLA_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_11
#define BUZZER_GPIO_Port GPIOB
#define IN2_Pin GPIO_PIN_12
#define IN2_GPIO_Port GPIOB
#define MODE_50KHZ_30KHZ_Pin GPIO_PIN_13
#define MODE_50KHZ_30KHZ_GPIO_Port GPIOB
#define ADC_TEMP_Pin GPIO_PIN_14
#define ADC_TEMP_GPIO_Port GPIOB
#define SYNC_INV_Pin GPIO_PIN_15
#define SYNC_INV_GPIO_Port GPIOB
#define IN1_Pin GPIO_PIN_8
#define IN1_GPIO_Port GPIOA
#define MCU_50HZ1_Pin GPIO_PIN_9
#define MCU_50HZ1_GPIO_Port GPIOA
#define COOLER_Pin GPIO_PIN_10
#define COOLER_GPIO_Port GPIOA
#define MCU_50HZ2_Pin GPIO_PIN_11
#define MCU_50HZ2_GPIO_Port GPIOA
#define FLT1_Pin GPIO_PIN_12
#define FLT1_GPIO_Port GPIOA
#define FLT2_Pin GPIO_PIN_15
#define FLT2_GPIO_Port GPIOA
#define DRV_SD_EX_Pin GPIO_PIN_3
#define DRV_SD_EX_GPIO_Port GPIOB
#define DRV_SD_IN_Pin GPIO_PIN_4
#define DRV_SD_IN_GPIO_Port GPIOB
#define VCC_15V_Pin GPIO_PIN_5
#define VCC_15V_GPIO_Port GPIOB
#define VCC_15B_Pin GPIO_PIN_6
#define VCC_15B_GPIO_Port GPIOB
#define LED_POLKA_Pin GPIO_PIN_7
#define LED_POLKA_GPIO_Port GPIOB
#define NC1_Pin GPIO_PIN_8
#define NC1_GPIO_Port GPIOB
#define NC2_Pin GPIO_PIN_9
#define NC2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
