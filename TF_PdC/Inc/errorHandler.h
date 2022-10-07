/*******************************************************************************
* @file    errorHandler.h
* @author  Guillermo Caporaletti
* @brief   Functions to call in case of error.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ERROR_HANDLER_H
#define __ERROR_HANDLER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"  		/* <- HAL include */
#include "stm32f4xx_nucleo_144.h" 	/* <- BSP include */

/* Types ---------------------------------------------------------------------*/

/* Constants and macros ------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
void Error_Handler(void);
void Error_Handler_Init(void);

/* ---------------------------------------------------------------------------*/
#endif /* __ERROR_HANDLER_H */

/***************************************************************END OF FILE****/
