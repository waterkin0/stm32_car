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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void forward(void);//ǰ��
void back(void);//����
void left_turn(void);//��ת 
void right_turn(void);//��ת
void stop(void);//ͣ��
void circle_left(void);//ԭ������תȦ
void circle_right(void);//ԭ������תȦ 

bool black_right(void);//�Ҳ��к�
bool black_left(void);//����к�

int pwmval_left_init = 500;//����ת��,ֵΪ0-2000֮��,ֵԽ���ٶ�Խ��
int pwmval_right_init = 500;
int pwmval_left = 0;
int pwmval_right = 0;
void left_moto(void);
void right_moto(void);

int Angle = 90;//����Ƕ�
int num = 0;
int time = 0;//���к�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//10usһ���ж�
{
	if(TIM1 == htim->Instance)//�������
	{
		num++;
		if(num <= (Angle * 11 + 500)/10)
		{
			HAL_GPIO_WritePin(steer_GPIO_Port, steer_Pin, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(steer_GPIO_Port, steer_Pin, GPIO_PIN_RESET);
		}
		if(num >= 2000) //2000*10us=20ms  20msһ������
		{
			num = 0;
		}
		if(num % 100 == 0)
		{
			time++;
			if(time > 10000)
				time = 0;
		}
		left_moto();
		right_moto();
	}
}
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
	HAL_TIM_Base_Start_IT(&htim1);//������ʱ��
	HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_RESET);//����ֹͣ
	int turn = 0;
	int nofire_time = 0;
	bool fire = false;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(HAL_GPIO_ReadPin(FIRE_GPIO_Port, FIRE_Pin) == GPIO_PIN_RESET)//��⵽����
		{
			fire = true;
			if(time >= 50)
			{
				time = 0;
				if(turn == 1)
					Angle += 10;
				else
					Angle -= 10;
				if(Angle >= 135)
					turn = 0;
				if(Angle <= 45)
					turn = 1;
			}
			stop();
			HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_SET);
		}
		else
		{
			if(time - nofire_time >= 300)//û�л�����ȥ��300ms
			{
				Angle = 90;
				HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_RESET);
				//Ѳ����Դ��
				if(((black_left()==0)&&(black_right()==0))||((black_left()==1)&&(black_right()==1)))//���߶�û�и�Ӧ�����߻������߶��к���
				{
					forward();
				}
				else
				{			  
				 if((black_left()==1)&&(black_right()==0))
				 {
					HAL_Delay(5);
					if((black_left()==1)&&(black_right() == 0))
					{
						circle_left();
					}	 	 
				 }			   			    
				 if((black_right()==1)&&(black_left()==0))  
				 {
					HAL_Delay(5);
					if((black_right()==1)&&(black_left()==0))              
					{		  
					 circle_right();
					}
				 }
				}
			}
			if(fire)
			{
				nofire_time = time;
				fire = false;
			}
		}
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void forward(void)                      //С��ǰ�����ƺ���
{
 HAL_GPIO_WritePin(GPIOA, IN1_Pin, GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOA, IN2_Pin, GPIO_PIN_SET);
 
 HAL_GPIO_WritePin(GPIOA, IN3_Pin, GPIO_PIN_SET);
 HAL_GPIO_WritePin(GPIOA, IN4_Pin, GPIO_PIN_RESET);
}
void back(void)			                    //С�����˿��ƺ���
{ 
 HAL_GPIO_WritePin(GPIOA, IN1_Pin, GPIO_PIN_SET);
 HAL_GPIO_WritePin(GPIOA, IN2_Pin, GPIO_PIN_RESET);
 
 HAL_GPIO_WritePin(GPIOA, IN3_Pin, GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOA, IN4_Pin, GPIO_PIN_SET);
}

void left_turn(void)	                  //����ת
{  
 HAL_GPIO_WritePin(GPIOA, IN1_Pin, GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOA, IN2_Pin, GPIO_PIN_RESET);
 
 HAL_GPIO_WritePin(GPIOA, IN3_Pin, GPIO_PIN_SET);
 HAL_GPIO_WritePin(GPIOA, IN4_Pin, GPIO_PIN_RESET);
}

void right_turn(void)		                //����ת  
{ 
 HAL_GPIO_WritePin(GPIOA, IN1_Pin, GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOA, IN2_Pin, GPIO_PIN_SET);
 
 HAL_GPIO_WritePin(GPIOA, IN3_Pin, GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOA, IN4_Pin, GPIO_PIN_RESET);
}

void stop(void)			                    //ͣ��
{ 
 HAL_GPIO_WritePin(GPIOA, IN1_Pin, GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOA, IN2_Pin, GPIO_PIN_RESET);
 
 HAL_GPIO_WritePin(GPIOA, IN3_Pin, GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOA, IN4_Pin, GPIO_PIN_RESET);
}

void circle_left(void)		              //ԭ������תȦ
{ 
 HAL_GPIO_WritePin(GPIOA, IN1_Pin, GPIO_PIN_SET);
 HAL_GPIO_WritePin(GPIOA, IN2_Pin, GPIO_PIN_RESET);
 
 HAL_GPIO_WritePin(GPIOA, IN3_Pin, GPIO_PIN_SET);
 HAL_GPIO_WritePin(GPIOA, IN4_Pin, GPIO_PIN_RESET);
}
void circle_right(void)		 	            //ԭ������תȦ 
{
 HAL_GPIO_WritePin(GPIOA, IN1_Pin, GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOA, IN2_Pin, GPIO_PIN_SET);
 
 HAL_GPIO_WritePin(GPIOA, IN3_Pin, GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOA, IN4_Pin, GPIO_PIN_SET);
}
bool black_right(void)
{
	if(HAL_GPIO_ReadPin(RIGHT_GPIO_Port,RIGHT_Pin) == GPIO_PIN_SET)
		return true;
	return false;
}
bool black_left(void)
{
	if(HAL_GPIO_ReadPin(LEFT_GPIO_Port,LEFT_Pin) == GPIO_PIN_SET)
		return true;
	return false;
}
void left_moto(void)                    //�������ٺ���
{  
	pwmval_left++;
	if(pwmval_left <= pwmval_left_init)
	{
		HAL_GPIO_WritePin(GPIOA, EN2_Pin, GPIO_PIN_SET);
	}
	else 
	{
		HAL_GPIO_WritePin(GPIOA, EN2_Pin, GPIO_PIN_RESET);
	}
	if(pwmval_left >= 2000)
	{
		pwmval_left = 0;
	}
}
void right_moto(void)                   //�ҵ�����ٺ���
{ 
	pwmval_right++;
  if(pwmval_right <= pwmval_right_init)		  
  {
		HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_SET);							   
  }
  else if(pwmval_right > pwmval_right_init)
  {
		HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_RESET);
  }
  if(pwmval_right >= 2000)
  {
		pwmval_right = 0;
  }
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
