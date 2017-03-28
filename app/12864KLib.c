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

#include "12864KLib.h"
#include "hal.h"


void sendCmd(uint8_t command)
{
	//Enable LCD
	//HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	//palClearPad(GPIOB, GPIOB_PB4);

	//A0 Low: Command
	//HAL_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin, GPIO_PIN_RESET);
	palClearPad(GPIOA, GPIOA_PA8);

//	spiAcquireBus(&SPID1);
	spiSelect(&SPID1);
	spiSend(&SPID1, 1, &command);
	spiUnselect(&SPID1);
//	spiReleaseBus(&SPID1);
	//HAL_SPI_Transmit_IT(&hspi1, &command, 1);

	//Disable LCD
	//HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
	//palSetPad(GPIOB, GPIOB_PB4);
}

void sendData(uint8_t data)
{

	//Enable LCD
	//HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	//palClearPad(GPIOB, GPIOB_PB4);

	//A0 Low: Command
	//HAL_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin, GPIO_PIN_SET);
	palSetPad(GPIOA, GPIOA_PA8);

//	spiAcquireBus(&SPID1);
//    spiStart(&SPID1, &ls_spicfg);
    spiSelect(&SPID1);
    spiSend(&SPID1, 1, &data);
    spiUnselect(&SPID1);
//    spiReleaseBus(&SPID1);
	//HAL_SPI_Transmit_IT(&hspi1, &data, 1);

	//Disable LCD
	//HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
	//palSetPad(GPIOB, GPIOB_PB4);

}

void initLCD(void)
{
	//HAL_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
	//HAL_GPIO_WritePin(LCD_RES_GPIO_Port, LCD_RES_Pin, GPIO_PIN_SET);
	palClearPad(GPIOA, GPIOA_PA8);
	palSetPad(GPIOB, GPIOB_PB4);
	palSetPad(GPIOB, GPIOB_PB10);

	//HAL_Delay(20);
	//HAL_GPIO_WritePin(LCD_RES_GPIO_Port, LCD_RES_Pin, GPIO_PIN_RESET);
	palClearPad(GPIOB, GPIOB_PB10);
	//Init Routine
	//HAL_Delay(50);
	//HAL_GPIO_WritePin(LCD_RES_GPIO_Port, LCD_RES_Pin, GPIO_PIN_SET);
	palSetPad(GPIOB, GPIOB_PB10);
	//HAL_Delay(1);

	sendCmd(LCD_BIAS); // Bias 1/9
	sendCmd(ADC_SEL_REV); //ADC Select -> SEG0 to 131
	sendCmd(COM_OUT_NORM); //SHL Select -> COM63 to COM0

	sendCmd(V0_RESIST_NORM); //V0 Voltage
	sendCmd(EL_VOL_MODE); //Electronic Volume Control
	sendCmd(0xE0); //EVC Register

	sendCmd(BOOSTER_RAT_SET); //Booster Ratio
	sendCmd(BOOSTER_VAL); // x4
	sendCmd(POWER_CONTROL);

	//HAL_Delay(20);

	sendCmd(DISP_NORMAL);
	sendCmd(DISP_ON); //Enable Display
	sendCmd(ALL_POINTS_ON); //All Points on
	//HAL_Delay(500);
	sendCmd(ALL_POINTS_OFF); //All Points Off

	LCD_Clear();
	LCD_Test();
}

void LCD_Clear(void) {
  uint8_t p, c;

  for(p = 0; p < 8; p++) {
    sendCmd(SET_PAGE | p);
    for(c = 0; c < 132; c++) {
      sendCmd(COL_ADDR_LOW  | (c & 0x0f));
      sendCmd(COL_ADDR_HIGH | ((c >> 4) & 0x0f));
      sendData(0x00);
    }
  }
}

void LCD_Test(void)
{
  uint8_t p, c;

	for(p = 0; p < 8; p=p+2) {
		sendCmd(SET_PAGE | p);
		for(c = 0; c < 132; c++) {
			sendCmd(COL_ADDR_LOW  | (c & 0x0f));
			sendCmd(COL_ADDR_HIGH | ((c >> 4) & 0x0f));
			sendData(0xff);
		}
	}
}


