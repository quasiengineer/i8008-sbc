/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

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
#define STM_IN_D0_Pin GPIO_PIN_0
#define STM_IN_D0_GPIO_Port GPIOC
#define STM_IN_D1_Pin GPIO_PIN_1
#define STM_IN_D1_GPIO_Port GPIOC
#define STM_IN_D2_Pin GPIO_PIN_2
#define STM_IN_D2_GPIO_Port GPIOC
#define STM_IN_D3_Pin GPIO_PIN_3
#define STM_IN_D3_GPIO_Port GPIOC
#define STM_OUT_D0_Pin GPIO_PIN_0
#define STM_OUT_D0_GPIO_Port GPIOA
#define STM_OUT_D1_Pin GPIO_PIN_1
#define STM_OUT_D1_GPIO_Port GPIOA
#define STM_OUT_D2_Pin GPIO_PIN_2
#define STM_OUT_D2_GPIO_Port GPIOA
#define STM_OUT_D3_Pin GPIO_PIN_3
#define STM_OUT_D3_GPIO_Port GPIOA
#define STM_OUT_D4_Pin GPIO_PIN_4
#define STM_OUT_D4_GPIO_Port GPIOA
#define STM_OUT_D5_Pin GPIO_PIN_5
#define STM_OUT_D5_GPIO_Port GPIOA
#define STM_OUT_D6_Pin GPIO_PIN_6
#define STM_OUT_D6_GPIO_Port GPIOA
#define STM_OUT_D7_Pin GPIO_PIN_7
#define STM_OUT_D7_GPIO_Port GPIOA
#define STM_IN_D4_Pin GPIO_PIN_4
#define STM_IN_D4_GPIO_Port GPIOC
#define STM_IN_D5_Pin GPIO_PIN_5
#define STM_IN_D5_GPIO_Port GPIOC
#define STM_IN_S2_Pin GPIO_PIN_12
#define STM_IN_S2_GPIO_Port GPIOB
#define STM_IN_S1_Pin GPIO_PIN_13
#define STM_IN_S1_GPIO_Port GPIOB
#define STM_IN_S0_Pin GPIO_PIN_14
#define STM_IN_S0_GPIO_Port GPIOB
#define STM_IN_SYNC_Pin GPIO_PIN_15
#define STM_IN_SYNC_GPIO_Port GPIOB
#define STM_IN_D6_Pin GPIO_PIN_6
#define STM_IN_D6_GPIO_Port GPIOC
#define STM_IN_D7_Pin GPIO_PIN_7
#define STM_IN_D7_GPIO_Port GPIOC
#define STM_OUT_PH2_Pin GPIO_PIN_9
#define STM_OUT_PH2_GPIO_Port GPIOC
#define STM_OUT_PH1_Pin GPIO_PIN_8
#define STM_OUT_PH1_GPIO_Port GPIOA
#define STM_OUT_RDY_Pin GPIO_PIN_9
#define STM_OUT_RDY_GPIO_Port GPIOA
#define STM_OUT_INT_Pin GPIO_PIN_10
#define STM_OUT_INT_GPIO_Port GPIOA
#define LED_OUT_Pin GPIO_PIN_14
#define LED_OUT_GPIO_Port GPIOA
#define STM_OUT_DBUS_Pin GPIO_PIN_15
#define STM_OUT_DBUS_GPIO_Port GPIOA
#define TEST2_Pin GPIO_PIN_2
#define TEST2_GPIO_Port GPIOD
#define TEST1_Pin GPIO_PIN_5
#define TEST1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
