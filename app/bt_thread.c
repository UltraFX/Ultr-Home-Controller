/*
 * bt_thread.c

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

#include <string.h>
#include "ch.h"
#include "hal.h"
#include "bgm11x/bgm11x.h"
#include "drivers/drv_eq3.h"

thread_t *btThreadEvt, *btThreadRsp;

void ble_out(uint32_t dwLen, uint8_t *pbyData)
{
    //HAL_UART_Transmit(&huart1, pbyData, dwLen, 0xFFFF);
    chSysLock();
    uartStartSend(&UARTD1, dwLen, pbyData);
    chSysUnlock();
}

int32_t ble_in(uint32_t dwLen, uint8_t *pbyData)
{
    //return (int32_t)HAL_UART_Receive(&huart1, pbyData, dwLen, 0xFFFF);
    chSysLockFromISR();
    uartStartReceiveI(&UARTD1, dwLen, pbyData);
    chSysUnlockFromISR();
    return 0;
}

int32_t ble_flag(void) {
    return (int32_t)(USART1->SR & USART_SR_RXNE);
}

/*
 * This callback is invoked when a transmission buffer has been completely
 * read by the driver.
 */
static void txend1(UARTDriver *uartp) {

  (void)uartp;
}

/*
 * This callback is invoked when a transmission has physically completed.
 */
static void txend2(UARTDriver *uartp) {

  (void)uartp;
}

/*
 * This callback is invoked on a receive error, the errors mask is passed
 * as parameter.
 */
static void rxerr(UARTDriver *uartp, uartflags_t e) {

  (void)uartp;
  (void)e;
}

/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
static void rxchar(UARTDriver *uartp, uint16_t c) {

  (void)uartp;
  (void)c;
}

/*
 * This callback is invoked when a receive buffer has been completely written.
 */
static void rxend(UARTDriver *uartp) {

  (void)uartp;
  chSysLockFromISR();
  gecko_wait_message_int();
  chSysUnlockFromISR();
}

/*
 * UART driver configuration structure.
 */
static UARTConfig uart_cfg_1 = {
  txend1,
  txend2,
  rxend,
  rxchar,
  rxerr,
  115200,
  0,
  0,
  0
};

THD_WORKING_AREA(waBTThread, 128);
THD_FUNCTION(BTThread, arg) {

  struct gecko_cmd_packet *p;
//  bd_addr test;
//  uint8_t test_arr[6] = {0,0,0,0,0,0};

  (void)arg;
  chRegSetThreadName("bluetooth");

  btThreadEvt = chThdGetSelfX();

  BGLIB_INITIALIZE_NONBLOCK(ble_out, ble_in, ble_flag);

//  eq3_init(&sDevice[0]);

  uartStart(&UARTD1, &uart_cfg_1);
  gecko_wait_message_int();

  palSetPad(GPIOB, GPIOB_SWO);
  chThdSleepMilliseconds(1);
  palClearPad(GPIOB, GPIOB_SWO);
  chThdSleepMilliseconds(10);
  palSetPad(GPIOB, GPIOB_SWO);
  chThdSleepMilliseconds(1);

//  memcpy(test.addr, test_arr, 6);
//  eq3_connect(&sDevice[0], test);

  while (true) {

    chEvtWaitAny((eventmask_t)1);

    p = gecko_peek_event();

    if(p != NULL)
    {
        bt_handler(p);
    }

//    sDevice[0].pHandler(&sDevice[0]);
  }
}
