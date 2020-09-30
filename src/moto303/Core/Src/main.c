/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// Config variables
int st_by_power = 250;
int fast_blink = 50;
int slow_blink = 100; // slow_blink > fast_blink
int turn_delay = 2000;

// Service variables
int low_light = 255;
int result = 0x0000;
int time_flag = 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  init_check();
  while (1)
  {
    measure();

    // Read inputs and make a decision

    if (HAL_GPIO_ReadPin(ST_BY_GPIO_Port, ST_BY_Pin) == GPIO_PIN_RESET)
    {
      low_light = st_by_power; // Slight light
    }
    else
    {
      low_light = 0; // Light off
    }

    switch (result)
    {
    case 0: // OFF
      off();
      break;
    case 1: // STOP & RIGHT
      sright();
      break;
    case 2: // STOP & LEFT
      sleft();
      break;
    case 3: // STOP when turning and STROBE by default
      if (HAL_GetTick() > (time_flag + turn_delay))
      {
        strobe(); // Strobe signal on stop when going straight
      }
      else
      {
        stop(); // Just the stop signal in pauses of turn signal blinking
      }
      break;
    case 4: // OFF
      off();
      break;
    case 5: // RIGHT
      right();
      break;
    case 6: // LEFT
      left();
      break;
    case 7: // OFF
      off();
      break;
    default: // NO input or error occurred
      off();
      break;
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM1;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 32;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 256;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : LEFT_Pin RIGHT_Pin ST_BY_Pin STOP_Pin */
  GPIO_InitStruct.Pin = LEFT_Pin|RIGHT_Pin|ST_BY_Pin|STOP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : button_Pin */
  GPIO_InitStruct.Pin = button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(button_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
int measure()
{
  result ^= (-(HAL_GPIO_ReadPin(LEFT_GPIO_Port, LEFT_Pin)) ^ result) & (1UL << 0);
  result ^= (-(HAL_GPIO_ReadPin(RIGHT_GPIO_Port, RIGHT_Pin)) ^ result) & (1UL << 1);
  result ^= (-(HAL_GPIO_ReadPin(STOP_GPIO_Port, STOP_Pin)) ^ result) & (1UL << 2);
  return 0;
}

int init_check()
{
  for (int i = 0; i > 255; i++)
  {
    TIM1->CCR2 = i;
    TIM1->CCR4 = 255 - i;
    HAL_Delay(10);
  }
  return 0;
}

int stop()
{
  TIM1->CCR2 = 255;
  TIM1->CCR4 = 255;
  return 0;
}

int strobe()
{
  // Fast blink
  TIM1->CCR2 = 0;
  TIM1->CCR4 = 0;
  HAL_Delay(fast_blink);

  TIM1->CCR2 = 255;
  TIM1->CCR4 = 255;
  HAL_Delay(fast_blink);

  TIM1->CCR2 = 0;
  TIM1->CCR4 = 0;
  HAL_Delay(fast_blink);

  TIM1->CCR2 = 255;
  TIM1->CCR4 = 255;
  HAL_Delay(fast_blink);

  TIM1->CCR2 = 0;
  TIM1->CCR4 = 0;
  HAL_Delay(fast_blink);

  TIM1->CCR2 = 255;
  TIM1->CCR4 = 255;
  HAL_Delay(fast_blink);

  // Slow blink if break is not released yet
  measure();
  if (result == 3)
  {
    TIM1->CCR2 = 0;
    TIM1->CCR4 = 0;
    HAL_Delay(slow_blink);

    TIM1->CCR2 = 255;
    TIM1->CCR4 = 255;
    HAL_Delay(slow_blink);

    TIM1->CCR2 = 0;
    TIM1->CCR4 = 0;
    HAL_Delay(slow_blink);

    TIM1->CCR2 = 255;
    TIM1->CCR4 = 255;
    HAL_Delay(slow_blink);

    TIM1->CCR2 = 0;
    TIM1->CCR4 = 0;
    HAL_Delay(slow_blink);

    TIM1->CCR2 = 255;
    TIM1->CCR4 = 255;
    HAL_Delay(slow_blink);
  }
  else
  {
    return 0;
  }

  // Static stop light
  while (result == 3)
  {
    TIM1->CCR2 = 255;
    TIM1->CCR4 = 255;
    HAL_Delay(100);
    measure();
  }
  return 0;
}

int left()
{
  TIM1->CCR2 = 0;
  TIM1->CCR4 = 255;
  time_flag = HAL_GetTick();
  return 0;
}

int right()
{
  TIM1->CCR2 = 255;
  TIM1->CCR4 = 0;
  time_flag = HAL_GetTick();
  return 0;
}

int sleft()
{
  TIM1->CCR2 = 255;
  TIM1->CCR4 = 0;
  time_flag = HAL_GetTick();
  return 0;
}

int sright()
{
  TIM1->CCR2 = 0;
  TIM1->CCR4 = 255;
  time_flag = HAL_GetTick();
  return 0;
}

int off()
{
  TIM1->CCR2 = low_light;
  TIM1->CCR4 = low_light;
  return 0;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
