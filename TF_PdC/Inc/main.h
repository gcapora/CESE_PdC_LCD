/*******************************************************************************
* @file    main.h
* @author  Guillermo Caporlaetti
* @brief   Programa de prueba de LCD_driver.h
*          CESE, 18Co, FIUBA, octubre de 2022
*
* @detail  El programa envia mensajes a la pantalla LCD y a la conexión UART,
*          la cual debe estar conectada a una terminal. Luego de unos mensajes
*          iniciales, envía una cuenta que se actualiza permanentemente en LCD.
*          Presionando el botón de usuario, se lee el contenido de pantalla y
*          se envía a la terminal vía UART.
********************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/

#include <LCD_driver.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"  		/* <- HAL include */
#include "stm32f4xx_nucleo_144.h" 	/* <- BSP include */
#include "API_delay.h"
#include "API_debounce.h"
#include "errorHandler.h"
#include "API_uart.h"

/* Types ---------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Definition for USARTx clock resources */
#define USARTx                           USART3
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define USARTx_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_8
#define USARTx_TX_GPIO_PORT              GPIOD
#define USARTx_TX_AF                     GPIO_AF7_USART3
#define USARTx_RX_PIN                    GPIO_PIN_9
#define USARTx_RX_GPIO_PORT              GPIOD
#define USARTx_RX_AF                     GPIO_AF7_USART3

/* Exported functions ------------------------------------------------------- */

void SystemClock_Config(void);

/* -------------------------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
