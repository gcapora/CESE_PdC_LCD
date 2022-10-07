/*******************************************************************************
* @file    errorHandler.c
* @author  Guillermo Caporaletti
* @brief   Functions to call in case of error.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "errorHandler.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define y const ----------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/*******************************************************************************
* @brief  This function is executed in case of error occurrence.
* @param  None
* @retval None
*/
void Error_Handler_Init(void)
{
  /* Inicializa el led de alarma
   * para garantizar que esté operativo */
  BSP_LED_Init(LED3);
}

/*******************************************************************************
* @brief  This function is executed in case of error occurrence.
* @param  None
* @retval None
*/
void Error_Handler(void)
{
  /* Turn LED2 on */
  BSP_LED_Off(LED1);
  BSP_LED_Off(LED2);
  BSP_LED_Off(LED3);
  while (1)
  {
	  BSP_LED_Toggle(LED3);
	  HAL_Delay(100);
  }
}

//******************************************************************************
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/***************************************************************END OF FILE****/
