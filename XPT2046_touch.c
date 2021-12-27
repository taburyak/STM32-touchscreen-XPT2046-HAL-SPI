/*
 * XPT2046_touch.c
 *
 *  Created on: 20 sep. 2019.
 *      Author: Andriy Honcharenko
 */

#include <stdio.h>
#include <stdlib.h>
#include <XPT2046_touch.h>

#if (ORIENTATION == 0)
#define READ_X 0xD0
#define READ_Y 0x90
#elif (ORIENTATION == 1)
#define READ_Y 0xD0
#define READ_X 0x90
#elif (ORIENTATION == 2)
#define READ_X 0xD0
#define READ_Y 0x90
#elif (ORIENTATION == 3)
#define READ_Y 0xD0
#define READ_X 0x90
#endif

#ifdef SOFTWARE_SPI

#define SPI_CLK_L()		HAL_GPIO_WritePin(XPT2046_CLK_GPIO_Port, XPT2046_CLK_Pin, GPIO_PIN_RESET)
#define SPI_CLK_H()		HAL_GPIO_WritePin(XPT2046_CLK_GPIO_Port, XPT2046_CLK_Pin, GPIO_PIN_SET)
#define SPI_MOSI_L()	HAL_GPIO_WritePin(XPT2046_MOSI_GPIO_Port, XPT2046_MOSI_Pin, GPIO_PIN_RESET)
#define SPI_MOSI_H()	HAL_GPIO_WritePin(XPT2046_MOSI_GPIO_Port, XPT2046_MOSI_Pin, GPIO_PIN_SET)
#define SPI_MISO()		HAL_GPIO_ReadPin(XPT2046_MISO_GPIO_Port, XPT2046_MISO_Pin)

static void spi_write_byte(uint8_t data)
{
	for(size_t i = 0; i < 8; i++)
	{
		if (data & 0x80)
		{
			SPI_MOSI_H();
		}
		else
		{
			SPI_MOSI_L();
		}
		data = data << 1;
		SPI_CLK_L();
		SPI_CLK_H();
	}
}

static uint8_t spi_read_byte(void)
{
	uint8_t i, ret , value;
	ret = 0;
	i = 8;

	do {
		i--;
		SPI_CLK_L();
		value = SPI_MISO();
		if (value)
		{
			//set the bit to 0 no matter what
			ret |= (1 << i);
		}

		SPI_CLK_H();
	} while (i > 0);

	return ret;
}

#endif /* SOFTWARE_SPI */

static void XPT2046_TouchSelect()
{
    HAL_GPIO_WritePin(XPT2046_CS_GPIO_Port, XPT2046_CS_Pin, GPIO_PIN_RESET);
}

static void XPT2046_TouchUnselect()
{
    HAL_GPIO_WritePin(XPT2046_CS_GPIO_Port, XPT2046_CS_Pin, GPIO_PIN_SET);
}

bool XPT2046_TouchPressed(void)
{
    return HAL_GPIO_ReadPin(XPT2046_IRQ_GPIO_Port, XPT2046_IRQ_Pin) == GPIO_PIN_RESET;
}

bool XPT2046_TouchGetCoordinates(uint16_t* x, uint16_t* y)
{
#ifndef SOFTWARE_SPI

	static const uint8_t cmd_read_x[] = { READ_X };
    static const uint8_t cmd_read_y[] = { READ_Y };
    static const uint8_t zeroes_tx[] = { 0x00, 0x00 };

#endif /* SOFTWARE_SPI */

    XPT2046_TouchSelect();

    uint32_t avg_x = 0;
    uint32_t avg_y = 0;
    uint8_t nsamples = 0;

    for(uint8_t i = 0; i < 16; i++)
    {
        if(!XPT2046_TouchPressed())
            break;

        nsamples++;

        uint8_t y_raw[2];
        uint8_t x_raw[2];

#ifdef SOFTWARE_SPI

       spi_write_byte(READ_Y);

	   y_raw[0] = spi_read_byte();
	   y_raw[1] = spi_read_byte();

	   spi_write_byte(READ_X);

	   x_raw[0] = spi_read_byte();
	   x_raw[1] = spi_read_byte();

#else

        HAL_SPI_Transmit(&XPT2046_SPI_PORT, (uint8_t*)cmd_read_y, sizeof(cmd_read_y), HAL_MAX_DELAY);
        HAL_SPI_TransmitReceive(&XPT2046_SPI_PORT, (uint8_t*)zeroes_tx, y_raw, sizeof(y_raw), HAL_MAX_DELAY);

        HAL_SPI_Transmit(&XPT2046_SPI_PORT, (uint8_t*)cmd_read_x, sizeof(cmd_read_x), HAL_MAX_DELAY);
        HAL_SPI_TransmitReceive(&XPT2046_SPI_PORT, (uint8_t*)zeroes_tx, x_raw, sizeof(x_raw), HAL_MAX_DELAY);

#endif /* SOFTWARE_SPI */

        avg_x += (((uint16_t)x_raw[0]) << 8) | ((uint16_t)x_raw[1]);
        avg_y += (((uint16_t)y_raw[0]) << 8) | ((uint16_t)y_raw[1]);
    }

    XPT2046_TouchUnselect();

    if(nsamples < 16)
        return false;

    uint32_t raw_x = (avg_x / 16);
    if(raw_x < XPT2046_MIN_RAW_X) raw_x = XPT2046_MIN_RAW_X;
    if(raw_x > XPT2046_MAX_RAW_X) raw_x = XPT2046_MAX_RAW_X;

    uint32_t raw_y = (avg_y / 16);
    if(raw_y < XPT2046_MIN_RAW_Y) raw_y = XPT2046_MIN_RAW_Y;
    if(raw_y > XPT2046_MAX_RAW_Y) raw_y = XPT2046_MAX_RAW_Y;

    // Uncomment this line to calibrate touchscreen:
    //printf("raw_x = %d, raw_y = %d\r\n", (int) raw_x, (int) raw_y);

    *x = (raw_x - XPT2046_MIN_RAW_X) * XPT2046_SCALE_X / (XPT2046_MAX_RAW_X - XPT2046_MIN_RAW_X);
    *y = (raw_y - XPT2046_MIN_RAW_Y) * XPT2046_SCALE_Y / (XPT2046_MAX_RAW_Y - XPT2046_MIN_RAW_Y);

    return true;
}
