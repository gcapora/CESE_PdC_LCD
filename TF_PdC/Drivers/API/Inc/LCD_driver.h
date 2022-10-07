/*******************************************************************************
* @file    API_LCD.h
* @author  Guillermo Caporaletti
* @brief   Controlador de LCD alimentado por HD44780.
* ********************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx_hal.h"  		/* <- HAL include */
#include "stm32f4xx_nucleo_144.h" 	/* <- BSP include */
#include "errorHandler.h"

/* Types ---------------------------------------------------------------------*/

typedef enum {WRITE_MODE, READ_MODE} io_mode;

typedef struct {
	uint32_t rs_pin;
	uint32_t rw_pin;
	uint32_t enable_pin;
	uint16_t data_pins[8];

	GPIO_TypeDef* rs_port;
	GPIO_TypeDef* rw_port;
	GPIO_TypeDef* enable_port;
	GPIO_TypeDef* data_ports[8];

	uint8_t displayfunction;
	uint8_t displaycontrol;
	uint8_t displaymode;

	bool initialized;
	bool fourbitmode;
	io_mode rw_config;

	uint8_t numlines;
	uint8_t row_offsets[4];
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

// Modos de lectura/escritura de los pines
#define LCD_WRITE GPIO_MODE_OUTPUT_OD	// Modo para escritura en LCD
										// "Open drain" para compatibilidad con TTL 5V
#define LCD_READ GPIO_MODE_INPUT		// Modo para lectura del LCD

// Traducción de pines Arduino

// Puertos de los pines
#define ARDUINO_D0_port		GPIOG
#define ARDUINO_D1_port		GPIOG
#define ARDUINO_D2_port		GPIOF
#define ARDUINO_D3_port		GPIOE
#define ARDUINO_D4_port		GPIOF
#define ARDUINO_D5_port		GPIOE
#define ARDUINO_D6_port		GPIOE
#define ARDUINO_D7_port		GPIOF
#define ARDUINO_D8_port		GPIOF
#define ARDUINO_D9_port		GPIOD
#define ARDUINO_D10_port	GPIOD

// Pines dentro de cada puerto
#define ARDUINO_D0_pin		GPIO_PIN_9
#define ARDUINO_D1_pin		GPIO_PIN_14
#define ARDUINO_D2_pin		GPIO_PIN_15
#define ARDUINO_D3_pin		GPIO_PIN_13
#define ARDUINO_D4_pin		GPIO_PIN_14
#define ARDUINO_D5_pin		GPIO_PIN_11
#define ARDUINO_D6_pin		GPIO_PIN_9
#define ARDUINO_D7_pin		GPIO_PIN_13
#define ARDUINO_D8_pin		GPIO_PIN_12
#define ARDUINO_D9_pin		GPIO_PIN_15
#define ARDUINO_D10_pin		GPIO_PIN_14

// Macros varios
#define DISCONNECTED_PIN	NULL
#define MAX_COUNT			0xFFFF
#define LOW_COUNT			0x01FF

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
uint8_t LCD_data_read(void);
uint8_t LCD_address_read(void);
bool LCD_busy_flag(void);

// Funciones de bajo nivel (llamadas a funciones HAL)
void LCD_init_stm32f4xx(LCDconfig * LCD_a_configurar);
void LCD_write_mode(LCDconfig * LCD_a_escribir);
void LCD_read_mode(LCDconfig * LCD_a_leer);
void pinMode(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t Pin_Mode);
void digitalWrite(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
GPIO_PinState digitalRead(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void delayMilliseconds(uint32_t delay);
void delayMicro(uint8_t Sticks);

/* ---------------------------------------------------------------------------*/

#endif /* LCD_DRIVER_H */

/***************************************************************END OF FILE****/
