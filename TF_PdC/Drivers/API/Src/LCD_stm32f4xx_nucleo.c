/*******************************************************************************
* @file    LCD_stm32f4xx_nucleo.c
* @author  Guillermo Caporaletti
* @brief   Manejo de puertos y pines GPIO para NUCLEO-F429ZI
* 		   para manejo de LCD a través de chipset Hitachi HD44780 (o compatible)
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include <API_LCD.h>

/* Private typedef -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

// Puertos de los pines
#define D0_port GPIOF
#define D1_port GPIOE
#define D2_port GPIOE
#define D3_port GPIOF
#define D4_port GPIOF
#define D5_port GPIOE
#define D6_port GPIOE
#define D7_port GPIOF
#define ENABLE_port GPIOD
#define RW_port NULL
#define RS_port GPIOF

// Pines dentro de cada puerto
#define D0_pin GPIO_PIN_14
#define D1_pin GPIO_PIN_11
#define D2_pin GPIO_PIN_9
#define D3_pin GPIO_PIN_13
#define D4_pin GPIO_PIN_14
#define D5_pin GPIO_PIN_11
#define D6_pin GPIO_PIN_9
#define D7_pin GPIO_PIN_13
#define ENABLE_pin GPIO_PIN_15
#define RW_pin GPIO_PIN_15
#define RS_pin GPIO_PIN_12

// Características del LCD
#define fourbitmode true
#define cols 16
#define lines 2
#define dotsize LCD_5x8DOTS

/* Functions -----------------------------------------------------------------*/

/*******************************************************************************
  * @brief  Inicializa la estructura LCDconfig para lograr la conexión al LCD
  * 		y los pines de salida a nivel hardware.
  * @param  puntero a la estructura de LCD a configurar
  * @retval None
  */
void LCD_init_stm32f4xx(LCDconfig * LCD_a_configurar)
{
	// Asigno pines y puertos a la estructura del LCD
	LCD_a_configurar->_rs_pin = RS_pin;
	LCD_a_configurar->_rs_port = RS_port;

	LCD_a_configurar->_rw_pin = RW_pin;
	LCD_a_configurar->_rw_port = RW_port;

	LCD_a_configurar->_enable_pin = ENABLE_pin;
	LCD_a_configurar->_enable_port = ENABLE_port;

	LCD_a_configurar->_data_pins[0] = D0_pin;
	LCD_a_configurar->_data_pins[1] = D1_pin;
	LCD_a_configurar->_data_pins[2] = D2_pin;
	LCD_a_configurar->_data_pins[3] = D3_pin;
	LCD_a_configurar->_data_pins[4] = D4_pin;
	LCD_a_configurar->_data_pins[5] = D5_pin;
	LCD_a_configurar->_data_pins[6] = D6_pin;
	LCD_a_configurar->_data_pins[7] = D7_pin;

	LCD_a_configurar->_data_ports[0] = D0_port;
	LCD_a_configurar->_data_ports[1] = D1_port;
	LCD_a_configurar->_data_ports[2] = D2_port;
	LCD_a_configurar->_data_ports[3] = D3_port;
	LCD_a_configurar->_data_ports[4] = D4_port;
	LCD_a_configurar->_data_ports[5] = D5_port;
	LCD_a_configurar->_data_ports[6] = D6_port;
	LCD_a_configurar->_data_ports[7] = D7_port;

	// Configuro modo de conexión (4 pines o 8 pines)
	LCD_a_configurar->_fourbitmode = fourbitmode;
	if (fourbitmode == true) {
		LCD_a_configurar->_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	} else {
		LCD_a_configurar->_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
	}

	// Configuro número de líneas
	LCD_a_configurar->_numlines = lines;
	if (LCD_a_configurar->_numlines > 1) {
		LCD_a_configurar->_displayfunction |= LCD_2LINE;
	}

	// Para algunos displays de 1 línea, se puede configurar
	// altura de caracter de 10 pixeles
	if ((dotsize != LCD_5x8DOTS) && (lines == 1)) {
	    LCD_a_configurar->_displayfunction |= LCD_5x10DOTS;
	}

	// Offsets de filas (generalización para LCD de 4 filas)
	LCD_a_configurar->_row_offsets[0] = 0x00;
	LCD_a_configurar->_row_offsets[1] = 0x40;
	LCD_a_configurar->_row_offsets[2] = 0x00+cols;
	LCD_a_configurar->_row_offsets[3] = 0x40+cols;

	// Dejo asentado que almacené valores iniciales en la estructura
	LCD_a_configurar->_initialized = true;

	// Ahora opero sobre el hardware: activo los puertos
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	// Inicializamos los pines de salida RS, RW y ENABLE
	pinMode(LCD_a_configurar->_rs_port, LCD_a_configurar->_rs_pin, OUTPUT);
	if (LCD_a_configurar->_rw_port != NULL) {
		// Podemos ahorrar un pin no usando RW y conectándolo a GND.
		// Si hacemos esto, lo indicamos como NULL en el puerto.
	    pinMode(LCD_a_configurar->_rw_port, LCD_a_configurar->_rw_pin, OUTPUT);
	}
	pinMode(LCD_a_configurar->_enable_port, LCD_a_configurar->_enable_pin, OUTPUT);

	// Inicializamos los 8 o 4 bits de datos
	for (int8_t i=0; i<((LCD_a_configurar->_displayfunction & LCD_8BITMODE) ? 8 : 4); ++i)
	{
		pinMode(LCD_a_configurar->_data_ports[i], LCD_a_configurar->_data_pins[i], OUTPUT);
	}
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

	// Se debe ejecutar __HAL_RCC_GPIOA_CLK_ENABLE();
    // en otro lugar
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
  * @brief  Un retardo en milisegundos
  * @param  retardo
  * @retval None
  */
void delayMilliseconds(uint32_t delay)
{
	  HAL_Delay(delay);
}

/***************************************************************END OF FILE****/
