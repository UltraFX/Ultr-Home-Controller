/**
 * @brief Bluetooth driver for EQ-3 temperature controllers

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
#include <string.h>
#include "drv_eq3.h"
#include "bgm11x/bgm11x.h"
#include "main.h"
#include "hmi_task.h"

/********************* LOCAL CONSTANTS ****************************************/
static uint8_t byaEq3_service[] = {0x46, 0x70, 0xb7, 0x5b, 0xff, 0xa6, 0x4a,
                                   0x13, 0x90, 0x90, 0x4f, 0x65, 0x42, 0x51,
                                   0x13, 0x3e};

static uint8_t byaEq3_read_char[] = {0x2a, 0xeb, 0xe0, 0xf4, 0x90, 0x6c, 0x41,
                                     0xaf, 0x96, 0x09, 0x29, 0xcd, 0x4d, 0x43,
                                     0xe8, 0xd0};
static uint8_t byaEq3_write_char[] = {0x09, 0xea, 0x79, 0x81, 0xdf, 0xb8, 0x4b,
                                      0xdb, 0xad, 0x3b, 0x4a, 0xce, 0x5a, 0x58,
                                      0xa4, 0x3f};

static const uint32_t dwCServiceHandle = 2000;
static const uint16_t wCWriteHandle = 0x411;
static const uint16_t wCReadHandle = 1057;//1072;;

static uint32_t dwServiceHandle = 0;
static uint16_t wTempWriteHandle = 0;
static uint16_t wNotifHandle = 0;

static eq3_data_t eq3Data;

static uint8_t dispData[10];

/********************* LOCAL FUNCTION PROTOTYPES ******************************/
void eq3_get_service_handle(bt_device_t *sDev, uint32_t dwServiceID);
void eq3_get_characteristic_handle(bt_device_t *sDev,
                                   uint16_t wCharacteristicID);
void eq3_handle_transfer(bt_device_t *sDev, uint32_t *pData);

void eq3_Disp(bt_device_t *sDev, uint8_t function, uint32_t dwData);

/********************* FUNCTION DEFINITIONS ***********************************/
void eq3_init(bt_device_t *sDev)
{
    sDev->byConnState = DISCONNECTED;
    sDev->byConnHandle = 0;
    sDev->byState = BT_STATE_IDLE;
    sDev->byProcState = BT_PROC_SERVICE;
    sDev->iErr = 0;
    sDev->byCharPos = 0;
    sDev->pgetService = eq3_get_service_handle;
    sDev->pgetCharacteristic = eq3_get_characteristic_handle;
    sDev->pDataHandler = eq3_handle_transfer;
    sDev->pHandler = eq3_handler;

    /* Set constant handle values for quick connect */
    dwServiceHandle = dwCServiceHandle;
    wTempWriteHandle = wCWriteHandle;
    wNotifHandle = wCReadHandle;
}

int16_t eq3_connect(bt_device_t *sDev)
{
    bt_set_sm_mode(sDev);
    bt_set_oob(sDev->dwKey);
    gecko_cmd_sm_set_bondable_mode(1);

    evCount = 0;
    sDev->byCharPos = 0;
    sDev->iErr = bt_connect(sDev);

    return sDev->iErr;
}

int16_t eq3_disconnect(bt_device_t *sDev)
{
    evCount = 0;
    sDev->byCharPos = 0;
    sDev->byConnState = BUSY;
    sDev->iErr = bt_disconnect(sDev);

    return sDev->iErr;
}

int16_t eq3_set_temperature(bt_device_t *sDev, float temperature)
{
    uint8_t byaWrtCmd[2];

    byaWrtCmd[0] = 0x41;
    byaWrtCmd[1] = (uint8_t)(temperature * 2);
    sDev->iErr = bt_write_data(sDev, wTempWriteHandle, byaWrtCmd, 2);

    return sDev->iErr;
}

int16_t eq3_set_mode(bt_device_t *sDev, uint8_t byMode)
{
    uint8_t byaWrtCmd[2];

    byaWrtCmd[0] = 0x00;
    byaWrtCmd[1] = byMode;
    sDev->iErr = bt_write_data(sDev, wTempWriteHandle, byaWrtCmd, 2);

    return sDev->iErr;
}

void eq3_get_service_handle(bt_device_t *sDev, uint32_t dwServiceID) {
  dwServiceHandle = dwServiceID;

  (void)sDev;
}

void eq3_get_characteristic_handle(bt_device_t *sDev,
                                   uint16_t wCharacteristicID) {
  (void*)sDev;

  if (sDevice->byProcState == BT_PROC_CHAR_WRITE) {
    wTempWriteHandle = wCharacteristicID;
  }
  else if (sDevice->byProcState == BT_PROC_CHAR_READ) {
    wNotifHandle = wCharacteristicID;
  }
}

