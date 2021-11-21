
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "fatfs.h"

/* USER CODE BEGIN Includes */
#include "CTR_nrf24l01.h"
#include "dwt_stm32_delay.h"
#include "stdio.h"
#include "string.h"
#include "sd.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
extern FATFS SDFatFs; //File system object structure (FATFS) goi lai bien 


//----------------------khai bao bien cho ham doc nhiet do va do am-----------------------//
uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t sum, RH, TEMP, soid_RH_adc;
int soid_RH;
int temp_low, temp_high, rh_low, rh_high;
char temp_char1[2], temp_char2, rh_char1[2], rh_char2;
uint8_t check = 0;
GPIO_InitTypeDef GPIO_InitStruct;

char nhiet_do_va_do_am[32];

char nhiet_do1[32];
char nhiet_do2[32];

char do_am1[32];
char do_am2[32];

char do_am_soid[32];

char m[32];
char r[32];

char chuoi_cat[32];

char min_c[32];
char max_c[32];
int lon_c;//max-min cu nhiet do
int nho_c;//max-min cu nhiet do

char min_h[32];
char max_h[32];
int lon_h;//max-min cu do am 
int nho_h;//max-min cu do am 

char min_s[32];
char max_s[32];
int lon_s;//max-min cu do am dat
int nho_s;//max-min cu do am dat

//----------------------khai bao bien cho ham doc nhiet do va do am-----------------------//

