/*
 * XPT2046_touch.h
 *
 *  Created on: 20 sep. 2019.
 *      Author: Andriy Honcharenko
 */

#ifndef XPT2046_TOUCH_H
#define XPT2046_TOUCH_H

#include "main.h"
#include <stdbool.h>

/*** Redefine if necessary ***/
#define	SOFTWARE_SPI

#ifdef SOFTWARE_SPI

#define XPT2046_MISO_Pin       	T_MISO_Pin
#define XPT2046_MISO_GPIO_Port 	T_MISO_GPIO_Port
#define XPT2046_MOSI_Pin        T_MOSI_Pin
#define XPT2046_MOSI_GPIO_Port  T_MOSI_GPIO_Port
#define XPT2046_CLK_Pin       	T_CLK_Pin
#define XPT2046_CLK_GPIO_Port 	T_CLK_GPIO_Port

#else

// Warning! Use SPI bus with < 2.5 Mbit speed, better ~650 Kbit to be save.
#define XPT2046_SPI_PORT hspi2
extern SPI_HandleTypeDef XPT2046_SPI_PORT;

#endif /* SOFTWARE_SPI */

#define XPT2046_IRQ_Pin       	T_PEN_Pin
#define XPT2046_IRQ_GPIO_Port 	T_PEN_GPIO_Port
#define XPT2046_CS_Pin        	T_CS_Pin
#define XPT2046_CS_GPIO_Port  	T_CS_GPIO_Port

#define TOUCH_ORIENTATION_PORTRAIT 			(0U)
#define TOUCH_ORIENTATION_LANDSCAPE 		(1U)
#define TOUCH_ORIENTATION_PORTRAIT_MIRROR 	(2U)
#define TOUCH_ORIENTATION_LANDSCAPE_MIRROR 	(3U)

#define ORIENTATION	(TOUCH_ORIENTATION_LANDSCAPE)

// change depending on screen orientation
#if (ORIENTATION == 0)
#define XPT2046_SCALE_X 240
#define XPT2046_SCALE_Y 320
#elif (ORIENTATION == 1)
#define XPT2046_SCALE_X 320
#define XPT2046_SCALE_Y 240
#elif (ORIENTATION == 2)
#define XPT2046_SCALE_X 240
#define XPT2046_SCALE_Y 320
#elif (ORIENTATION == 3)
#define XPT2046_SCALE_X 320
#define XPT2046_SCALE_Y 240
#endif

// to calibrate uncomment UART_Printf line in ili9341_touch.c
#define XPT2046_MIN_RAW_X 3400
#define XPT2046_MAX_RAW_X 29000
#define XPT2046_MIN_RAW_Y 3300
#define XPT2046_MAX_RAW_Y 30000

#ifdef __cplusplus
 extern "C" {
#endif

// call before initializing any SPI devices
extern bool XPT2046_TouchPressed(void);
extern bool XPT2046_TouchGetCoordinates(uint16_t* x, uint16_t* y);

#ifdef __cplusplus
}
#endif

#endif /* XPT2046_TOUCH_H_ */
