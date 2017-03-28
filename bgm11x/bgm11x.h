/**
  * @brief Bluetooth driver for Bluegiga/Silabs BGM111/113 BLE devices

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

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

/********************* INCLUDES ***********************************************/
#include <stdint.h>
#include "gecko_bglib.h"
#include "app/bluetooth.h"

/********************* GLOBAL DEFINES *****************************************/
#define SCAN_TIME 		5000
#define BT_DATA_MAXLENGTH       30

#define TRUE                    1
#define FALSE                   0

#define DEBUG_EN		TRUE

#define H_LIVROOM	        0
#define H_PC		        1
#define H_BATH		        2
#define H_KITCHEN	        3
#define L_LVROOM0	        4

#define DISCONNECTED        0
#define CONNECTED           1
#define READY   	        2
#define BUSY		        3

#define CONN_MODE_NORMAL        0
#define CONN_MODE_QUICK         1

#define bt_err_t	        int16_t

/********************* TYPE DEFINITIONS AND ENUMS *****************************/


/********************* GLOBAL VARIABLES ***************************************/
extern uint8_t evCount;
extern uint8_t byModuleState;
extern uint8_t g_bootOK;

/********************* GLOBAL FUNCTION PROTOTYPES *****************************/

/**
  * @brief Bluetooth (BGM11x) Event statemachine
  */
void bt_statemachine(void);

/**
  * @brief bluetooth handler (deprecated!)
  */
void bt_handler(struct gecko_cmd_packet *packet);

/**
  * @brief scan for nearby bluetooth devices
  *
  * @return     0 or error code
  */
bt_err_t bt_scan(void);

/**
  * @brief connect to a bluetooth device
  *
  * @param[in]  pointer to bluetooth device instance
  * @return     0 or error code
  */
bt_err_t bt_connect(bt_device_t *sDev);

/**
  * @brief disconnect from device
  *
  * @param[in]  pointer to bluetooth device instance
  * @return     0 or error code
  */
bt_err_t bt_disconnect(bt_device_t *sDev);

/**
  * @brief get service Handle by UUID
  *
  * @param[in]  pointer to bluetooth device instance
  * @param[in]  length of the UUID (16- or 128-Bit)
  * @param[in]  array containing the UUID
  * @return     0 or error code
  */
bt_err_t bt_get_serviceID(bt_device_t *sDev, uint8_t byUUID_Len,
                          uint8_t *byUUID);

/**
  * @brief get characteristic Handle by UUID
  *
  * @param[in]  pointer to bluetooth device instance
  * @param[in]  service Handle (from bt_get_serviceID())
  * @param[in]  length of the UUID (16- or 128-Bit)
  * @param[in]  array containing the UUID
  * @return     0 or error code
  */
bt_err_t bt_get_characteristicID(bt_device_t *sDev,
            uint32_t dwServiceID, uint8_t byUUID_Len, uint8_t *byUUID);

/**
  * @brief read data from characteristic via Handle
  *
  * @param[in]  pointer to bluetooth device instance
  * @param[in]  read characteristic Handle
  * @return     0 or error code
  */
bt_err_t bt_read_data(bt_device_t *sDev, uint16_t wReadHandle);

/**
  * @brief write data to characteristic via Handle
  *
  * @param[in]  pointer to bluetooth device instance
  * @param[in]  write characteristic Handle
  * @param[in]  array of the data
  * @param[in]  length of the data
  * @rturn      0 or error code
  */
bt_err_t bt_write_data(bt_device_t *sDev, uint32_t dwWriteHandle,
                       uint8_t *pdata, uint16_t wLength);

/**
  * @brief enable notification on a read characteristic
  *
  * @param[in]  pointer to bluetooth device instance
  * @param[in]  read characteristic Handle
  * @return     0 or error code
  */
bt_err_t bt_enable_notification(bt_device_t *sDev, uint16_t wNotifHandle);

/**
  * @brief change security state from standard to encrypted communication
  *
  * @param[in]  pointer to bluetooth device instance
  * @return     0 or error code
  */
bt_err_t bt_inc_security(bt_device_t *sDev);

void bt_list_bonds(void);

void bt_delete_bonding(uint8_t bond);

void bt_delete_all_bondings(void);

void bt_set_oob(uint32_t dwKey);

bt_err_t bt_set_sm_mode(bt_device_t *sDev);

#endif