void set_gpio_output (void)
{
	/*Configure GPIO pin output: PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void set_gpio_input (void)
{
	/*Configure GPIO pin input: PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


//----------------------khai bao bien cho ham truyen nhan NRF24L01 A-----------------------//
uint8_t chuoi[32];
uint8_t chuoi_nhan[32]="chuoi_nhan\r\n";

uint8_t dang_truyen[32]="dang truyen\r\n";
uint8_t dang_nhan[32]="dang nhan\r\n";
uint8_t loi[32]="khong thanh cong\r\n";
uint8_t chuoi_loi[32]="chuoi loi\r\n";
uint8_t ket_thuc[32]="KET THUC\r\n";

char ok[32]="ok\r\n";
char check_1a[32]="min1a";
char check_2a[32]="min2a";

uint8_t check_lai_lan_cuoi[32]="check lai lan cuoi\r\n";
uint8_t test[32]="start\r\n";
char xuong_hang[32]="\r\n";
uint8_t du_lieu_moi_truong[32]="25.5 oC\r\n";
int a=0;
int b=0;
int thoi_gian_check=0;
int thoi_gian_truyen=0;
//----------------------khai bao bien cho ham truyen nhan NRF24L01 A-----------------------//

//----------------------khai bao bien cho ham truyen nhan NRF24L01 B-----------------------//
char bat_bom[32]="bat bom\r\n";
char tat_bom[32]="tat bom\r\n";
char dong_mai[32]="dong mai che\r\n";
char mo_mai[32]="mo mai che\r\n";

//----------------------khai bao bien cho ham truyen nhan NRF24L01 B-----------------------//

void NRF24L01_Config()
{
	CTR_nrfPinConfig();
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI2_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
//-------------------------tao cac ham cho viec doc nhiet do va do am----------------//
void DHT22_start (void)
{
	set_gpio_output ();  // set the pin as output
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_1, 0);   // pull the pin low
	DWT_Delay_us (500);   // wait for 500us
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_1, 1);   // pull the pin high
	DWT_Delay_us (30);   // wait for 30us
	set_gpio_input ();   // set as input
}

void check_response (void)
{
	DWT_Delay_us (40);
	if (!(HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)))
	{
		DWT_Delay_us (80);
		if ((HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1))) check = 1;
	}
	while ((HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)));   // wait for the pin to go low
}

uint8_t read_data (void)
{
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)));   // wait for the pin to go high
		DWT_Delay_us (40);   // wait for 40 us
		if ((HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)) == 0)   // if the pin is low 
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)));  // wait for the pin to go low
	}
	return i;
}
//-------------------------tao cac ham cho viec doc nhiet do va do am----------------//


/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
struct sd_data SD;



//-----------------ham chuyen doi lieu so sang kieu chuoi----------------//
void UlToStr(char *s, unsigned long bin, unsigned char n)
{
    s += n;
    *s = '\0';

    while (n--)
    {
        *--s = (bin % 10) + '0';
        bin /= 10;
    }
}
//-----------------ham chuyen doi lieu so sang kieu chuoi----------------//

//------------------------------------------------------------------ doc soid humidity, temperature va humidity ---------------------------------------------------//
void read()
{
	//---------------doc temperature va humidity --------------//
	DHT22_start ();
	check_response ();
	Rh_byte1 = read_data ();
	Rh_byte2 = read_data ();
	Temp_byte1 = read_data ();
	Temp_byte2 = read_data ();
	sum = read_data();
	if (sum == (Rh_byte1+Rh_byte2+Temp_byte1+Temp_byte2))
	{
		TEMP = ((Temp_byte1<<8)|Temp_byte2);
		RH = ((Rh_byte1<<8)|Rh_byte2);
	}
	//---------------doc temperature va humidity --------------//
	
	
	//---------------doc soid humidity --------------//
	//0 den 4096 gia tri adc
	//0 den	100%
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&soid_RH_adc,1);
		HAL_Delay(1);
		HAL_ADC_Stop_DMA(&hadc1);
		soid_RH= (soid_RH_adc*100)/4096;
	
	//---------------doc soid humidity --------------//
	
	
		temp_low = TEMP/10;
//	temp_high = TEMP%10;
//	
		rh_low = RH/10;
//	rh_high = RH%10;
	
	//----------------------------------------chuyen tu so sang chuoi------------------------------------------------------//
	UlToStr (nhiet_do1, (TEMP/10), 2); //chuyen so '(TEMP/10)' sang chuoi co 2 gia tri
	UlToStr (nhiet_do2, (TEMP%10), 1);//chuyen so '(TEMP%10)' sang chuoi co 1 gia tri
	UlToStr (do_am1, (RH/10), 2);
	UlToStr (do_am2, (RH%10), 1);
	
	int soid_RH1=100-soid_RH;
	UlToStr (do_am_soid, soid_RH1, 2);
	
	char m [10];
	char r [10];
	//---------------dieu khien motor tuoi-----------------//
	if(nho_s < soid_RH1 && soid_RH1 < lon_s)//neu dat qua kho
	{
		if(nho_c < temp_low && temp_low < lon_c)//temp_low la gia tri int 2byte cua TEMP(kieu uint16_t)
		{
			//HAL_UART_Transmit(&huart1,bat_bom,32,5);
			if(nho_h < rh_low && rh_low < lon_h)//neu dat qua kho
			{	
				HAL_UART_Transmit(&huart1,bat_bom,32,5);
				//snprintf(m, sizeof(m), "%s", "ON_1b");	
				snprintf(m, sizeof(m), "%s", "ON_2b");	
			}
			else //neu dat qua uot hoac he thong loi
			{
				HAL_UART_Transmit(&huart1,tat_bom,32,5);
				//snprintf(m, sizeof(m), "%s", "OFF_1b");	
				snprintf(m, sizeof(m), "%s", "OFF_2b");	
			}
		}
		else //neu dat qua uot hoac he thong loi
		{
			HAL_UART_Transmit(&huart1,tat_bom,32,5);
			//snprintf(m, sizeof(m), "%s", "OFF_1b");	
			snprintf(m, sizeof(m), "%s", "OFF_2b");	
		}
	}
	else //neu dat qua uot hoac he thong loi
	{
		HAL_UART_Transmit(&huart1,tat_bom,32,5);
		//snprintf(m, sizeof(m), "%s", "OFF_1b");	
		snprintf(m, sizeof(m), "%s", "OFF_2b");	
	}
	//---------------dieu khien motor tuoi-----------------//
	
	//---------------dieu khien motor rem-----------------//
	if(30 < TEMP && TEMP < 40)
	{
		//if(TEMP
		HAL_UART_Transmit(&huart1,dong_mai,32,5);
		snprintf(r, sizeof(r), "%s", "D");	
	}
	else
	{
		HAL_UART_Transmit(&huart1,mo_mai,32,5);
		snprintf(r, sizeof(r), "%s", "M");
	}
	//---------------dieu khien motor rem-----------------//
	
	//TONG KET CHUOI GUI DI//
	snprintf(nhiet_do_va_do_am, sizeof(nhiet_do_va_do_am), "C=%s.%s&H=%s.%s&S=%s&R=%s&M=%s", nhiet_do1,nhiet_do2,do_am1,do_am2,do_am_soid,r,m);	
	//TONG KET CHUOI GUI DI//
	//----------------------------------------chuyen tu so sang chuoi------------------------------------------------------//	
}
//------------------------------------------------------------------ doc soid humidity, temperature va humidity ---------------------------------------------------//
		
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_SPI2_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
	DWT_Delay_Init ();
	
	
	disk_initialize(SDFatFs.drv);
	SD_Write_File("Data.txt","nhietdo",100);
	HAL_Delay(10);
	SD_List_File();
	
	//NRF24L01_Config();
	//CTR_nrfInit(); // khoi tao kenh 4, luong 3, ..
	//CTR_spiRWreg(CTR_WRITE_RE + CTR_STATUS, 0xFF);//XOA BIT tren thanh Status
	//CTR_nrfSetRX();//	do co delay nen chi nen goi 1 lan de nhan duoc du lieu tot hon
	// them doan ma can gui vao buffer gui
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{ 
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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
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
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI2 init function */
static void MX_SPI2_Init(void)
{

  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
		 tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
