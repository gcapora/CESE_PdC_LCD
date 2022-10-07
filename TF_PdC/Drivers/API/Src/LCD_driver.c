/*******************************************************************************
* @file    API_LCD.c
* @author  Guillermo Caporaletti
* @brief   Controlador de LCD alimentado por HD44780.
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include <LCD_driver.h>

/* Types ---------------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static LCDconfig miLCD;

/* Private function prototypes -----------------------------------------------*/

static void LCD_send(uint8_t value, uint8_t mode);
static void LCD_write4bits(uint8_t value);
static void LCD_write8bits(uint8_t value);
static uint8_t LCD_receive(uint8_t Registro);
static uint8_t LCD_read4bits(void);
static uint8_t LCD_read8bits(void);
static void LCD_pulseEnable();

/* Functions -----------------------------------------------------------------*/

/*******************************************************************************
* @brief  Inicializa LCD
* @param  None
* @retval None
*/
void LCD_init() {
	// Configuro el hardware de la conexión con el LCD:
	LCD_init_stm32f4xx(&miLCD);

	// Ver pp. 45-46 sobre las especificaciones de inicialización:
	// Lo primero a enviar es para establecer una conexión de 4 pines o de 8 pines.
	// Según la hoja de datos, debemos esperar más de 40ms antes de enviar comandos.
	delayMilliseconds(50);

	// Ahora reseteamos RS, RW y ENABLE para iniciar comandos
	digitalWrite(miLCD.rs_port, miLCD.rs_pin, GPIO_PIN_RESET);
	digitalWrite(miLCD.enable_port, miLCD.enable_pin, GPIO_PIN_RESET);
	if (miLCD.rw_port != DISCONNECTED_PIN) {
		// Quiere decir que RW está conectado a un pinout (y no GND)
		digitalWrite(miLCD.rw_port, miLCD.rw_pin, GPIO_PIN_RESET);
	}

	// Establecemos modo 4 bit o 8 bit del LCD
	if ((miLCD.displayfunction & LCD_8BITMODE) == false) {
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
	    LCD_command(LCD_FUNCTIONSET | miLCD.displayfunction);
	    delayMilliseconds(6);

	    // Segunda vez (al meos 100us)
	    LCD_command(LCD_FUNCTIONSET | miLCD.displayfunction);
	    delayMilliseconds(1);

	    // Tercera
	    LCD_command(LCD_FUNCTIONSET | miLCD.displayfunction);
	 }

	 // Finalmente, enviamos comandos:
	 // Establecemos número de líneas, tamaño de fuente, etc. (con display off)
	 LCD_command(LCD_FUNCTIONSET | miLCD.displayfunction);

	 // turn the display on with no cursor or blinking default (ahora display on)
	 miLCD.displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	 LCD_command(LCD_DISPLAYCONTROL | miLCD.displaycontrol);

	 // Borramos pantalla
	 LCD_clear();

	 // Initialize to default text direction (for romance languages)
	 miLCD.displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	 LCD_command(LCD_ENTRYMODESET | miLCD.displaymode);

}

/*******************************************************************************
* @brief  Borra pantalla y posiciona el cursor en 0
* @param  None
* @retval None
*/
void LCD_clear() {
	 LCD_command(LCD_CLEARDISPLAY);
	 for (uint16_t i=0; (i<MAX_COUNT && LCD_busy_flag()); i++) {
		 // Just keep on walking...
	 }
}

/*******************************************************************************
* @brief  Posiciona el cursor en 0
* @param  None
* @retval None
*/
void LCD_home() {
	 LCD_command(LCD_RETURNHOME);
	 for (uint16_t i=0; (i<MAX_COUNT && LCD_busy_flag()); i++) {}
}

/*******************************************************************************
* @brief  Ubica al cursor
* @param  Columna y Fila
* @retval None
*/
void LCD_setCursor(uint8_t col, uint8_t row)
{
  const size_t max_lines = sizeof(miLCD.row_offsets) / sizeof(miLCD.row_offsets[0]);
  if ( row >= max_lines ) {
    row = max_lines - 1;    // we count rows starting w/0
  }
  if ( row >= miLCD.numlines ) {
    row = miLCD.numlines - 1;    // we count rows starting w/0
  }
  LCD_command(LCD_SETDDRAMADDR | (col + miLCD.row_offsets[row]));
}

/*******************************************************************************
* @brief  Apaga y prende display
* @param  None
* @retval None
*/
void LCD_noDisplay() {
  miLCD.displaycontrol &= ~LCD_DISPLAYON;
  LCD_command(LCD_DISPLAYCONTROL | miLCD.displaycontrol);
}
void LCD_display() {
  miLCD.displaycontrol |= LCD_DISPLAYON;
  LCD_command(LCD_DISPLAYCONTROL | miLCD.displaycontrol);
}

