/**
* Simple library for DisplayTech 64128K graphic LC Display

MIT License

Copyright (c) 2017 Nicolas Dammin, UltraFX

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

#include <stdint.h>

void initLCD(void);
void LCD_Clear(void);
void LCD_Test(void);

void sendCmd(uint8_t command);
void sendData(uint8_t data);

#define LCD_WIDTH	128
#define LCD_HEIGHT	64

#define LCD_PORT	PORTC
#define LCD_SPI		SPIC

#define LCD_SCK		PIN7_bm
#define LCD_SI		PIN5_bm
#define LCD_RESET	PIN1_bm
#define LCD_A0		PIN2_bm
#define LCD_CS		PIN0_bm

//Command Table
#define DISP_ON			0xAF
#define DISP_OFF		0xAE

#define DISP_StLineSet	0x40
#define SET_PAGE		0xB0
#define COL_ADDR_HIGH	0x10
#define COL_ADDR_LOW	0x00

#define ADC_SEL_NORM	0xA0
#define ADC_SEL_REV		0xA1
#define DISP_NORMAL		0xA6
#define DISP_REV		0xA7
#define ALL_POINTS_ON	0xA5
#define ALL_POINTS_OFF	0xA4

#define LCD_BIAS		0xA2 //1/9
#define LCD_MOD_WRITE	0xE0
#define LCD_END			0xEE //Clear Read/Modify/Write
#define LCD_RES			0xE2

#define COM_OUT_NORM	0xC0
#define COM_OUT_REV		0xC8
#define POWER_CONTROL	0x2F

#define V0_RESIST_SET	0x20
#define V0_RESIST_NORM	0x25
#define EL_VOL_MODE		0x81
#define EL_VOL_REG		0xDC //Initial Contrast

#define SLEEP_MODE_EN	0xAC
#define SLEEP_MODE_DIS	0xAD

#define BOOSTER_RAT_SET	0xF8
#define BOOSTER_VAL		0x00

#define LCD_NOP			0xE3
