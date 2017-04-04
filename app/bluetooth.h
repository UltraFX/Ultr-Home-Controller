/*
 * bluetooth.h
 *

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

#ifndef APP_BLUETOOTH_H_
#define APP_BLUETOOTH_H_

/********************* TYPE DEFINITIONS AND ENUMS *****************************/
#define NUM_DEVICES     10

typedef enum
{
    BT_OK = 0,
    BT_ERROR = -1,
    BT_ERR_CONNECTED = -2,
    BT_ERR_DISCONNECTED = -3,
    BT_ERR_BUSY = -4,
    BT_ERR_NULLPOINTER = - 100
} bt_err_states_t;

typedef enum
{
    BT_STATE_IDLE = 0,
    BT_STATE_INIT,
    BT_STATE_SCAN,
    BT_STATE_SCAN_COMPLETED,
    BT_STATE_CONNECT,
    BT_STATE_CONNECTED_NORMAL,
    BT_STATE_CONNECTED_SECURE,
    BT_STATE_PARAM_CHANGED,
    BT_STATE_DISCONNECTED,
    BT_STATE_TRANSACTION,
    BT_STATE_WAIT_CHARACTERISTICS,
    BT_STATE_SEEK_SERVICE,
    BT_STATE_SEEK_CHARACTERISTICS,
    BT_PROCEDURE_COMPLETED,
    BT_BOND_FAILED,
    BT_BOND_SUCCESS,
    BT_STATE_ERROR
} bt_state_t;

typedef struct DeviceT
{
    uint8_t                     byID;
    uint8_t                     byRoomID;
    uint8_t                     byState;
    uint8_t                     byaName[25];
    uint8_t                     byNameLength;
    uint8_t                     byConnHandle;
    uint8_t                     byConnState;
    uint8_t                     byaBT_Address[6];
    uint8_t                     bySecurity;
    int16_t                     iErr;
    uint8_t                     byCharPos;
    uint8_t                     byProcState;
    uint32_t                    dwKey;
    uint8_t                     byType;
    struct gecko_cmd_packet     *packet;
    void                        (*pDataHandler)(struct DeviceT *sDev);
    void                        (*pgetService)(struct DeviceT *sDev,
                                uint32_t dwService);
    void                        (*pgetCharacteristic)(struct DeviceT *sDev,
                                uint16_t dwCharacteristic);
    void                        (*pHandler)(struct DeviceT *sDev);
} bt_device_t;

typedef enum
{
    BT_PROC_SERVICE = 0,
    BT_PROC_CHAR_READ,
    BT_PROC_CHAR_WRITE,
    BT_PROC_NOTIF,
    BT_PROC_WRITE,
    BT_PROC_NONE
} bt_procedure_t;

/********************* GLOBAL VARIABLES ***************************************/
extern bt_device_t sDevice[NUM_DEVICES];

/********************* GLOBAL FUNCTION PROTOTYPES *****************************/
uint8_t loadCtrlConfig(void);
uint8_t loadDevices(void);
uint8_t loadRooms(void);
uint8_t loadTypes(void);

#endif /* APP_BLUETOOTH_H_ */
