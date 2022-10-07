/*******************************************************************************
* @file    main.c
* @author  Guillermo Caporaletti
* @brief   Programa de prueba de LCD_driver.h
*          CESE, 18Co, FIUBA, octubre de 2022
*          Más detalles en main.h
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private macros ------------------------------------------------------------*/
#define TIEMPO_1_ENCENDIDO_LED 100
#define TIEMPO_2_ENCENDIDO_LED 1000
#define INTERVALO_FINAL_COUNTDOWN 10

/* Private variables ---------------------------------------------------------*/
delay_t parpadeoLed;				// Estructura para parpadeo de LED2
delay_t refresco_Final_Countdown;	// (leer en pantalla...)
uint32_t The_Final_Countdown=0;
char Numero_en_cadena[16];

// Caracter especial
uint8_t Alf[8] = {
  0b10001,
  0b10001,
  0b01110,
  0b10101,
  0b11111,
  0b11111,
  0b01110,
  0b11011
};

/* Private function prototypes -----------------------------------------------*/
static void CambiarTiempoParpadeoLed(void);
static void LeerPantalla();

/*******************************************************************************
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Inicialización para el manejo de errores    */
  Error_Handler_Init();

  /* STM32F4xx HAL library initialization:       */
  HAL_Init();

  /* Configure the system clock to 180 MHz       */
  SystemClock_Config();

  // Inicializo APIs
  if (uartInit() == false) Error_Handler();
  debounceFSM_init();
  BSP_LED_Init(LED2);
  delayInit( &parpadeoLed, TIEMPO_1_ENCENDIDO_LED);
  delayInit( &refresco_Final_Countdown, INTERVALO_FINAL_COUNTDOWN);
  The_Final_Countdown--;

  // Driver que queremos probar!!!
  LCD_init();
  LCD_createChar(0, Alf);	// <--¿Aparecerá en la pantalla?

  // Comienzo a mandar mensajes...
  uartSendCR();
  uartSendString((uint8_t *)"Recibiendo mensaje...\n\n");

  LCD_home();
  LCD_cursor();
  LCD_blink();
  delayMilliseconds(2000);

  LCD_setCursor(0,0);
  LCD_print("Hola terricolas");
  delayMilliseconds(2000);

  LCD_setCursor(0,1);
  LCD_print("Como estan?");
  delayMilliseconds(2000);

  LeerPantalla();
  uartSendCR();
  uartSendString((uint8_t *)"Empieza cuenta...\n");

  LCD_clear();
  LCD_print("Vamos en camino!");
  sprintf(Numero_en_cadena, "%lu",The_Final_Countdown);
  LCD_setCursor(0,1);
  LCD_noCursor();
  LCD_noBlink();
  LCD_print(Numero_en_cadena);

  LCD_setCursor(15,1);
  LCD_write(0);



  /* Infinite loop */
  while (1)
  {
	  // Reviso el boton de usuario...
	  debounceFSM_update();

	  // Reviso cuenta final
	  if (delayRead( &refresco_Final_Countdown )) {
		  The_Final_Countdown-=5;
		  LCD_setCursor(0,1);
		  sprintf(Numero_en_cadena, "%lu",The_Final_Countdown);
		  LCD_print(Numero_en_cadena);
	  }

	  // Además parpadeo LED2
	  if (delayRead( &parpadeoLed )) BSP_LED_Toggle(LED2);

	  // Si hay flanco descendiente cambio parpadeo
	  if (readKey()) {
		  CambiarTiempoParpadeoLed();
		  uartSendCR();
		  LeerPantalla();
	  }

  }
} /* Fin de función main ******************************************************/

/*******************************************************************************
  * @brief  Cambia el tiempo de parpadeo del LED2
  * @param  None
  * @retval None
  */
static void CambiarTiempoParpadeoLed(void) {
	// Cambio el tiempo de encendido
	static tick_t Encendido = TIEMPO_1_ENCENDIDO_LED;
	delayWrite( &parpadeoLed, Encendido );

	switch (Encendido) {
		case TIEMPO_1_ENCENDIDO_LED:
			Encendido = TIEMPO_2_ENCENDIDO_LED;
		break;

		case TIEMPO_2_ENCENDIDO_LED:
			Encendido = TIEMPO_1_ENCENDIDO_LED;
		break;

		default:
			Encendido = TIEMPO_1_ENCENDIDO_LED;
		break;
	}
}

/*******************************************************************************
  * @brief  Lee pantalla y lo manda a la UART
  * @param  None
  * @retval None
  */
static void LeerPantalla(void) {
	LCD_setCursor(0,0);
	uint8_t Caracter_leido;

	for (uint8_t i=0; i<80; i++) {
		if (i==40) uartSendCR();	// <-- Retorno de línea al completarse una fila de la pantalla
		Caracter_leido = LCD_data_read();
		if (Caracter_leido > 31) {
			uartSendStringSize(&Caracter_leido, 1);
		} else {
			uartSendStringSize((uint8_t *)" ", 1);
		}
	}
	uartSendCR();	// <-- Retorno de línea
}



/***************************************************************END OF FILE****/