/*******************************************************************************
* @brief  Saca o pone cursor
* @param  None
* @retval None
*/
void LCD_noCursor() {
	miLCD.displaycontrol &= ~LCD_CURSORON;
	LCD_command(LCD_DISPLAYCONTROL | miLCD.displaycontrol);
}
void LCD_cursor() {
	miLCD.displaycontrol |= LCD_CURSORON;
	LCD_command(LCD_DISPLAYCONTROL | miLCD.displaycontrol);
}

/*******************************************************************************
* @brief  Prende y apaga parpadeo de cursor
* @param  None
* @retval None
*/
void LCD_noBlink() {
	miLCD.displaycontrol &= ~LCD_BLINKON;
  LCD_command(LCD_DISPLAYCONTROL | miLCD.displaycontrol);
}
void LCD_blink() {
  miLCD.displaycontrol |= LCD_BLINKON;
  LCD_command(LCD_DISPLAYCONTROL | miLCD.displaycontrol);
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
  miLCD.displaymode |= LCD_ENTRYLEFT;
  LCD_command(LCD_ENTRYMODESET | miLCD.displaymode);
}
void LCD_rightToLeft(void) {
  miLCD.displaymode &= ~LCD_ENTRYLEFT;
  LCD_command(LCD_ENTRYMODESET | miLCD.displaymode);
}

/*******************************************************************************
* @brief  Activa y desactiva autoscroll
* @param  None
* @retval None
*/
void LCD_autoscroll(void) {
  miLCD.displaymode |= LCD_ENTRYSHIFTINCREMENT;
  LCD_command(LCD_ENTRYMODESET | miLCD.displaymode);
}
void LCD_noAutoscroll(void) {
  miLCD.displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  LCD_command(LCD_ENTRYMODESET | miLCD.displaymode);
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
* @brief  Lee el registro de dirección y Busy flag desde el LCD
* @param  None
* @retval Registro de instrucción
*/
uint8_t LCD_address_read(void) {
	return LCD_receive(GPIO_PIN_RESET);
}

/*******************************************************************************
* @brief  Lee el registro de dirección y Busy flag desde el LCD
* @param  None
* @retval Registro de instrucción
*/
uint8_t LCD_data_read(void) {
	return LCD_receive(GPIO_PIN_SET);
}

/*******************************************************************************
* @brief  Envía comando o dato, con 8 o 4 pines conectados
* @param  Puntero a LCD, valor a enviar y modo (comando o dato)
* @retval None
*/
static void LCD_send(uint8_t value, uint8_t mode) {
  digitalWrite(miLCD.rs_port, miLCD.rs_pin, (GPIO_PinState) mode);

  // Si RW está, lo bajamos para escribir
  if (miLCD.rw_port != DISCONNECTED_PIN) {
	digitalWrite(miLCD.rw_port, miLCD.rw_pin, GPIO_PIN_RESET);
  }

  // Debo poner pines de datos en modo escritura...
  if (miLCD.rw_config != WRITE_MODE) LCD_write_mode(&miLCD);

  // Veo si mando de a 4 bits o de a 8 bits
  if (miLCD.displayfunction & LCD_8BITMODE) {
	LCD_write8bits(value);
  } else {
    LCD_write4bits(value>>4);
    LCD_write4bits(value);
  }
}

/*******************************************************************************
* @brief  Lee registro de Instrucción o Dato
* @param  Puntero a LCD, valor a enviar y modo (comando o dato)
* @retval Byte leído
*/
static uint8_t LCD_receive(uint8_t Registro) {
	uint8_t LecturaByte = 0;

	// Primero verifico que el pin RW esté conectado:
	if (miLCD.rw_port == NULL) Error_Handler();					// <-- No está conectado!!!
	digitalWrite(miLCD.rw_port, miLCD.rw_pin, GPIO_PIN_SET);	// <-- Modo lectura.

	// Luego selecciono el registro Instrucción o Dato
	digitalWrite(miLCD.rs_port, miLCD.rs_pin, (GPIO_PinState) Registro);

	// Debo poner pines de datos en modo lectura...
	if (miLCD.rw_config != READ_MODE) LCD_read_mode(&miLCD);

	// Leo los pines: evalúo si leo de a 4 bits o de a 8 bits
	if (miLCD.fourbitmode == true) {
 	  LecturaByte =  LCD_read4bits() << 4;
      LecturaByte |= LCD_read4bits();
	} else {
	  LecturaByte = LCD_read8bits();
	}

	// Listo!!!
	return LecturaByte;
}

/*******************************************************************************
* @brief  Lee BUSY FLAG
* @param  NONE
* @retval Estado de BUSY FLAG
*/
bool LCD_busy_flag(void) {
	bool Lectura = true;

	// Primero verifico que el pin RW esté conectado y pongo en lectura:
	if (miLCD.rw_port == DISCONNECTED_PIN) return Lectura;	// Como no sé si está ocupado, devuelvo OCUPADO
	digitalWrite(miLCD.rw_port, miLCD.rw_pin, GPIO_PIN_SET);	// <-- Modo lectura.

	// Luego selecciono el registro de ADDRESS y BUSY FLAG
	digitalWrite(miLCD.rs_port, miLCD.rs_pin, GPIO_PIN_RESET);

	// Debo poner pines de datos en modo lectura...
	if (miLCD.rw_config != READ_MODE) LCD_read_mode(&miLCD);

	// Activo ENABLE y leo EL PIN de busy flag:
    digitalWrite(miLCD.enable_port, miLCD.enable_pin, GPIO_PIN_SET);		// <-- Flanco ascendente de ENABLE
	if (miLCD.fourbitmode == true) {
		// Leo en modo 4 pines
		Lectura = (bool) digitalRead(miLCD.data_ports[3], miLCD.data_pins[3]);
		// Mando pulso para saltear siguiente lectura
		digitalWrite(miLCD.enable_port, miLCD.enable_pin, GPIO_PIN_RESET);
	    digitalWrite(miLCD.enable_port, miLCD.enable_pin, GPIO_PIN_SET);
	} else {
		// Leo en modo 8 pines
		Lectura = (bool) digitalRead(miLCD.data_ports[7], miLCD.data_pins[7]);
	}
    digitalWrite(miLCD.enable_port, miLCD.enable_pin, GPIO_PIN_RESET);		// <-- Flanco descendente de ENABLE

	// Listo!!!
	return Lectura;
}

/*******************************************************************************
* @brief  Manda un pulso de lectura "enable"
* @param  None
* @retval None
*/
static void LCD_pulseEnable() {
  digitalWrite(miLCD.enable_port, miLCD.enable_pin, GPIO_PIN_RESET);
  delayMicro(1);	// debería ser 1us

  digitalWrite(miLCD.enable_port, miLCD.enable_pin, GPIO_PIN_SET);
  delayMicro(10);	// debería ser > 450ns

  digitalWrite(miLCD.enable_port, miLCD.enable_pin, GPIO_PIN_RESET);
  delayMicro(1);	// debería ser > 37us
}

/*******************************************************************************
* @brief  Escribe valor en 4 pines (en dos veces)
* @param  Valor
* @retval None
*/
static void LCD_write4bits(uint8_t value) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(miLCD.data_ports[i], miLCD.data_pins[i], (value >> i) & 0x01);
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
    digitalWrite(miLCD.data_ports[i], miLCD.data_pins[i], (value >> i) & 0x01);
  }
  LCD_pulseEnable();
}