void eq3_handle_transfer(bt_device_t *sDev, uint32_t *pData)
{
  float fTemp = 0.0;
  uint8_t byLength;

  switch (sDev->packet->data.evt_gatt_characteristic_value.att_opcode) {
  case gatt_handle_value_notification:
      byLength = sDev->packet->data.evt_gatt_characteristic_value.value.len;
      fTemp = (float)sDev->packet->data.evt_gatt_characteristic_value.value.data[byLength-1]/2.0;
      chprintf((BaseChannel *)&SD2, "Temperature: (%d) %f°C\n", byLength, fTemp);
      eq3_Disp(sDevice, 5, (uint32_t)(fTemp*10));
    break;
  default:
    break;
  }
}

uint8_t eq3_check_state(bt_device_t *sDev) {
    return sDev->byConnState;
}

void eq3_handler(bt_device_t *sDevice)
{
    switch (sDevice->byState) {
    case BT_STATE_IDLE:
      break;
    case BT_STATE_INIT:
      sDevice->byState = BT_STATE_IDLE;
      break;
    case BT_STATE_CONNECT:
      sDevice->byState = BT_STATE_IDLE;
      break;
    case BT_STATE_CONNECTED_NORMAL:
        eq3_Disp(sDevice, 1, 0);
        sDevice->byConnState = READY;
        bt_inc_security(sDevice);
        sDevice->byState = BT_STATE_IDLE;
      break;
    case BT_STATE_CONNECTED_SECURE:
      eq3_Disp(sDevice, 1, 1);
      sDevice->byConnState = READY;
      sDevice->byProcState = BT_PROC_NOTIF;
      bt_enable_notification(sDevice, wNotifHandle);
      sDevice->byState = BT_STATE_IDLE;
      break;
    case BT_STATE_SEEK_SERVICE:
      sDevice->byState = BT_STATE_IDLE;
      break;
    case BT_STATE_SEEK_CHARACTERISTICS:
      sDevice->byState = BT_STATE_IDLE;
      break;
    case BT_STATE_TRANSACTION:
      sDevice->byState = BT_STATE_IDLE;
      break;
    case BT_STATE_DISCONNECTED:
      sDevice->byConnState = DISCONNECTED;
      eq3_Disp(sDevice, 2, 0);
      sDevice->byState = BT_STATE_IDLE;
      break;
    case BT_STATE_SCAN_COMPLETED:
      sDevice->byConnState = READY;
      break;
    case BT_PROCEDURE_COMPLETED:
      sDevice->byConnState = READY;
      switch (sDevice->byProcState)
      {
          case BT_PROC_SERVICE:
            sDevice->byProcState = BT_PROC_CHAR_WRITE;
            sDevice->iErr = bt_get_characteristicID(sDevice, dwServiceHandle,
                                                    sizeof(byaEq3_write_char),
                                                    byaEq3_write_char);
            break;
          case BT_PROC_CHAR_READ:
            bt_inc_security(sDevice);
            break;
          case BT_PROC_CHAR_WRITE:
            sDevice->byProcState = BT_PROC_CHAR_READ;
            sDevice->iErr = bt_get_characteristicID(sDevice, dwServiceHandle,
                                                    sizeof(byaEq3_read_char),
                                                    byaEq3_read_char);
            break;
          case BT_PROC_NOTIF:
            eq3_Disp(sDevice, 3, 0);
            if (sDevice->packet->data.evt_gatt_procedure_completed.result != 0)
            {
                if(sDevice->packet->data.evt_gatt_procedure_completed.result == 0x405)
                {
                    bt_inc_security(sDevice);
                }
                else
                {
                    bt_disconnect(sDevice);
                }
            }
            else
            {
                //eq3_Disp(sDevice, 3, 0);
                chprintf((BaseChannel *)&SD2, "Complete\n");
                //bt_read_data(sDevice, wNotifHandle);
//                eq3_set_temperature(sDevice, 23.0);
                uint8_t test[] = {03};
                bt_write_data(sDevice, wTempWriteHandle, test, 1);
                sDevice->byProcState = BT_PROC_NONE;
                sDevice->byConnState = READY;
            }
            break;
          case BT_PROC_WRITE:
            sDevice->byConnState = READY;
            break;
          case BT_PROC_NONE:
            break;
          default:
            break;
      }
    case BT_BOND_FAILED:
      chprintf((BaseChannel *)&SD2, "Bonding failed\n");
      sDevice->byState = BT_STATE_IDLE;
      break;
    case BT_STATE_ERROR:
      sDevice->byState = BT_STATE_IDLE;
      break;
    default:
      sDevice->byState = BT_STATE_IDLE;
      break;
    }
}

void eq3_Disp(bt_device_t *sDev, uint8_t function, uint32_t dwData)
{

  dispData[0] = sDev->byID;
  dispData[1] = function;

  dispData[2] = (dwData >> 24);
  dispData[3] = (dwData >> 16);
  dispData[4] = (dwData >> 8);
  dispData[5] = (dwData&0xFF);

  (void)chMBPost(&mb_dispData, (msg_t)dispData, TIME_IMMEDIATE);
}
