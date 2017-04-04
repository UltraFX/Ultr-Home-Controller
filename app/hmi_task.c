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
#include "bgm11x/bgm11x.h"
#include "drivers/drivers.h"
#include "xprintf.h"

thread_t *hmiThreadEvt, *debugThdEvt;

static msg_t msg_dispData[NUM_BUFFERS];
mailbox_t mb_dispData;
char cRxData = 0;
char caCommand[25];
char *pcCmd = caCommand;

char caBuffer[50];
size_t dwLength;

void decodeCmd(char *command);

static const SPIConfig ls_spicfg = {
  NULL,
  GPIOB,
  4,
  SPI_CR1_BR_0 | SPI_CR1_SSM
};

static void resetCmd(void)
{
    uint8_t i;

    for(i = 0; i < 25; i++)
    {
        caCommand[i] = 0;
    }
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
  *pcCmd = cRxData;
  pcCmd++;
  if(cRxData == 13) //carriage return
  {
      pcCmd = caCommand;
      chEvtSignalI(debugThdEvt, (eventmask_t)2);
  }
  uartStartReceiveI(&UARTD2, 1, &cRxData);
  chSysUnlockFromISR();
}

static uint8_t str_cmp(char *src, char *cmp)
{
    uint8_t ptr= 0x00;
    while (cmp[ptr]!=0 && cmp[ptr]!=13)
    {
        if (src[ptr] !=cmp[ptr]) return 0;
        ptr++;
    }
    return 1;
}

/*
 * UART driver configuration structure.
 */
static UARTConfig uart_cfg_2 = {
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
    gwinSetColor(ghConsole1, HTML2COLOR(0xffffff));
    gwinSetBgColor(ghConsole1, HTML2COLOR(0x000000));
    gwinSetFont(ghConsole1, font1);
    gwinRedraw(ghConsole1);
}

static void processDisp(uint8_t *content)
{
    switch(content[1])
    {
    case 0:
        break;
    case 1: gwinPrintf(ghConsole1, "%d. Connected: %d\n", content[0], content[5]);
        break;
    case 2: gwinPrintf(ghConsole1, "%d. Disconnected!\n", content[0]);
        break;
    case 3: gwinPrintf(ghConsole1, "%d. Notifications active!\n", content[0]);
        break;
    case 4: gwinPrintf(ghConsole1, "Message!\n", content[0]);
        break;
    case 5: gwinPrintf(ghConsole1, "%d. Temperature value: %d.%d!\n", content[0], (content[5]/10), (content[5]%10));
        break;
    case 6:   if(str_cmp("test", (char *)&content[2]))
              {
                  gwinPrintf(ghConsole1, "Testing!\n");
              }
              else
              {
                  gwinPrintf(ghConsole1, "Command: %s\n", &content[2]);
              }
        break;
    case 7: gwinPrintf(ghConsole1, "%s\n", &content[2]);
        break;
    case 0xff:  gwinPrintf(ghConsole1, "Command not found\n");
        break;
    default:
      gwinPrintf(ghConsole1, "Error! Content: %d %d %d %d %d %d\n", content[0], content[1], content[2], content[3], content[4], content[5]);
    }
}

THD_WORKING_AREA(hmiThread, 512);
THD_FUNCTION(HMIThread, arg)
{

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

  gwinPrintf(ghConsole1, "UltraHome v 0.1.1\n\n");

  while(true)
  {
      msg = chMBFetch(&mb_dispData, (msg_t*)&pbuf, TIME_INFINITE);

      if(msg != MSG_RESET)
      {
          processDisp((uint8_t *)pbuf);
      }
      else
      {
          gwinPrintf(ghConsole1, "Empty Buffer!\n");
      }
  }

}

THD_WORKING_AREA(debugArea, 512);
THD_FUNCTION(DebugThread, arg)
{
    char byaTestMsg[] = "Debug-Thread started...\n";

    (void)arg;
    chRegSetThreadName("debug");

    debugThdEvt = chThdGetSelfX();

    uartStart(&UARTD2, &uart_cfg_2);

    chSysLock();
    uartStartSend(&UARTD2, sizeof(byaTestMsg), byaTestMsg);
    chSysUnlock();

    uartStartReceive(&UARTD2, 1, &cRxData);

    while(1)
    {
        chEvtWaitAny((eventmask_t)2);

        decodeCmd(caCommand);
    }

}

uint8_t dispData[5][25];

