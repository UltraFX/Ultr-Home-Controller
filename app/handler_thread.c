/*
 * handler_thread.c

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

#include <stdio.h>
#include "handler_thread.h"
#include "bgm11x/bgm11x.h"
#include "app/bt_thread.h"
#include "drivers/drivers.h"
#include "ch.h"
#include "hal.h"

/** DEFINES *******************************************************************/

/** LOCAL VARIABLES ***********************************************************/

/** LOCAL FUNCTION PROTOTYPES *************************************************/

/** THREAD ********************************************************************/
THD_WORKING_AREA(hThread, 128);
THD_FUNCTION(handlerThread, arg) {

  uint8_t byCurrentDevice = 0;
  uint8_t i;

  (void)arg;
  chRegSetThreadName("Handler");

  btThreadRsp = chThdGetSelfX();

  uint8_t byaBT_WoZa[] = {0x27, 0x87, 0x07, 0x22, 0x1a, 0x00};
  uint8_t pData[] = "Boot OK!\n";

  sDevice[0].byType = DRV_EQ3;
  memcpy(sDevice[0].byaBT_Address, byaBT_WoZa, 6);
  sDevice[0].byID = 1;
  sDevice[0].dwKey = 0;//4332;

  for(i = 0; i < NUM_DEVICES; i++)
  {
      if(sDevice[i].byType != 0)
      {
          drv_init(&sDevice[i]);
      }
  }

  while(true)
  {
      if(g_bootOK == 1)
      {
        //bt_list_bonds();
        eq3_connect(&sDevice[0]);
        g_bootOK = 0;
      }

      if(sDevice[byCurrentDevice].byType != 0)
      {
          sDevice[byCurrentDevice].pHandler(&sDevice[byCurrentDevice]);
      }

      chThdSleepMilliseconds(10);
  }
}

/** FUNCTION DEFINITIONS ******************************************************/



