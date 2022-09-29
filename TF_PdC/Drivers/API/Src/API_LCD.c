/*******************************************************************************
* @file    API_LCD.c
* @author  Guillermo Caporaletti
* @brief   Implementación de instrucciones para LCD controlado por HD44780.
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include <API_LCD.h>

/* Types ---------------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static LCDconfig LCD44780;

/* Private function prototypes -----------------------------------------------*/

static void LCD_send(uint8_t value, uint8_t mode);
static void LCD_write4bits(uint8_t value);
static void LCD_write8bits(uint8_t value);
static void LCD_pulseEnable();

/* Functions -----------------------------------------------------------------*/

/*******************************************************************************
* @brief  Inicializa LCD
* @param  None
* @retval None
*/
void LCD_init() {
	// Configuro el hardware de la conexión con el LCD:
	LCD_init_stm32f4xx(&LCD44780);

	// Ver pp. 45-46 sobre las especificaciones de inicialización:
	// Lo primero a enviar es para establecer una conexión de 4 pines o de 8 pines.
	// Según la hoja de datos, debemos esperar más de 40ms antes de enviar comandos.
	delayMilliseconds(60);

	// Ahora reseteamos RS, RW y ENABLE para iniciar comandos
	digitalWrite(LCD44780._rs_port, LCD44780._rs_pin, GPIO_PIN_RESET);
	digitalWrite(LCD44780._enable_port, LCD44780._enable_pin, GPIO_PIN_RESET);
	if (LCD44780._rw_port != NULL) {
		// Quire decir que RW está conectado a un pinout (y no GND)
		digitalWrite(LCD44780._rw_port, LCD44780._rw_pin, GPIO_PIN_RESET);
	}

	// Establecemos modo 4 bit o 8 bit del LCD
	if ((LCD44780._displayfunction & LCD_8BITMODE) == false) {
	    // Según tengo almacenado en _displayfunction,
		// tengo sólo 4 pines de datos conectados.
		// Secuencia según figura 24, pg. 46:

	    // Indicamos 4 bit mode (y esperamos al menos 4.1ms)
		LCD_write4bits(0x03);
	    delayMilliseconds(5);

	    // Indicamos por segunda vez (y esperamos al menos 100us)
	    LCD_write4bits(0x03);
	    delayMilliseconds(1);

	    // Indicamos por tercera vez!
	    LCD_write4bits(0x03);
	    delayMilliseconds(1);

	    // Configuramos 4-bit:
	    LCD_write4bits(0x02);
	} else {
	    // Tengo 8 pines de datos.
		// Secuencia según pg. 45, figura 23:

	    // Indicamos 8 bit mode (y esperamos al menos 4.1ms)
	    LCD_command(LCD_FUNCTIONSET | LCD44780._displayfunction);
	    delayMilliseconds(6);

	    // Segunda vez (al meos 100us)
	    LCD_command(LCD_FUNCTIONSET | LCD44780._displayfunction);
	    delayMilliseconds(1);

	    // Tercera
	    LCD_command(LCD_FUNCTIONSET | LCD44780._displayfunction);
	 }

	 // Finalmente, enviamos comandos:
	 // Establecemos número de líneas, tamaño de fuente, etc. (con display off)
	 LCD_command(LCD_FUNCTIONSET | LCD44780._displayfunction);

	 // turn the display on with no cursor or blinking default (ahora display on)
	 LCD44780._displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	 LCD_command(LCD_DISPLAYCONTROL | LCD44780._displaycontrol);

	 // Borramos pantalla
	 LCD_clear();

	 // Initialize to default text direction (for romance languages)
	 LCD44780._displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	 LCD_command(LCD_ENTRYMODESET | LCD44780._displaymode);

}

/*******************************************************************************
* @brief  Borra pantalla y posiciona el cursor en 0
* @param  None
* @retval None
*/
void LCD_clear() {
	 LCD_command(LCD_CLEARDISPLAY);
	 delayMilliseconds(2);
}

/*******************************************************************************
* @brief  Posiciona el cursor en 0
* @param  None
* @retval None
*/
void LCD_home() {
	 LCD_command(LCD_RETURNHOME);
	 delayMilliseconds(2);
}

/*******************************************************************************
* @brief  Ubica al cursor
* @param  Columna y Fila
* @retval None
*/
void LCD_setCursor(uint8_t col, uint8_t row)
{
  const size_t max_lines = sizeof(LCD44780._row_offsets) / sizeof(LCD44780._row_offsets[0]);
  if ( row >= max_lines ) {
    row = max_lines - 1;    // we count rows starting w/0
  }
  if ( row >= LCD44780._numlines ) {
    row = LCD44780._numlines - 1;    // we count rows starting w/0
  }
  LCD_command(LCD_SETDDRAMADDR | (col + LCD44780._row_offsets[row]));
}

/*******************************************************************************
* @brief  Apaga y prende display
* @param  None
* @retval None
*/
void LCD_noDisplay() {
  LCD44780._displaycontrol &= ~LCD_DISPLAYON;
  LCD_command(LCD_DISPLAYCONTROL | LCD44780._displaycontrol);
}
void LCD_display() {
  LCD44780._displaycontrol |= LCD_DISPLAYON;
  LCD_command(LCD_DISPLAYCONTROL | LCD44780._displaycontrol);
}

