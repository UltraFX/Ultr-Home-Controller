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
#include "drv_led_rev4.h"

/********************* LOCAL CONSTANTS ****************************************/
static uint8_t byaLed_rev4_service[] = {0x00};

static uint8_t byaLed_rev4_read_char[] = {0x00};
static uint8_t byaLed_rev4_write_char[] = {0x00};

static uint32_t dwServiceHandle = 0;
static uint16_t wWriteHandle = 0;
static uint16_t wNotifHandle = 0;

/********************* LOCAL FUNCTION PROTOTYPES ******************************/
void led_rev4_get_service_handle(bt_device_t *sDev, uint32_t dwServiceID);
void led_rev4_get_characteristic_handle(bt_device_t *sDev, uint16_t wCharacteristicID);
void led_rev4_handle_transfer(bt_device_t *sDev, uint32_t *pData);
void led_rev4_handler(bt_device_t *sDevice);

/********************* FUNCTION DEFINITIONS ***********************************/
void led_rev4_init(bt_device_t *sDev) 
{
    sDev->byConnState = DISCONNECTED;
    sDev->byConnHandle = 0;
    sDev->byState = BT_STATE_IDLE;
    sDev->iErr = 0;
    sDev->byCharPos = 0;
    sDev->pgetService = led_rev4_get_service_handle;
    sDev->pgetCharacteristic = led_rev4_get_characteristic_handle;
    sDev->pDataHandler = led_rev4_handle_transfer;
    sDev->pHandler = led_rev4_handler;
}

int16_t led_rev4_set_color(bt_device_t *sDev, uint8_t byChannel, WS2812_RGB_t sColor) 
{
    int16_t iErr = 0;
    uint8_t byaWrtCmd[20];
    
    if(dwServiceHandle == 0) 
    {
        //led_rev4_get_service_id(sDev);
    }
    else
    {
        byaWrtCmd[0] = 0x41;
        byaWrtCmd[1] = byChannel;
        byaWrtCmd[2] = sColor.red;
        byaWrtCmd[3] = sColor.green;
        byaWrtCmd[4] = sColor.blue;
        iErr = bt_write_data(sDev, wWriteHandle, byaWrtCmd, 20);
    }
    
    return iErr;
}

void led_rev4_get_service_handle(bt_device_t *sDev, uint32_t dwServiceID)
{
	dwServiceHandle = dwServiceID;

	(void)sDev;
}

void led_rev4_get_characteristic_handle(bt_device_t *sDev, uint16_t wCharacteristicID)
{
	if(sDev->byCharPos == 1) {
		wWriteHandle = wCharacteristicID;
	}
	else if(sDev->byCharPos == 2) {
		wNotifHandle = wCharacteristicID;
	}
}

void led_rev4_handle_transfer(bt_device_t *sDev, uint32_t *pData)
{
  struct gecko_msg_gatt_characteristic_value_evt_t *data =
      (struct gecko_msg_gatt_characteristic_value_evt_t*)pData;
    switch(data->att_opcode) 
    {
    case gatt_handle_value_notification:
        break;
    default:
        break;
    }

    (void)sDev;
}

void led_rev4_handler(bt_device_t *sDevice) {
	switch(sDevice->byState) {
        case BT_STATE_IDLE:
            break;
	case BT_STATE_CONNECT:
            sDevice->byState = BT_STATE_IDLE;
            break;
        case BT_STATE_CONNECTED_NORMAL:
            if(sDevice->iErr == BT_OK)
            {
                sDevice->iErr = bt_get_serviceID(sDevice,
                                  sizeof(byaLed_rev4_service), byaLed_rev4_service);
                if(sDevice->iErr == BT_OK)
                {
                    sDevice->byState = BT_STATE_IDLE;
                }
                else
                {
                    sDevice->byState = BT_STATE_ERROR;
                }
            } 
            else 
            {
                sDevice->byCharPos = 2;
                sDevice->byState = BT_STATE_SEEK_CHARACTERISTICS;
            }
            sDevice->byState = BT_STATE_IDLE;
            break;
        case BT_STATE_CONNECTED_SECURE:
            bt_enable_notification(sDevice, wNotifHandle);
            sDevice->byState = BT_STATE_IDLE;
            break;
        case BT_STATE_SEEK_SERVICE:
            sDevice->byState = BT_STATE_IDLE;
            break;
        case BT_STATE_SEEK_CHARACTERISTICS:
            break;
//        case BT_STATE_SERVICES_FOUND:
//            if(sDevice->byConnState == CONNECTED)
//            {
//                if(sDevice->byCharPos == 0)
//                {
//                    sDevice->iErr = bt_get_characteristicID(sDevice, dwServiceHandle,
//                            sizeof(byaLed_rev4_write_char), byaLed_rev4_write_char);
//
//                }
//                else if(sDevice->byCharPos == 1)
//                {
//                    sDevice->iErr = bt_get_characteristicID(sDevice, dwServiceHandle,
//                            sizeof(byaLed_rev4_read_char), byaLed_rev4_read_char);
//                }
//                else if(sDevice->byCharPos == 2)
//                {
//                    bt_inc_security(sDevice);
//                }
//
//                if(sDevice->iErr == BT_OK)
//                {
//                    sDevice->byCharPos++;
//                    sDevice->byState = BT_STATE_IDLE;
//                }
//                else
//                {
//                    sDevice->byState = BT_STATE_ERROR;
//                }
//
//            }
//            sDevice->byState = BT_STATE_IDLE;
//            break;
//        case BT_STATE_NOTIFY:
//            break;
        case BT_STATE_TRANSACTION:
            sDevice->byState = BT_STATE_IDLE;
            break;
	case BT_STATE_DISCONNECTED:
            sDevice->byState = BT_STATE_IDLE;
            break;
	case BT_STATE_SCAN_COMPLETED:
            break;
        case BT_PROCEDURE_COMPLETED:
            switch(sDevice->byProcState) 
            {
//            case BT_PROC_READ:
//                break;
            case BT_PROC_WRITE:
                break;
            case BT_PROC_NOTIF:
                sDevice->byConnState = READY;
                break;
            }
            break;
        case BT_STATE_ERROR:
        	sDevice->byState = BT_STATE_IDLE;
            break;
	default: break;
	}
}
