/*******************************************************************************
* @file    LCD_stm32f4xx_nucleo.c
* @author  Guillermo Caporaletti
* @brief   Manejo de puertos y pines GPIO para NUCLEO-F429ZI
* 		   para controlador de LCD alimentado por Hitachi HD44780 (o compatible)
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include <LCD_driver.h>

/* Private typedef -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

// Puertos de los pines
#define D0_port		ARDUINO_D0_port
#define D1_port		ARDUINO_D1_port
#define D2_port		ARDUINO_D2_port
#define D3_port		ARDUINO_D3_port
#define D4_port		ARDUINO_D4_port
#define D5_port		ARDUINO_D5_port
#define D6_port		ARDUINO_D6_port
#define D7_port		ARDUINO_D7_port
#define ENABLE_port	ARDUINO_D8_port
#define RW_port		ARDUINO_D9_port
#define RS_port		ARDUINO_D10_port

// Pines dentro de cada puerto
#define D0_pin		ARDUINO_D0_pin
#define D1_pin		ARDUINO_D1_pin
#define D2_pin		ARDUINO_D2_pin
#define D3_pin		ARDUINO_D3_pin
#define D4_pin		ARDUINO_D4_pin
#define D5_pin		ARDUINO_D5_pin
#define D6_pin		ARDUINO_D6_pin
#define D7_pin		ARDUINO_D7_pin
#define ENABLE_pin	ARDUINO_D8_pin
#define RW_pin		ARDUINO_D9_pin
#define RS_pin		ARDUINO_D10_pin

// Características del LCD
#define LCD_FOURBITMODE	false
#define LCD_COLUMNS		16
#define LCD_LINES		2
#define LCD_DOT_SIZE	LCD_5x8DOTS

/*******************************************************************************
  * @brief  Inicializa la estructura LCDconfig para lograr la conexión al LCD
  * 		y los pines de salida a nivel hardware.
  * @param  puntero a la estructura de LCD a configurar
  * @retval None
  */
void LCD_init_stm32f4xx(LCDconfig * LCD_a_configurar)
{
	// Asigno pines y puertos a la estructura del LCD
	LCD_a_configurar->rs_pin = RS_pin;
	LCD_a_configurar->rs_port = RS_port;

	LCD_a_configurar->rw_pin = RW_pin;
	LCD_a_configurar->rw_port = RW_port;

	LCD_a_configurar->enable_pin = ENABLE_pin;
	LCD_a_configurar->enable_port = ENABLE_port;

	LCD_a_configurar->data_pins[0] = D0_pin;
	LCD_a_configurar->data_pins[1] = D1_pin;
	LCD_a_configurar->data_pins[2] = D2_pin;
	LCD_a_configurar->data_pins[3] = D3_pin;
	LCD_a_configurar->data_pins[4] = D4_pin;
	LCD_a_configurar->data_pins[5] = D5_pin;
	LCD_a_configurar->data_pins[6] = D6_pin;
	LCD_a_configurar->data_pins[7] = D7_pin;

	LCD_a_configurar->data_ports[0] = D0_port;
	LCD_a_configurar->data_ports[1] = D1_port;
	LCD_a_configurar->data_ports[2] = D2_port;
	LCD_a_configurar->data_ports[3] = D3_port;
	LCD_a_configurar->data_ports[4] = D4_port;
	LCD_a_configurar->data_ports[5] = D5_port;
	LCD_a_configurar->data_ports[6] = D6_port;
	LCD_a_configurar->data_ports[7] = D7_port;

	// Configuro modo de conexión (4 pines o 8 pines)
	LCD_a_configurar->fourbitmode = LCD_FOURBITMODE;
	if (LCD_FOURBITMODE == true) {
		LCD_a_configurar->displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	} else {
		LCD_a_configurar->displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
	}

	// Configuro número de líneas
	LCD_a_configurar->numlines = LCD_LINES;
	if (LCD_a_configurar->numlines > 1) {
		LCD_a_configurar->displayfunction |= LCD_2LINE;
	}

	// Para algunos displays de 1 línea, se puede configurar
	// altura de caracter de 10 pixeles
	if ((LCD_DOT_SIZE != LCD_5x8DOTS) && (LCD_LINES == 1)) {
	    LCD_a_configurar->displayfunction |= LCD_5x10DOTS;
	}

	// Offsets de filas (generalización para LCD de 4 filas)
	LCD_a_configurar->row_offsets[0] = 0x00;
	LCD_a_configurar->row_offsets[1] = 0x40;
	LCD_a_configurar->row_offsets[2] = 0x00+LCD_COLUMNS;
	LCD_a_configurar->row_offsets[3] = 0x40+LCD_COLUMNS;

	// Dejo asentado que almacené valores iniciales en la estructura
	LCD_a_configurar->initialized = true;

	// Ahora opero sobre el hardware: activo los puertos
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	// Inicializamos los pines de salida RS, RW y ENABLE
	pinMode(LCD_a_configurar->rs_port, LCD_a_configurar->rs_pin, LCD_WRITE);
	if (LCD_a_configurar->rw_port != NULL) {
		// Podemos ahorrar un pin no usando RW y conectándolo a GND.
		// Si hacemos esto, lo indicamos como NULL en el puerto.
	    pinMode(LCD_a_configurar->rw_port, LCD_a_configurar->rw_pin, LCD_WRITE);
	}
	pinMode(LCD_a_configurar->enable_port, LCD_a_configurar->enable_pin, LCD_WRITE);

    // Configuramos a los pines de datos en modo escritura
	LCD_write_mode(LCD_a_configurar);

}