/*******************************************************************************
* @brief  Lee valores en los 8 pines
* @param  None
* @retval Byte leído
*/
static uint8_t LCD_read8bits(void) {
  // Verifico conexión 8 pines
  if (miLCD.fourbitmode == true) Error_Handler();

  // Byte a leer bit por bit
  uint8_t LecturaByte = 0;
  GPIO_PinState LecturaPin = GPIO_PIN_RESET;

  // Envío ENABLE y leo
  digitalWrite(miLCD.enable_port, miLCD.enable_pin, GPIO_PIN_SET);		// <-- Flanco ascendente de ENABLE
  for (int i = 0; i < 8; i++) {
	  LecturaPin = digitalRead(miLCD.data_ports[i], miLCD.data_pins[i]);
	  LecturaByte |= (LecturaPin << i);
  }
  digitalWrite(miLCD.enable_port, miLCD.enable_pin, GPIO_PIN_RESET);	// <-- Flanco descendente de ENABLE

  // Leído!!!
  return LecturaByte;
}

/*******************************************************************************
* @brief  Lee valores en 4 pines
* @param  None
* @retval Byte leído (entre 0 y 16)
*/
static uint8_t LCD_read4bits(void) {
  // Verifico conexión 4 pines
  if (miLCD.fourbitmode == false) Error_Handler();

  // Byte a leer bit por bit
  uint8_t LecturaByte = 0;
  GPIO_PinState LecturaPin = GPIO_PIN_RESET;

  // Envío ENABLE y leo
  digitalWrite(miLCD.enable_port, miLCD.enable_pin, GPIO_PIN_SET);		// <-- Flanco ascendente de ENABLE
  for (int i = 0; i < 4; i++) {
	  LecturaPin = digitalRead(miLCD.data_ports[i], miLCD.data_pins[i]);
	  LecturaByte |= (LecturaPin << i);
  }
  digitalWrite(miLCD.enable_port, miLCD.enable_pin, GPIO_PIN_RESET);	// <-- Flanco descendente de ENABLE

  // Leído!!!
  return LecturaByte;
}

/***************************************************************END OF FILE****/
