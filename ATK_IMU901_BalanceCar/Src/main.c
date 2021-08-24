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
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "pid.h"
#include "control.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "pid.h"
#include "atk_imu901.h"
#include "motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern int Vertical_out,Velocity_out,Turn_out;//直立环&速度环&转向环 的输出变量
extern float Med_Angle;//机械中值--能使得小车真正平衡住的角度。
extern float Vertical_Kp;
float Pitch,Roll,Yaw;						//角度
short gyrox,gyroy,gyroz;				//陀螺仪--角速度
short aacx,aacy,aacz;						//加速度
int Encoder_Left,Encoder_Right;	//编码器数据（速度）
int PWM_MAX=7200,PWM_MIN=-7200;	//PWM限幅变量
int MOTO1,MOTO2;								//电机装载变量



static uint8_t Get_moter_Flag = 0;
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
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();
	
	/* Configure the MPU attributes for the QSPI 256MB without instruction access */
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress      = QSPI_BASE;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_256MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
	
  /* Configure the MPU attributes for the QSPI 8MB (QSPI Flash Size) to Cacheable WT */
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress      = QSPI_BASE;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_8MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
	
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

static void LED_Blink(uint32_t Hdelay,uint32_t Ldelay)
{
	HAL_GPIO_WritePin(E3_GPIO_Port,E3_Pin,GPIO_PIN_SET);
	HAL_Delay(Hdelay - 1);
	HAL_GPIO_WritePin(E3_GPIO_Port,E3_Pin,GPIO_PIN_RESET);
	HAL_Delay(Ldelay-1);
}

/**
  * @brief  Get the current time and date.
  * @param  
  * @retval None
  */
static void RTC_CalendarShow(RTC_DateTypeDef *sdatestructureget,RTC_TimeTypeDef *stimestructureget)
{
  /* 必须同时获取时间和日期 不然会导致下次RTC不能读取 */
  /* Both time and date must be obtained or RTC cannot be read next time */
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, sdatestructureget, RTC_FORMAT_BIN);
}
#pragma import(__use_no_semihosting)
/* 解决HAL库使用时, 某些情况可能报错的bug */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 解决HAL库使用时, 某些情况可能报错的bug */
struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

/* FILE is typedef’ d in stdio.h. */
FILE __stdout;

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}
///* 重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1 , (uint8_t *)&ch, 1, 0xFFF);
    return ch;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  #ifdef W25Qxx
    SCB->VTOR = QSPI_BASE;
  #endif
  MPU_Config();
  CPU_CACHE_Enable();
	uint8_t uart_temp[100];
    uint8_t ch;
	uint32_t times = 0;
	int PWM_out;
	uint32_t Lcd_Temp[20];
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
  MX_RTC_Init();
  MX_SPI4_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_TIM7_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	//LCD_Test();
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
	HAL_Delay(100);
	imu901_init();							/* IMU901模块初始 */
	//HAL_Delay(1000);//等待校准
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1); // 开启编码器A0
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_2); // 开启编码器A1
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1); // 开启编码器B0
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_2); // 开启编码器B1
	
		HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);	//打开电机调速
		HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	HAL_TIM_Base_Start_IT(&htim6);                // 使能定时器中断(10ms)	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	

  while (1)
  {   
	  //HAL_Delay(50);
	  //HAL_Delay(100);
	  //HAL_GPIO_TogglePin(E3_GPIO_Port,E3_Pin);

//		float Pitch,Roll,Yaw;						//角度
//		short gyrox,gyroy,gyroz;				//陀螺仪--角速度
//		short aacx,aacy,aacz;						//加速度

      //printf("姿态角[XYZ]:    %-6.1f     %-6.1f     %-6.1f   (°)\r\n", attitude.roll, attitude.pitch, attitude.yaw);
      //printf("加速度[XYZ]:    %-6.3f     %-6.3f     %-6.3f   (g)\r\n", gyroAccData.faccG[0], gyroAccData.faccG[1], gyroAccData.faccG[2]);
//	    printf("姿态角Y,Pitch:%-6.1f °\r\n", attitude.pitch);
//      printf("加速度YZ,gyroy:%-6.3f,gyroz:%-6.3f g\r\n",gyroAccData.faccG[1], gyroAccData.faccG[2]);
		while(imu901_uart_receive(&ch, 1)) 	/*!< 获取串口fifo一个字节 */
		{
					if(imu901_unpack(ch)) 			/*!< 解析出有效数据包 */
					{
						if(rxPacket.startByte2 == UP_BYTE2) 			/*!< 主动上传的数据包 */
						{
							atkpParsing(&rxPacket);
						}
					}
		}

			Pitch = attitude.pitch;
			gyroy = gyroAccData.fgyroD[1];
			gyroz = gyroAccData.fgyroD[2];
			HAL_GPIO_TogglePin(E3_GPIO_Port,E3_Pin);
			//printf("Left:%d,Right:%d\r\n",Encoder_Left,Encoder_Right);
			
			//2.将数据压入闭环控制中，计算出控制输出量。
			Vertical_out=Vertical(Med_Angle,Pitch,gyroy);				//直立环
			Velocity_out=Velocity(Encoder_Left,Encoder_Right);	//速度环
			Turn_out=Turn(gyroz);																//转向环			
			PWM_out=Vertical_out-Vertical_Kp*Velocity_out;//最终输出
			//printf("PWM_out:%d\r\n",PWM_out);
			//3.把控制输出量加载到电机上，完成最终的的控制。
			MOTO1=PWM_out-Turn_out;//左电机
			MOTO2=PWM_out+Turn_out;//右电机
			Limit(&MOTO1,&MOTO2);//PWM限幅			
			Load(MOTO1,MOTO2);//加载到电机上。
			Get_moter_Flag = 0;
			printf("%-6.1f,%-6.3f,%-6.3f,%d,%d,%d\r\n", attitude.pitch,gyroAccData.faccG[1], gyroAccData.faccG[2],Encoder_Left,Encoder_Right,PWM_out);
			HAL_GPIO_TogglePin(E3_GPIO_Port,E3_Pin);
		
		

		

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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&htim7))
    {	

		
	
			
    }
    if (htim == (&htim6))
    {		
		
		GetMotorPulse();

//		Pitch = attitude.pitch;
//		gyroy = gyroAccData.faccG[1];
//		gyroz = gyroAccData.faccG[2];
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
  LED_Blink(500,500);
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
