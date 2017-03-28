/*

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

#ifndef DRV_LED_REV4_H
#define DRV_LED_REV4_H

#include "bgm11x/bgm11x.h"

typedef struct 
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} WS2812_RGB_t;

void led_rev4_init(bt_device_t *sDev);
int16_t led_rev4_set_color(bt_device_t *sDev, uint8_t byChannel, WS2812_RGB_t sColor);
void led_rev4_handle_notification(uint8_t byDeviceID, uint8_t *pbyData, uint16_t byLength);

#endif