/*******************************************************************************
* @brief  Saca o pone cursor
* @param  None
* @retval None
*/
void LCD_noCursor() {
	LCD44780._displaycontrol &= ~LCD_CURSORON;
	LCD_command(LCD_DISPLAYCONTROL | LCD44780._displaycontrol);
}
void LCD_cursor() {
	LCD44780._displaycontrol |= LCD_CURSORON;
	LCD_command(LCD_DISPLAYCONTROL | LCD44780._displaycontrol);
}

/*******************************************************************************
* @brief  Prende y apaga parpadeo de cursor
* @param  None
* @retval None
*/
void LCD_noBlink() {
	LCD44780._displaycontrol &= ~LCD_BLINKON;
  LCD_command(LCD_DISPLAYCONTROL | LCD44780._displaycontrol);
}
void LCD_blink() {
  LCD44780._displaycontrol |= LCD_BLINKON;
  LCD_command(LCD_DISPLAYCONTROL | LCD44780._displaycontrol);
}

/*******************************************************************************
* @brief  Scroll el display sin cambiar la RAM
* @param  None
* @retval None
*/
void LCD_scrollDisplayLeft(void) {
  LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LCD_scrollDisplayRight(void) {
  LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

/*******************************************************************************
* @brief  Texto de izquierda a derecha y viceversa
* @param  None
* @retval None
*/
void LCD_leftToRight(void) {
  LCD44780._displaymode |= LCD_ENTRYLEFT;
  LCD_command(LCD_ENTRYMODESET | LCD44780._displaymode);
}
void LCD_rightToLeft(void) {
	LCD44780._displaymode &= ~LCD_ENTRYLEFT;
	LCD_command(LCD_ENTRYMODESET | LCD44780._displaymode);
}

/*******************************************************************************
* @brief  Activa y desactiva autoscroll
* @param  None
* @retval None
*/
void LCD_autoscroll(void) {
  LCD44780._displaymode |= LCD_ENTRYSHIFTINCREMENT;
  LCD_command(LCD_ENTRYMODESET | LCD44780._displaymode);
}
void LCD_noAutoscroll(void) {
	LCD44780._displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  LCD_command(LCD_ENTRYMODESET | LCD44780._displaymode);
}

/*******************************************************************************
* @brief  Crear caracter
* @param  direccion y mapa del caracter
* @retval None
*/
void LCD_createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  LCD_command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    LCD_write(charmap[i]);
  }
}

/*******************************************************************************
* @brief  Envía una cadena de caracteres
* @param  Cadena de caracteres
* @retval None
*/
void LCD_print(char * Cadena) {
	size_t Largo = strlen(Cadena);
	for (size_t i=0; i<Largo; i++) {
		LCD_write((uint8_t) Cadena[i]);
	}
}

/* Funciones nivel medio -----------------------------------------------------*/

/*******************************************************************************
* @brief  Envía un dato al LCD
* @param  Dato de 8 bits
* @retval None
*/
void LCD_write(uint8_t value) {
  LCD_send(value, GPIO_PIN_SET);
  // return true; // assume sucess
}

/*******************************************************************************
* @brief  Envía un comando al LCD
* @param  Comando
* @retval None
*/
void LCD_command(uint8_t value) {
	LCD_send(value, GPIO_PIN_RESET);
}

/*******************************************************************************
* @brief  Envía comando o dato, con 8 o 4 pines conectados
* @param  Puntero a LCD, valor a enviar y modo (comando o dato)
* @retval None
*/
static void LCD_send(uint8_t value, uint8_t mode) {
  digitalWrite(LCD44780._rs_port, LCD44780._rs_pin, (GPIO_PinState) mode);

  // Si RW está, lo bajamos para escribir
  if (LCD44780._rw_port != NULL) {
	digitalWrite(LCD44780._rw_port, LCD44780._rw_pin, GPIO_PIN_RESET);
  }

  // Veo si mando de a 4 bits o de a 8 bits
  if (LCD44780._displayfunction & LCD_8BITMODE) {
	LCD_write8bits(value);
  } else {
    LCD_write4bits(value>>4);
    LCD_write4bits(value);
  }
}

/*******************************************************************************
* @brief  Manda un pulso de lectura "enable"
* @param  None
* @retval None
*/
static void LCD_pulseEnable() {
  digitalWrite(LCD44780._enable_port, LCD44780._enable_pin, GPIO_PIN_RESET);
  delayMilliseconds(1); // debería ser 1us

  digitalWrite(LCD44780._enable_port, LCD44780._enable_pin, GPIO_PIN_SET);
  delayMilliseconds(1); // debería ser > 450ns

  digitalWrite(LCD44780._enable_port, LCD44780._enable_pin, GPIO_PIN_RESET);
  delayMilliseconds(1); // debería ser > 37us
}

/*******************************************************************************
* @brief  Escribe valor en 4 pines (en dos veces)
* @param  Valor
* @retval None
*/
static void LCD_write4bits(uint8_t value) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(LCD44780._data_ports[i], LCD44780._data_pins[i], (value >> i) & 0x01);
  }
  LCD_pulseEnable();
}

/*******************************************************************************
* @brief  Escribe valor en los 8 pines
* @param  Valor
* @retval None
*/
static void LCD_write8bits(uint8_t value) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(LCD44780._data_ports[i], LCD44780._data_pins[i], (value >> i) & 0x01);
  }

  LCD_pulseEnable();
}

/***************************************************************END OF FILE****/
