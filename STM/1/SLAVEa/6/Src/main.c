
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include "CTR_nrf24l01.h"
#include "dwt_stm32_delay.h"
#include "stdio.h"
#include "string.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
//----------------------khai bao bien cho ham doc nhiet do va do am-----------------------//
uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t sum, RH, TEMP;
int temp_low, temp_high, rh_low, rh_high;
char temp_char1[2], temp_char2, rh_char1[2], rh_char2;
uint8_t check = 0;
GPIO_InitTypeDef GPIO_InitStruct;

char nhiet_do_va_do_am[32];
char nhiet_do1[32];
char nhiet_do2[32];

char do_am1[32];
char do_am2[32];
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

uint8_t dang_truyen[32]="dang truyen\r\n";
uint8_t dang_nhan[32]="dang nhan\r\n";
uint8_t loi[32]="khong thanh cong\r\n";
uint8_t chuoi_loi[32]="chuoi loi\r\n";
uint8_t ket_thuc[32]="KET THUC\r\n";

char ok[32]="ok\r\n";
char check_1a[32]="yeu cau slave 1a doc du lieu\r\n";
char check_2a[32]="yeu cau slave 2a doc du lieu\r\n";

uint8_t check_lai_lan_cuoi[32]="check lai lan cuoi\r\n";
uint8_t test[32]="start\r\n";
char T[32]=" C   ";
char H[32]=" %\r\n";
char xuong_hang[32]="\r\n";
uint8_t du_lieu_moi_truong[32]="25.5 oC\r\n";
int a=0;
int b=0;
int thoi_gian_check=0;
int thoi_gian_truyen=0;
//----------------------khai bao bien cho ham truyen nhan NRF24L01 A-----------------------//

//----------------------khai bao bien cho ham truyen nhan NRF24L01 B-----------------------//
char bat_bom_1b[32]="bat bom 1b\r\n";
char tat_bom_1b[32]="tat bom 1b\r\n";
char bat_bom_2b[32]="bat bom 2b\r\n";
char tat_bom_2b[32]="tat bom 2b\r\n";
char loi_he_thong[32]="Loi he thong\r\n";
//----------------------khai bao bien cho ham truyen nhan NRF24L01 B-----------------------//

void NRF24L01_Config()
{
	CTR_nrfPinConfig();
}
/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);



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

