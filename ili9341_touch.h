/*
 * ili9341_touch.h
 *
 *  Created on: 20 sep. 2019 ð.
 *      Author: Andriy Honcharenko
 */

#ifndef ILI9341_TOUCH_H_
#define ILI9341_TOUCH_H_

#include "main.h"
#include <stdbool.h>

/*** Redefine if necessary ***/

// Warning! Use SPI bus with < 2.5 Mbit speed, better ~650 Kbit to be save.
#define ILI9341_TOUCH_SPI_PORT hspi2
extern SPI_HandleTypeDef ILI9341_TOUCH_SPI_PORT;

#define ILI9341_TOUCH_IRQ_Pin       T_IRQ_Pin
#define ILI9341_TOUCH_IRQ_GPIO_Port T_IRQ_GPIO_Port
#define ILI9341_TOUCH_CS_Pin        T_CS_Pin
#define ILI9341_TOUCH_CS_GPIO_Port  T_CS_GPIO_Port

// change depending on screen orientation
#define ILI9341_TOUCH_SCALE_X 240
#define ILI9341_TOUCH_SCALE_Y 320

// to calibrate uncomment UART_Printf line in ili9341_touch.c
#define ILI9341_TOUCH_MIN_RAW_X 3400
#define ILI9341_TOUCH_MAX_RAW_X 29000
#define ILI9341_TOUCH_MIN_RAW_Y 3300
#define ILI9341_TOUCH_MAX_RAW_Y 30000

// call before initializing any SPI devices
bool ILI9341_TouchPressed();
bool ILI9341_TouchGetCoordinates(uint16_t* x, uint16_t* y);

#endif /* ILI9341_TOUCH_H_ */
