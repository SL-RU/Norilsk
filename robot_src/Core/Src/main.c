/* USER CODE BEGIN Header */
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
* COPYRIGHT(c) 2018 STMicroelectronics
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nrf24.h"
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
#define FORW 1
#define BACK 2
#define STOP 0
void mot(uint8_t l, uint8_t r)
{
    //left
    HAL_GPIO_WritePin(ML_P_GPIO_Port, ML_P_Pin,
                      (l == FORW) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ML_N_GPIO_Port, ML_N_Pin,
                      (l == BACK) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    //right
    HAL_GPIO_WritePin(MR_P_GPIO_Port, MR_P_Pin,
                      (r == FORW) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MR_N_GPIO_Port, MR_N_Pin,
                      (r == BACK) ? GPIO_PIN_SET : GPIO_PIN_RESET);

}
#define D_Read(x) HAL_GPIO_ReadPin(x ## _GPIO_Port, x ## _Pin) == GPIO_PIN_SET
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
    //CDC_Transmit_FS((uint8_t*)ptr, len);
    HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, 10);
    return len;
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
    MX_SPI1_Init();
    MX_USART1_UART_Init();
    MX_TIM4_Init();
    /* USER CODE BEGIN 2 */
    printf("Hello!\n");
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
    mot(STOP, STOP);
    HAL_Delay(2000);

    // Buffer to store a payload of maximum width
    uint8_t nRF24_payload[32];

// Pipe number
    nRF24_RXResult pipe;

// Length of received payload
    uint8_t payload_length;
    NRF24 n;
    printf("Hello, norilsk\n");
    nRF24_Init(&n, &hspi1,
               NRF_CS_GPIO_Port, NRF_CS_Pin,
               NRF_SCN_GPIO_Port, NRF_SCN_Pin,
               NRF_IRQ_GPIO_Port, NRF_IRQ_Pin);
    printf("nrf24 inited\n");
    if(nRF24_Check(&n))
        printf("nrf24 OK\n");
    else
        printf("nrf24 ERR\n");


    // Disable ShockBurst for all RX pipes
    nRF24_DisableAA(&n, 0xFF);
    // Set RF channel
    nRF24_SetRFChannel(&n, 115);
    // Set data rate
    nRF24_SetDataRate(&n, nRF24_DR_250kbps);
    // Set CRC scheme
    nRF24_SetCRCScheme(&n, nRF24_CRC_2byte);
    // Set address width, its common for all pipes (RX and TX)
    nRF24_SetAddrWidth(&n, 3);
    // Configure RX PIPE#1
    static const uint8_t nRF24_ADDR[] = { 'p', 'u', 'l' };
    nRF24_SetAddr(&n, nRF24_PIPE1, nRF24_ADDR); // program address for RX pipe #1
    nRF24_SetRXPipe(&n, nRF24_PIPE1, nRF24_AA_OFF, 5); // Auto-ACK: disabled, payload length: 5 bytes

    // Set operational mode (PRX == receiver)
    nRF24_SetOperationalMode(&n, nRF24_MODE_RX);

    // Wake the transceiver
    nRF24_SetPowerMode(&n, nRF24_PWR_UP);

    // Put the transceiver to the RX mode
    nRF24_toRX(&n);

    // The main loop
    while (1) {
        //
        // Constantly poll the status of the RX FIFO and get a payload if FIFO is not empty
        //
        // This is far from best solution, but it's ok for testing purposes
        // More smart way is to use the IRQ pin :)
        //
        if (nRF24_GetStatus_RXFIFO(&n) != nRF24_STATUS_RXFIFO_EMPTY) {
            // Get a payload from the transceiver
            pipe = nRF24_ReadPayload(&n, nRF24_payload, &payload_length);

            // Clear all pending IRQ flags
            nRF24_ClearIRQFlags(&n);

            // Print a payload contents to UART
            printf("recieved %d - %d %d\n", pipe, nRF24_payload[0], nRF24_payload[1]);
        }
    }

  
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
      
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        TIM4->CCR4 = 250;
        printf("%d %d %d %d\n",
               D_Read(D_FL),
               D_Read(D_FR),
               D_Read(D_BL),
               D_Read(D_BR));
        HAL_Delay(500);
        /* for(int i = 250 ; i <= 750; i++) */
        /* { */
        /*     TIM4->CCR4 = i; */
        /*     HAL_Delay(1); */
        /* } */
        /* HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin); */
        /* for(int i = 750; i >= 250; i--) */
        /* { */
        /*     TIM4->CCR4 = i; */
        /*     HAL_Delay(1); */
        /* } */
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

    /**Initializes the CPU, AHB and APB busses clocks 
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
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