//------------------------------------------------------------------ doc temperature va huminity ---------------------------------------------------//
void read()
{
	DHT22_start ();
	check_response ();
	Rh_byte1 = read_data ();
	Rh_byte2 = read_data ();
	Temp_byte1 = read_data ();
	Temp_byte2 = read_data ();
	sum = read_data();
	//if (sum == (Rh_byte1+Rh_byte2+Temp_byte1+Temp_byte2))
	{
		TEMP = ((Temp_byte1<<8)|Temp_byte2);
		RH = ((Rh_byte1<<8)|Rh_byte2);
	}
	
	temp_low = TEMP/10;
	temp_high = TEMP%10;
	
	rh_low = RH/10;
	rh_high = RH%10;
	
	//----------------------------------------chuyen tu so sang chuoi------------------------------------------------------//
	UlToStr (nhiet_do1, (TEMP/10), 2); //chuyen so sang chuoi
	UlToStr (nhiet_do2, (TEMP%10), 1);
	UlToStr (do_am1, (RH/10), 2);
	UlToStr (do_am2, (RH%10), 1);
	
	if(300< TEMP && TEMP < 400)
	{
		HAL_UART_Transmit(&huart1,bat_bom_1b,32,5);
		//HAL_UART_Transmit(&huart1,bat_bom_2b,32,5);
		snprintf(nhiet_do_va_do_am, sizeof(nhiet_do_va_do_am), "C=%s.%s&H=%s.%s&motor=ON_1b\r\n", nhiet_do1,nhiet_do2,do_am1,do_am2);
		//snprintf(nhiet_do_va_do_am, sizeof(nhiet_do_va_do_am), "C=%s.%s&H=%s.%s&motor=ON_2b\r\n", nhiet_do1,nhiet_do2,do_am1,do_am2);
	}
	else if(TEMP<=300)
	{
		HAL_UART_Transmit(&huart1,tat_bom_1b,32,5);
		//HAL_UART_Transmit(&huart1,tat_bom_2b,32,5);
		snprintf(nhiet_do_va_do_am, sizeof(nhiet_do_va_do_am), "C=%s.%s&H=%s.%s&motor=OFF_1b\r\n", nhiet_do1,nhiet_do2,do_am1,do_am2);	
		//snprintf(nhiet_do_va_do_am, sizeof(nhiet_do_va_do_am), "C=%s.%s&H=%s.%s&motor=OFF_2b\r\n", nhiet_do1,nhiet_do2,do_am1,do_am2);	
	}
	else
	{
		HAL_UART_Transmit(&huart1,loi_he_thong,32,5);
		snprintf(nhiet_do_va_do_am, sizeof(nhiet_do_va_do_am), "C=Loi&H=Loi&motor=OFF_2b\r\n");	
	}
	//----------------------------------------chuyen tu so sang chuoi------------------------------------------------------//	
}
//------------------------------------------------------------------ doc temperature va huminity ---------------------------------------------------//
		
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	DWT_Delay_Init ();
	
	
	
	NRF24L01_Config();
	CTR_nrfInit(); // khoi tao kenh 4, luong 3, ..
	CTR_nrfSetRX();
	// them doan ma can gui vao buffer gui
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{ 
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		
			
			a=0;
			b=0;
			thoi_gian_check=0;
			//thoi_gian_truyen=0;
			if(CTR_nrfGetPacket(chuoi)==1)
			{
				int chuoi_check = strcmp(chuoi, check_1a);
				//int chuoi_check = strcmp(chuoi, check_2a);
				if(chuoi_check == 0)
				{
					
					HAL_UART_Transmit(&huart1,chuoi,32,5);//xem chuoi nhan duoc tren pc
					memset(chuoi,0x00,32);//xoa chuoi
					read();
					while(a==0)//doi den khi da nhan duoc chuoi ok
					{
						//truyen du lieu qua cho master
						HAL_UART_Transmit(&huart1,dang_truyen,32,5);
						HAL_UART_Transmit(&huart1,nhiet_do_va_do_am,32,5);
						CTR_nrfSetTX();
						CTR_nrfSendPacket(nhiet_do_va_do_am);
						//cho phan hoi ok tu master
						//HAL_Delay(500);
						
						while(b==0)
						{
							thoi_gian_check+=1;
							HAL_UART_Transmit(&huart1,check_lai_lan_cuoi,32,5);
							memset(chuoi,0x00,32);//xoa chuoi
							CTR_nrfSetRX();
							if(CTR_nrfGetPacket(chuoi)==1)
							{
								int chuoi_ok = strcmp(chuoi, ok);
								if(chuoi_ok == 0)
								{
									HAL_UART_Transmit(&huart1,chuoi,32,5); 
									thoi_gian_check=4;
									a=1;//thoat ca vong
									b=1;//thoat che do check lai lan cuoi
									HAL_UART_Transmit(&huart1,ket_thuc,32,5);
								}
								else
								{
									HAL_UART_Transmit(&huart1,chuoi,32,5);
									HAL_UART_Transmit(&huart1,chuoi_loi,32,5);
									thoi_gian_check=6;
								}
								memset(chuoi,0x00,32);//xoa chuoi
							}
							if(thoi_gian_check>5)
							{
								a=1;//thoat ca vong
								b=1;
								HAL_UART_Transmit(&huart1,loi,32,5);
							}
						}
					}
				}
				memset(chuoi,0x00,32);//xoa chuoi
			}
			CTR_nrfSetRX();
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

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
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

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_TRANS_GPIO_Port, LED_TRANS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_TRANS_Pin */
  GPIO_InitStruct.Pin = LED_TRANS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_TRANS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : button_Pin */
  GPIO_InitStruct.Pin = button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(button_GPIO_Port, &GPIO_InitStruct);

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
