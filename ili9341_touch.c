/*
 * ili9341_touch.c
 *
 *  Created on: 20 вер. 2019 р.
 *      Author: Andriy Honcharenko
 */

#include "ili9341_touch.h"
#include "stdio.h"
#include <stdlib.h>

#define READ_X 0xD0
#define READ_Y 0x90

//static void ILI9341_TouchSelect(void);
//static void ILI9341_TouchUnselect(void);

static void ILI9341_TouchSelect()
{
    HAL_GPIO_WritePin(ILI9341_TOUCH_CS_GPIO_Port, ILI9341_TOUCH_CS_Pin, GPIO_PIN_RESET);
}

static void ILI9341_TouchUnselect()
{
    HAL_GPIO_WritePin(ILI9341_TOUCH_CS_GPIO_Port, ILI9341_TOUCH_CS_Pin, GPIO_PIN_SET);
}

bool ILI9341_TouchPressed()
{
    return HAL_GPIO_ReadPin(ILI9341_TOUCH_IRQ_GPIO_Port, ILI9341_TOUCH_IRQ_Pin) == GPIO_PIN_RESET;
}

/*-----------------------------------------------------------*/

//bool _ILI9341_TouchGetCoordinates(uint16_t* x, uint16_t* y)
//{
//	uint8_t SpiBuffer[3] = {0x80, 0x00, 0x00};
//
//    uint32_t avg_x = 0;
//    uint32_t avg_y = 0;
//
//	ILI9341_TouchSelect();
//
//	HAL_SPI_TransmitReceive(&ILI9341_TOUCH_SPI_PORT, SpiBuffer, SpiBuffer, 3, 100);
//
//	ILI9341_TouchUnselect();
//
//	for(;;)
//	{
//		if(ILI9341_TouchPressed())
//		{
//			uint8_t data_x [] = {0xD0, 0x00, 0x00};
//			uint8_t data_y [] = {0x90, 0x00, 0x00};
//
//			ILI9341_TouchSelect();
//
//			HAL_SPI_TransmitReceive(&ILI9341_TOUCH_SPI_PORT, data_x, data_x, 3, 100);
//			HAL_SPI_TransmitReceive(&ILI9341_TOUCH_SPI_PORT, data_y, data_y, 3, 100);
//
//			ILI9341_TouchUnselect();
//
////			avg_x += (((uint16_t)data_x[0]) << 8) | ((uint16_t)data_x[1]);
////			avg_y += (((uint16_t)data_y[0]) << 8) | ((uint16_t)data_y[1]);
//
//			avg_x = data_x[1];
//			avg_x = ((avg_x) << 8) + data_x[2];
//			avg_x >>= 4;
//
//			avg_y = data_y[1];
//			avg_y = ((avg_y) << 8) + data_y[2];
//			avg_y >>= 4;
//		}
//		else
//		{
//			avg_x = -1;
//			avg_y = -1;
//		}
//		break;
//	}
//
//    uint32_t raw_x = (avg_x / 16);
//    if(raw_x < ILI9341_TOUCH_MIN_RAW_X) raw_x = ILI9341_TOUCH_MIN_RAW_X;
//    if(raw_x > ILI9341_TOUCH_MAX_RAW_X) raw_x = ILI9341_TOUCH_MAX_RAW_X;
//
//    uint32_t raw_y = (avg_y / 16);
//    if(raw_y < ILI9341_TOUCH_MIN_RAW_Y) raw_y = ILI9341_TOUCH_MIN_RAW_Y;
//    if(raw_y > ILI9341_TOUCH_MAX_RAW_Y) raw_y = ILI9341_TOUCH_MAX_RAW_Y;
//
//    // Uncomment this line to calibrate touchscreen:
//    printf("raw_x = %d, raw_y = %d\r\n", raw_x, raw_y);
//
//    *x = (raw_x - ILI9341_TOUCH_MIN_RAW_X) * ILI9341_TOUCH_SCALE_X / (ILI9341_TOUCH_MAX_RAW_X - ILI9341_TOUCH_MIN_RAW_X);
//    *y = (raw_y - ILI9341_TOUCH_MIN_RAW_Y) * ILI9341_TOUCH_SCALE_Y / (ILI9341_TOUCH_MAX_RAW_Y - ILI9341_TOUCH_MIN_RAW_Y);
//
//	return true;
//}

/*--------------------------------------------------------*/

bool ILI9341_TouchGetCoordinates(uint16_t* x, uint16_t* y)
{
    static const uint8_t cmd_read_x[] = { READ_X };
    static const uint8_t cmd_read_y[] = { READ_Y };
    static const uint8_t zeroes_tx[] = { 0x00, 0x00 };

    ILI9341_TouchSelect();

    uint32_t avg_x = 0;
    uint32_t avg_y = 0;
    uint8_t nsamples = 0;

    for(uint8_t i = 0; i < 16; i++)
    {
        if(!ILI9341_TouchPressed())
            break;

        nsamples++;

        HAL_SPI_Transmit(&ILI9341_TOUCH_SPI_PORT, (uint8_t*)cmd_read_y, sizeof(cmd_read_y), HAL_MAX_DELAY);
        uint8_t y_raw[2];
        HAL_SPI_TransmitReceive(&ILI9341_TOUCH_SPI_PORT, (uint8_t*)zeroes_tx, y_raw, sizeof(y_raw), HAL_MAX_DELAY);

        HAL_SPI_Transmit(&ILI9341_TOUCH_SPI_PORT, (uint8_t*)cmd_read_x, sizeof(cmd_read_x), HAL_MAX_DELAY);
        uint8_t x_raw[2];
        HAL_SPI_TransmitReceive(&ILI9341_TOUCH_SPI_PORT, (uint8_t*)zeroes_tx, x_raw, sizeof(x_raw), HAL_MAX_DELAY);

        avg_x += (((uint16_t)x_raw[0]) << 8) | ((uint16_t)x_raw[1]);
        avg_y += (((uint16_t)y_raw[0]) << 8) | ((uint16_t)y_raw[1]);
    }

    ILI9341_TouchUnselect();

    if(nsamples < 16)
        return false;

    uint32_t raw_x = (avg_x / 16);
    if(raw_x < ILI9341_TOUCH_MIN_RAW_X) raw_x = ILI9341_TOUCH_MIN_RAW_X;
    if(raw_x > ILI9341_TOUCH_MAX_RAW_X) raw_x = ILI9341_TOUCH_MAX_RAW_X;

    uint32_t raw_y = (avg_y / 16);
    if(raw_y < ILI9341_TOUCH_MIN_RAW_Y) raw_y = ILI9341_TOUCH_MIN_RAW_Y;
    if(raw_y > ILI9341_TOUCH_MAX_RAW_Y) raw_y = ILI9341_TOUCH_MAX_RAW_Y;

    // Uncomment this line to calibrate touchscreen:
    //printf("raw_x = %d, raw_y = %d\r\n", (int) raw_x, (int) raw_y);

    *x = (raw_x - ILI9341_TOUCH_MIN_RAW_X) * ILI9341_TOUCH_SCALE_X / (ILI9341_TOUCH_MAX_RAW_X - ILI9341_TOUCH_MIN_RAW_X);
    *y = (raw_y - ILI9341_TOUCH_MIN_RAW_Y) * ILI9341_TOUCH_SCALE_Y / (ILI9341_TOUCH_MAX_RAW_Y - ILI9341_TOUCH_MIN_RAW_Y);

    return true;
}
