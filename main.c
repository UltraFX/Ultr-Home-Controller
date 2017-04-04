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

#include <stdint.h>
#include "main.h"
#include "test.h"
#include "bt_thread.h"
#include "gecko_bglib.h"
#include "handler_thread.h"
#include "hmi_task.h"

BGLIB_DEFINE();

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
//  sdStart(&SD2, NULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waBTThread, sizeof(waBTThread), NORMALPRIO, BTThread, NULL);
  chThdCreateStatic(hThread, sizeof(hThread), NORMALPRIO, handlerThread, NULL);
  chThdCreateStatic(hmiThread, sizeof(hmiThread), LOWPRIO, HMIThread, NULL);
  chThdCreateStatic(debugArea, sizeof(debugArea), LOWPRIO, DebugThread, NULL);

//  chprintf((BaseChannel *)&SD2, "test....\n");
  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */

  while (true) {
    if (!palReadPad(GPIOC, GPIOC_BUTTON));
//      TestThread(&SD2);
    chThdSleepMilliseconds(500);
  }
}
