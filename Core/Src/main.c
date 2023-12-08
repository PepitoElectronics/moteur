/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// Nombre de paires de pôles sur le rotor
#define PAIRES_DE_POLES 3
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t data [20];
int send_flag;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
static void MX_GPIO_Init_perso(int type);
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
	//int total_angle = 0;  // Utilisé pour stocker l'angle total parcouru
	uint16_t angle_increment = 360.0 / (2.0 * PAIRES_DE_POLES * 30);// Angle associé à chaque transition des capteurs
	int state_Hal_A;
	int state_Hal_B;
	int old_state_Hal_A = 0;
	int old_state_Hal_B = 0;
	uint16_t total_angle;
	uint16_t total_angle_copy;

	uint8_t angle_bit_array [16];

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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  old_state_Hal_A = HAL_GPIO_ReadPin(GPIOA, 9);
  old_state_Hal_B = HAL_GPIO_ReadPin(GPIOA, 8);
  total_angle = 0;
  total_angle_copy = 0;
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if(send_flag){
		  if(send_flag == 1){
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2, 0 );
			total_angle_copy = total_angle;
			MX_GPIO_Init_perso(1);
			// Parcours des bits de droite à gauche (du bit 7 au bit 0)
			for (int i = 15; i >= 0; i--) {
				// Opération de décalage à droite pour isoler le bit
				unsigned char bit = (total_angle_copy >> i) & 0x01;
				angle_bit_array[i]=bit;
			}
			//HAL_GPIO_WritePin(GPIOB, 2, angle_bit_array[send_flag-1]);
		  }
		  else{
			  //int test = angle_bit_array[send_flag-2];
			if(angle_bit_array[send_flag-2] == 1){
				//int test = angle_bit_array[send_flag-2];
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2, 1 );
			}
			else{
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2, 0 );
			}
		  }
	  }
	  else{
		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2, 1);
	  }


	  if(send_flag>21){
		MX_GPIO_Init_perso(0);
		send_flag=0;
		//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2, 0 );
	  }

	  state_Hal_A = HAL_GPIO_ReadPin(HALL_A_GPIO_Port, HALL_A_Pin);
	  /*memset(data,'0',sizeof(data));
	  sprintf(data,"%i",state_Hal_A);
	  HAL_UART_Transmit(&huart2, data, 5, 10);
	  HAL_UART_Transmit(&huart2, "-", 1, 10);
	  HAL_Delay(10);*/
	  state_Hal_B = HAL_GPIO_ReadPin(HALL_B_GPIO_Port, HALL_B_Pin);
	  /*memset(data,'0',sizeof(data));
	  sprintf(data,"%i",state_Hal_B);
	  HAL_UART_Transmit(&huart2, data, 5, 10);
	  HAL_UART_Transmit(&huart2, "\n\r", 5, 10);
	  HAL_Delay(1000);*/

	  if(state_Hal_A != old_state_Hal_A){
		  if(state_Hal_A!=state_Hal_B){
			  // sens positif + 90°
			  total_angle += angle_increment;
		  }
		  else if(state_Hal_A == state_Hal_B){
			  //sens - 90°
			  total_angle -= angle_increment;
		  }
	  }
	  old_state_Hal_A = state_Hal_A;
	  old_state_Hal_B = state_Hal_B;
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(dataout_GPIO_Port, dataout_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : dataout_Pin */
  GPIO_InitStruct.Pin = dataout_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(dataout_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : HALL_B_Pin HALL_A_Pin */
  GPIO_InitStruct.Pin = HALL_B_Pin|HALL_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : clock_master_Pin */
  GPIO_InitStruct.Pin = clock_master_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(clock_master_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
static void MX_GPIO_Init_perso(int type)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = clock_master_Pin;
  if(type==1){
	  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  }
  else{
	  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  }
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(clock_master_GPIO_Port, &GPIO_InitStruct);

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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
