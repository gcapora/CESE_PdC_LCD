# Controlador de Pantalla de Cristal Líquido (LCD) para plataforma Nucleo-F4ZI

Autor: Guillermo F. Caporaletti <gfcaporaletti@undav.edu.ar>
Fecha: Septiembre 2022
Versión: 1.0

El **controlador para LCD** fue desarrollado en el curso de Protocolos de Comunicaciones de la Carrera de Especialización en Sistemas Embebidos. Se aplica a las pantallas controladas por el **_chipset_ HD44780 de Hitachi** (o compatible). La librería fue implementada con una pantalla 1602 (16 columnas y 2 filas), aunque debería funcionar con pantallas de hasta 4 filas. Permite una comunicación paralela entre el MCU y el HD44780 en modo 8 pines y 4 pines.

La implementación está basada en la librería C++ para Arduino “LiquidCrystal.cpp” realizada por Hans-Christoph Steiner (2008). Las principales diferencias residen en las instrucciones necesarias para acceder al hardware de la plataforma y que fue implementada en C (sin programación orientada a objetos). Los comandos fueron cotejados además con la hoja de datos del *chipset* HD44780 (cuyo documento incluimos en el repositorio).

La librería desarrollada tiene los siguientes archivos:
- **"LCD_driver.h"**: Contiene las definiciones públicas de tipos y macros, y los prototipos de funciones públicas.
- **"LCD_driver.c"**: Contiene los comandos que serán utilizados por el programa que necesite acceder a la pantalla, sin el detalle del hardware. Llama a las funciones de "LCD_stm32f4xx_nucleo.c" para concretar las acciones.
- **"LCD_stm32f4xx_nucleo.c"**: Contiene las instrucciones HAL de acceso al hardware (puerto específico). También tiene las configuraciones de hardware del display (como pines utilizados y especificaciones de la pantalla).

## Modo de uso

En el módulo de puerto específico “LCD_stm32f4xx.c” se encuentran definidos los pines utilizados (cada pin se identifica como un puerto GPIO de A a K, más un número de pin de 0 a 15). Pueden cambiarse por otros; aunque debe garantizarse que los clocks de los puertos utilizados sean activados (esto se hace dentro de la función LCD_init_stm32f4xx() de este módulo).

Los comandos del módulo “LCD_driver.c” a utilizar por el programa principal son:
- void LCD_init();
- void LCD_clear();
- void LCD_home();
- void LCD_setCursor(uint8_t, uint8_t);
- void LCD_noDisplay();
- void LCD_display();
- void LCD_noCursor();
- void LCD_cursor();
- void LCD_noBlink();
- void LCD_blink();
- void LCD_scrollDisplayLeft();
- void LCD_scrollDisplayRight();
- void LCD_leftToRight();
- void LCD_rightToLeft();
- void LCD_autoscroll();
- void LCD_noAutoscroll();
- void LCD_createChar(uint8_t, uint8_t[]);
- void LCD_print(char *);
- void LCD_write(uint8_t);
- void LCD_command(uint8_t);
- uint8_t LCD_data_read(void);
- uint8_t LCD_address_read(void);
- bool LCD_busy_flag(void);

## Comentario sobre la implementación

Un problema extra que surgió fue lograr la **compatibilidad de tensiones** entre el MPU STM32F429 y el LCD1602 utilizado. El MPU utiliza una tensión de 3,3V de salida en los pines, mientras que el display utilizado requería valores lógicos de TTL 5V. Consultado el Manual de referencia STM32Fxx (RM0090, pg. 268, Tabla 35 y Figura 25), configuramos los pines de salida como _Open Drain_ (OD), de modo de imponer un 0 pero dejar el pin flotante en un 1. Esto logró que la tensión de salida alcance los 5V en 1, alcanzando así la compatibilidad con el LCD1602. La tensión de 5V en 1 es forzada por el LCD, y la tensión de 0V en 0 por el STM32Fxx. 

Sin embargo, fue necesario agregar además una resistencia de 3,9K desde ENABLE a 5V. Evalúo que esta resistencia permitió no sólo garantizar mejor los 5V en 1, sino también disminuir el tiempo de crecimiento de la señal. Para que funcione, bastó con incluir esta resistencia _pull-up_ en ENABLE. El pin ENABLE es el que indica con un pulso la lectura de los datos y por lo tanto es el más crítico. De todos modos, sería recomendable incluir estas resistencias _pull-up_ para todos los pines y comprobar su buen funcionamiento. 





