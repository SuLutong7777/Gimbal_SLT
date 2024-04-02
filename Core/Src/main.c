/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "util_pwm.h"
#include "periph_servo.h"
#include "periph_pc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
////PWM�����
//PWM_PWMHandleTypeDef pwm_pitch;
//PWM_PWMHandleTypeDef pwm_yaw;
//float duty_pitch = 0.06;
//float duty_yaw = 0.06;

////servo�����
// Servo_ServoTypeDef servo_pitch;
// Servo_ServoTypeDef servo_yaw;
float servo_pitch_angle = 100;       //�϶�� 65 - 135
float servo_yaw_angle = 100;         //�¶��  0 - 350
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
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	
	//// PWM�����
	/// ��������Ϊ��ָ������������
	//PWM_InitPWM(pwm_yaw, &htim2, 4);
	//PWM_StartPWM(pwm_yaw);
	/// ��������Ϊ����ָ����������ȷ��
	//PWM_InitPWM(&pwm_pitch, &htim2, TIM_CHANNEL_3);
	//PWM_InitPWM(&pwm_yaw, &htim2, TIM_CHANNEL_4);
	//PWM_SetPWMFreq(&pwm_pitch, 50);
	//PWM_SetPWMFreq(&pwm_yaw, 50);
	//PWM_StartPWM(&pwm_pitch);
	//PWM_StartPWM(&pwm_yaw);
	
	////servo�����
	// Servo_InitServo(&servo_pitch, &htim2, TIM_CHANNEL_3);
	// Servo_InitServo(&servo_yaw, &htim2, TIM_CHANNEL_4);
	// Servo_SetServoAngle(&servo_pitch, servo_pitch_angle);
	// Servo_SetServoAngle(&servo_yaw, servo_yaw_angle);
	// Servo_StartServo(&servo_yaw);
	// Servo_StartServo(&servo_pitch);

  ////gimbal�����
  Gimbal_InitGimbal();
  Gimbal_SetGimbalAngle(servo_pitch_angle, servo_yaw_angle);
  Gimbal_StartGimbal();
  Gimbal_OnlineNotice();       // ����֪ͨ

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		////PWM�����
		//PWM_SetPWMDuty(&pwm_pitch, duty_pitch);
		//PWM_SetPWMDuty(&pwm_yaw, duty_yaw);
		////servo�����
		// Servo_SetServoAngle(&servo_pitch, servo_pitch_angle);
		// Servo_SetServoAngle(&servo_yaw, servo_yaw_angle);
		HAL_Delay(60000);                // ��1���ӷ���һ��
		Gimbal_Heartbeat();              // ����������
		
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
