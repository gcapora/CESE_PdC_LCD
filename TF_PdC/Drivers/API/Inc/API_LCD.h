/*******************************************************************************
* @file    API_LCD.h
* @author  Guillermo Caporaletti
* @brief   Controlador para LCD a través del chipset HD44780.
********************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef API_LCD_H
#define API_LCD_H

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx_hal.h"  		/* <- HAL include */
#include "stm32f4xx_nucleo_144.h" 	/* <- BSP include */
#include "errorHandler.h"

/* Types ---------------------------------------------------------------------*/

typedef struct {
	uint32_t _rs_pin;
	uint32_t _rw_pin;
	uint32_t _enable_pin;
	uint16_t _data_pins[8];

	GPIO_TypeDef* _rs_port;
	GPIO_TypeDef* _rw_port;
	GPIO_TypeDef* _enable_port;
	GPIO_TypeDef* _data_ports[8];

	uint8_t _displayfunction;
	uint8_t _displaycontrol;
	uint8_t _displaymode;

	bool _initialized;
	bool _fourbitmode;

	uint8_t _numlines;
	uint8_t _row_offsets[4];
} LCDconfig;

/* Exported macro ------------------------------------------------------------*/

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// Constantes varias
#define OUTPUT GPIO_MODE_OUTPUT_OD	// Simplifica modos de pines
									// "Open drain" para compatibilidad con TTL 5V

/* Exported functions --------------------------------------------------------*/

// Comandos de alto nivel
void LCD_init();
void LCD_clear();
void LCD_home();
void LCD_setCursor(uint8_t, uint8_t);
void LCD_noDisplay();
void LCD_display();
void LCD_noCursor();
void LCD_cursor();
void LCD_noBlink();
void LCD_blink();
void LCD_scrollDisplayLeft();
void LCD_scrollDisplayRight();
void LCD_leftToRight();
void LCD_rightToLeft();
void LCD_autoscroll();
void LCD_noAutoscroll();
void LCD_createChar(uint8_t, uint8_t[]);
void LCD_print(char *);

// Funciones de nivel medio
void LCD_write(uint8_t);
void LCD_command(uint8_t);

// Funciones de bajo nivel (hard)
void LCD_init_stm32f4xx(LCDconfig * LCD_a_configurar);
void pinMode(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t Pin_Mode);
void digitalWrite(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void delayMilliseconds(uint32_t delay);

/* ---------------------------------------------------------------------------*/

#endif /* LCD_STM32F4XXNUCLEO_H */

/***************************************************************END OF FILE****/