/*******************************************************************************
  * @brief  Configura los pines de datos en modo escritura.
  * @param	Estructura del LCD.
  * @retval None
  * @note
  */
void LCD_write_mode(LCDconfig * LCD_a_escribir)
{
	// Seteamos los 8 o 4 bits de datos en modo escritura
	for (int8_t i=0; i<((LCD_a_escribir->displayfunction & LCD_8BITMODE) ? 8 : 4); ++i)
	{
		pinMode(LCD_a_escribir->data_ports[i], LCD_a_escribir->data_pins[i], LCD_WRITE);
	}
	LCD_a_escribir->rw_config = WRITE_MODE;
}

/*******************************************************************************
  * @brief  Configura los pines de datos en modo lectura.
  * @param	Estructura del LCD.
  * @retval None
  * @note
  */
void LCD_read_mode(LCDconfig * LCD_a_leer)
{
	// Seteamos los 8 o 4 bits de datos en modo lectura
	for (int8_t i=0; i<((LCD_a_leer->displayfunction & LCD_8BITMODE) ? 8 : 4); ++i)
	{
		pinMode(LCD_a_leer->data_ports[i], LCD_a_leer->data_pins[i], LCD_READ);
	}
	LCD_a_leer->rw_config = READ_MODE;
}

/*******************************************************************************
  * @brief  Configura el modo de un pin de salida individual.
  * @param	Puerto, Pin del puerto y Modo.
  * @retval None
  * @note	Debe habilitarse el puerto correspondiente.
  */
void pinMode(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t Pin_Mode)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = Pin_Mode;
	GPIO_InitStruct.Pull = GPIO_NOPULL;			// <- Configuración predeterminada
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;	// <- Configuración predeterminada

	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

	// ¡ATENCION!
	// En algún otro lugar, se debe activar el CLK del puerto ejecutando:
	// __HAL_RCC_GPIOA_CLK_ENABLE();
}

/*******************************************************************************
  * @brief  Escribe en un pin o pines de un puerto.
  * @param  Puerto, Pin del puerto y Estado de salida.
  * @retval None
  */
void digitalWrite(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState);
}

/*******************************************************************************
  * @brief  Lee un pin o pines de un puerto.
  * @param  Puerto, Pin del puerto y Estado de salida.
  * @retval None
  */
GPIO_PinState digitalRead(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	GPIO_PinState Lectura;
	Lectura = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
	return Lectura;
}

/*******************************************************************************
  * @brief  Un retardo en milisegundos
  * @param  retardo
  * @retval None
  */
void delayMilliseconds(uint32_t delay)
{
	  HAL_Delay(delay);
}

/*******************************************************************************
  * @brief  Un micro retardo
  * @param  Sticks...
  * @retval None
  */
void delayMicro(uint8_t Sticks) {
	if (Sticks==0) Sticks=1;
	for (uint8_t i=0; i<Sticks; i++) {
		for (uint16_t j=0; j<LOW_COUNT; j++) {
			// Keep on walking!
		}
	}
}



/***************************************************************END OF FILE****/