void decodeCmd(char *command)
{
    uint32_t dwNumber = 0;
    uint8_t byUpperVal, byLowerVal, byCnt;
    uint8_t *pbyVal;
    float fTemp;
    uint8_t byi, byk;
    dispData[0][1] = 7;
    dispData[1][1] = 7;
    dispData[2][1] = 7;
    dispData[3][1] = 7;
    dispData[4][1] = 7;

    char *cmd = &command[0];

    if(str_cmp(cmd, (char *)"connect"))
    {
        cmd += 8;
        while(*cmd != 0 && *cmd != '\r')
        {
            if((*cmd > 0x2F) && (*cmd < 0x3A))
            {
                dwNumber = (dwNumber * 10) + (*cmd - 0x30);
            }
            cmd++;
        }

        if(dwNumber > 0 && dwNumber < 10)
        {
          xsprintf((char *)&dispData[0][2], "Connecting to Nr. %d", dwNumber);
                  (void)chMBPost(&mb_dispData, (msg_t)dispData[0], TIME_IMMEDIATE);
            eq3_connect(&sDevice[dwNumber-1]);
        }

    }
    else if(str_cmp(cmd, (char *)"list"))
    {
          xsprintf((char *)&dispData[0][2], "Stored devices:");
          (void)chMBPost(&mb_dispData, (msg_t)dispData[0], TIME_IMMEDIATE);

          byi = 0;
          byk = 1;
          while(sDevice[byi].byID != 0)
          {
              xsprintf((char *)&dispData[byk][2], "%d: %02X:%02X:%02X:%02X:%02X:%02X",
                       byk,
                       sDevice[byi].byaBT_Address[0],
                       sDevice[byi].byaBT_Address[1],
                       sDevice[byi].byaBT_Address[2],
                       sDevice[byi].byaBT_Address[3],
                       sDevice[byi].byaBT_Address[4],
                       sDevice[byi].byaBT_Address[5]);
             (void)chMBPost(&mb_dispData, (msg_t)dispData[byk], TIME_IMMEDIATE);
             if(byk == 4)
             {
                 byk = 0;
             }
             else
             {
                 byk++;
             }
             byi++;
          }
    }
    else if(str_cmp(cmd, (char *)"disconnect"))
    {
        cmd += 11;
        while(*cmd != 0 && *cmd != '\r')
        {
            if((*cmd > 0x2F) && (*cmd < 0x3A))
            {
                dwNumber = (dwNumber * 10) + (*cmd - 0x30);
            }
            cmd++;
        }

        xsprintf((char *)&dispData[0][2], "Disconnect from Nr. %d", dwNumber);
        (void)chMBPost(&mb_dispData, (msg_t)dispData[0], TIME_IMMEDIATE);

        if(dwNumber > 0 && dwNumber < 10)
        {
            eq3_disconnect(&sDevice[dwNumber-1]);
        }

    }
    else if(str_cmp(cmd, (char *)"set"))
    {
        //Get device ID
        byCnt = 0;
        cmd += 4;
        while(*cmd != 0 && *cmd != '\r' && *cmd != ' ')
        {
            if((*cmd > 0x2F) && (*cmd < 0x3A))
            {
                dwNumber = (dwNumber * 10) + (*cmd - 0x30);
            }
            cmd++;
            byCnt++;
        }

        cmd += byCnt;

        //get temperature value
        fTemp = 0;
        byLowerVal = byUpperVal = 0;
        pbyVal = &byUpperVal;
        while(*cmd != 0 && *cmd != '\r')
        {
            if(*cmd == '.' || *cmd == ',')
            {
                pbyVal = &byLowerVal;
            }

            if((*cmd > 0x2F) && (*cmd < 0x3A))
            {
                *pbyVal = (*pbyVal * 10) + (*cmd - 0x30);
            }
            cmd++;
        }

        fTemp = (float)byUpperVal + ((float)byLowerVal/10.0);

        xsprintf((char *)&dispData[1][2], "Upper: %d, Lower: %d",
                 byUpperVal, byLowerVal);
        (void)chMBPost(&mb_dispData, (msg_t)dispData[1], TIME_IMMEDIATE);

        if(dwNumber > 0 && dwNumber < 10)
        {
            eq3_set_temperature(&sDevice[dwNumber-1], fTemp);
        }

    }
    else if(str_cmp(cmd, (char *)"get"))
    {
        //Get device ID
        cmd += 4;
        while(*cmd != 0 && *cmd != '\r' && *cmd != ' ')
        {
            if((*cmd > 0x2F) && (*cmd < 0x3A))
            {
                dwNumber = (dwNumber * 10) + (*cmd - 0x30);
            }
            cmd++;
            byCnt++;
        }

        if(dwNumber > 0 && dwNumber < 10)
        {
            eq3_get_temperature(&sDevice[dwNumber-1]);
        }

    }
    else
    {
        dispData[0][1] = 0xff;
        (void)chMBPost(&mb_dispData, (msg_t)dispData[0], TIME_IMMEDIATE);
    }

    resetCmd();
}

