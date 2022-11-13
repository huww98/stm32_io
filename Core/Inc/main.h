/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
extern I2C_HandleTypeDef hi2c1;

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
#define STATUS_Pin GPIO_PIN_13
#define STATUS_GPIO_Port GPIOC
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_3
#define LED3_GPIO_Port GPIOA
#define OLED_PWR_Pin GPIO_PIN_4
#define OLED_PWR_GPIO_Port GPIOA
#define OLED_RE__Pin GPIO_PIN_5
#define OLED_RE__GPIO_Port GPIOA
#define SER_Pin GPIO_PIN_6
#define SER_GPIO_Port GPIOA
#define RCK_Pin GPIO_PIN_7
#define RCK_GPIO_Port GPIOA
#define SCK_Pin GPIO_PIN_0
#define SCK_GPIO_Port GPIOB
#define FOCUS_Pin GPIO_PIN_1
#define FOCUS_GPIO_Port GPIOB
#define BTN3_Pin GPIO_PIN_12
#define BTN3_GPIO_Port GPIOB
#define BTN3_EXTI_IRQn EXTI15_10_IRQn
#define BTN2_Pin GPIO_PIN_13
#define BTN2_GPIO_Port GPIOB
#define BTN2_EXTI_IRQn EXTI15_10_IRQn
#define BTN1_Pin GPIO_PIN_14
#define BTN1_GPIO_Port GPIOB
#define BTN1_EXTI_IRQn EXTI15_10_IRQn
#define BTN0_Pin GPIO_PIN_15
#define BTN0_GPIO_Port GPIOB
#define BTN0_EXTI_IRQn EXTI15_10_IRQn
#define SHUTTER_IN_Pin GPIO_PIN_9
#define SHUTTER_IN_GPIO_Port GPIOA
#define FOCUS_IN_Pin GPIO_PIN_10
#define FOCUS_IN_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
