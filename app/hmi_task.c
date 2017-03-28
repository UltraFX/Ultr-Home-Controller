/*
 * hmi_task.c

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
#include "main.h"
#include "hmi_task.h"

thread_t *hmiThreadEvt;

static msg_t msg_dispData[NUM_BUFFERS];
mailbox_t mb_dispData;

static const SPIConfig ls_spicfg = {
  NULL,
  GPIOB,
  4,
  SPI_CR1_BR_0 | SPI_CR1_SSM
};

// GListeners
GListener glistener;

// GHandles
GHandle ghContainerPage0;
GHandle ghConsole1;

static void _createConsole(void) {
    GWidgetInit wi;
    gwinWidgetClearInit(&wi);

    font_t font1;

    //font1 = gdispOpenFont("BankGothic Md BT Medium 10");
    font1 = gdispOpenFont("DejaVuSans10");

    // create container widget: ghContainerPage0
    wi.g.show = FALSE;
    wi.g.x = 0;
    wi.g.y = 0;
    wi.g.width = 128;
    wi.g.height = 64;
    wi.g.parent = 0;
    wi.text = "Container";
    wi.customDraw = 0;
    wi.customParam = 0;
    wi.customStyle = 0;
    ghContainerPage0 = gwinContainerCreate(0, &wi, 0);

    wi.g.show = TRUE;
    wi.g.x = 0;
    wi.g.y = 0;
    wi.g.width = 128;
    wi.g.height = 64;
    wi.g.parent = ghContainerPage0;
    ghConsole1 = gwinConsoleCreate(0, &wi.g);
    gwinSetColor(ghConsole1, HTML2COLOR(0x000000));
    gwinSetBgColor(ghConsole1, HTML2COLOR(0xffffff));
    gwinSetFont(ghConsole1, font1);
    gwinRedraw(ghConsole1);
}

static void processDisp(uint8_t *content)
{
    switch(content[1])
    {
    case 0:
        break;
    case 1: gwinPrintf(ghConsole1, "Connected: %d\n", content[5]);
        break;
    case 2: gwinPrintf(ghConsole1, "Disconnected!\n");
        break;
    case 3: gwinPrintf(ghConsole1, "Notifications active!\n");
        break;
    case 4: gwinPrintf(ghConsole1, "Message!\n");
        break;
    case 5: gwinPrintf(ghConsole1, "Temperature value: %d!\n", content[5]);
        break;
    default:
      gwinPrintf(ghConsole1, "Error! Content: %d %d %d %d %d %d\n", content[0], content[1], content[2], content[3], content[4], content[5]);
    }
}

THD_WORKING_AREA(hmiThread, 512);
THD_FUNCTION(HMIThread, arg) {

  msg_t pbuf;
  msg_t msg;

  (void)arg;
  chRegSetThreadName("hmi");

  hmiThreadEvt = chThdGetSelfX();

  chMBObjectInit(&mb_dispData, msg_dispData, NUM_BUFFERS);

  spiStart(&SPID1, &ls_spicfg);

  chSysLock();
  gfxInit();
  chSysUnlock();

  gdispSetContrast(55);
  gdispSetOrientation(GDISP_ROTATE_180);

  _createConsole();
  gwinShow(ghContainerPage0);

  gwinPrintf(ghConsole1, "STM32 BGM113 Testprogram\n\n");
  gwinPrintf(ghConsole1, "doep\n");

  while(true)
  {
      msg = chMBFetch(&mb_dispData, (msg_t*)&pbuf, TIME_INFINITE);

      if(msg != MSG_RESET)
      {
          processDisp(pbuf);
      }
      else
      {
          gwinPrintf(ghConsole1, "Empty Buffer!\n");
      }
  }

}

