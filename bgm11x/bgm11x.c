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

/********************* INCLUDES ***********************************************/
#include <stdio.h>
#include "ch.h"
#include "hal.h"
#include "bgm11x/bgm11x.h"


/********************* LOCAL VARIABLES ****************************************/
bt_device_t generalDevice;

static uint8_t byNumDevices = 0;

uint8_t evCount;
uint8_t g_bootOK = 0;

uint8_t byModuleState = DISCONNECTED;

static virtual_timer_t vt1;


/********************* LOCAL FUNCTION PROTOTYPES ******************************/
static uint8_t comp_array(uint8_t *pchar1, uint8_t *pchar2);
static void addDevice(struct gecko_msg_le_gap_scan_response_evt_t *msg);
static void list_devices(void);
static void reset_found_devices(void);
static void bt_stop_scan(void *p);

/********************* FUNCTION DEFINITIONS ***********************************/

void bt_handler(struct gecko_cmd_packet *packet) {
    uint8_t byI;
    evCount++;

    switch(BGLIB_MSG_ID(packet->header)) {
    case gecko_evt_system_boot_id:
        g_bootOK = 1;
        for(byI = 0; byI < (NUM_DEVICES-1); byI++) {
            sDevice[byI].byState = BT_STATE_INIT;
        }
        generalDevice.byState = BT_STATE_INIT;
        break;
    case gecko_evt_le_connection_opened_id: 
        for(byI = 0; byI < (NUM_DEVICES-1); byI++) {
            if(sDevice[byI].byConnHandle == packet->data.evt_le_connection_opened.connection)
            {
                sDevice[byI].packet = packet;
            	sDevice[byI].byConnState = CONNECTED;
                break;
            }
        }
        break;
    case gecko_evt_le_connection_parameters_id:
        for(byI = 0; byI < (NUM_DEVICES-1); byI++) {
            if(sDevice[byI].byConnHandle == packet->data.evt_le_connection_parameters.connection)
            {
                sDevice[byI].packet = packet;
                if(packet->data.evt_le_connection_parameters.security_mode == 0)
                {
                  sDevice[byI].byState = BT_STATE_CONNECTED_NORMAL;
                }
                else
                {
                  sDevice[byI].byState = BT_STATE_CONNECTED_SECURE;
                }
                sDevice[byI].bySecurity = packet->data.evt_le_connection_parameters.security_mode;
                break;
            }
        }
    	break;
    case gecko_evt_le_connection_closed_id:
        for(byI = 0; byI < (NUM_DEVICES-1); byI++) {
            if(sDevice[byI].byConnHandle == packet->data.evt_le_connection_closed.connection)
            {
                sDevice[byI].packet = packet;
                sDevice[byI].byState = BT_STATE_DISCONNECTED;
                sDevice[byI].byConnHandle = 0;
                sDevice[byI].byConnState = DISCONNECTED;
                break;
            }
        }
        break;
    case gecko_evt_endpoint_status_id:
        for(byI = 0; byI < (NUM_DEVICES-1); byI++) {
            if(sDevice[byI].byConnHandle == packet->data.evt_endpoint_status.endpoint)
            {
                sDevice[byI].packet = packet;
                sDevice[byI].byState = BT_STATE_DISCONNECTED;
                sDevice[byI].byConnHandle = 0;
                sDevice[byI].byConnState = DISCONNECTED;
                break;
            }
        }
        break;
    case gecko_evt_le_gap_scan_response_id:
    		addDevice(&packet->data.evt_le_gap_scan_response);
    		break;
    case gecko_evt_gatt_service_id:
        for(byI = 0; byI < (NUM_DEVICES-1); byI++) {
            if(sDevice[byI].byConnHandle == packet->data.evt_gatt_service.connection)
            {
                sDevice[byI].packet = packet;
            	sDevice[byI].pgetService(&sDevice[byI], packet->data.evt_gatt_service.service);
                sDevice[byI].byState = BT_STATE_SEEK_CHARACTERISTICS;
                break;
            }
        }
        break;
    case gecko_evt_gatt_characteristic_id:
    	for(byI = 0; byI < (NUM_DEVICES-1); byI++) {
            if(sDevice[byI].byConnHandle == packet->data.evt_gatt_characteristic.connection)
            {
                sDevice[byI].packet = packet;
            	sDevice[byI].pgetCharacteristic(&sDevice[byI], packet->data.evt_gatt_characteristic.characteristic);
                sDevice[byI].byState = BT_STATE_SEEK_CHARACTERISTICS;
                break;
            }
        }
        break;
    case gecko_evt_gatt_characteristic_value_id:
    	for(byI = 0; byI < (NUM_DEVICES-1); byI++) {
            if(sDevice[byI].byConnHandle == packet->data.evt_gatt_characteristic_value.connection)
            {
                sDevice[byI].packet = packet;
                sDevice[byI].pDataHandler(&sDevice[byI],
                       (uint32_t*)&packet->data.evt_gatt_characteristic_value);
                sDevice[byI].byState = BT_STATE_TRANSACTION;
                break;
            }
        }
        break;
    case gecko_evt_gatt_procedure_completed_id:
        for(byI = 0; byI < (NUM_DEVICES-1); byI++) {
            if(sDevice[byI].byConnHandle == packet->data.evt_gatt_procedure_completed.connection)
            {
                sDevice[byI].packet = packet;
                sDevice[byI].byState = BT_PROCEDURE_COMPLETED;
                break;
            }
        }
        break;
    case gecko_evt_sm_passkey_request_id:
    	//printf("Paskey request!\n");
    	break;
    case gecko_evt_sm_bonded_id:
    	break;
    case gecko_evt_sm_bonding_failed_id:
        for(byI = 0; byI < (NUM_DEVICES-1); byI++) {
          if(sDevice[byI].byConnHandle == packet->data.evt_sm_bonding_failed.connection)
          {
            sDevice[byI].byState = BT_BOND_FAILED;
            break;
          }
        }
    	break;
    default:
    	break;
    }
}

bt_err_t bt_set_sm_mode(bt_device_t *sDev)
{
    struct gecko_msg_sm_configure_rsp_t *smConf;

    sDev->iErr = BT_OK;

    smConf = gecko_cmd_sm_configure(0x0 ,sm_io_capability_keyboardonly);
    //sDev->iErr = smConf->result;

    return sDev->iErr;
}

bt_err_t bt_set_key(bt_device_t *sDev, uint32_t dwKey)
{
    struct gecko_msg_sm_enter_passkey_rsp_t *pKey;

    pKey = gecko_cmd_sm_enter_passkey(sDev->byConnHandle, dwKey);
    sDev->iErr = pKey->result;

    return sDev->iErr;
}

bt_err_t bt_get_serviceID(bt_device_t *sDev, uint8_t byUUID_Len, uint8_t *byUUID) {
    struct gecko_msg_gatt_discover_primary_services_by_uuid_rsp_t *sGattRsp;
    
    sDev->iErr = BT_OK;

    if(sDev == NULL) 
    {
        sDev->iErr = BT_ERR_NULLPOINTER;
    } 
    else if(sDev->byConnState == READY)
    {
        sDev->byConnState = BUSY;
        sDev->byState = BT_STATE_SEEK_SERVICE;
        sGattRsp = gecko_cmd_gatt_discover_primary_services_by_uuid(sDev->byConnHandle,
                                                              byUUID_Len, byUUID);
        sDev->iErr = sGattRsp->result;
    }
    else if(sDev->byConnState == BUSY)
    {
        sDev->iErr = BT_ERR_BUSY;
    }
    else
    {
        sDev->iErr = BT_ERR_DISCONNECTED;
    }
    
    return sDev->iErr;
}

bt_err_t bt_get_characteristicID(bt_device_t *sDev, uint32_t dwServiceID,
                                 uint8_t byUUID_Len, uint8_t *byUUID) 
{
    struct gecko_msg_gatt_discover_characteristics_by_uuid_rsp_t *sGattRsp;

    sDev->iErr = BT_OK;
  
    if(sDev == NULL) 
    {
        sDev->iErr = BT_ERR_NULLPOINTER;
    } 
    else if(sDev->byConnState == READY)
    {
        sDev->byConnState = BUSY;
        sGattRsp = gecko_cmd_gatt_discover_characteristics_by_uuid(sDev->byConnHandle,
                                                        dwServiceID, byUUID_Len, byUUID);
        sDev->iErr = sGattRsp->result;

    }
    else if(sDev->byConnState == BUSY)
    {
        sDev->iErr = BT_ERR_BUSY;
    }
    else
    {
        sDev->iErr = BT_ERR_DISCONNECTED;
    }

    return sDev->iErr;
}

bt_err_t bt_enable_notification(bt_device_t *sDev, uint16_t wNotifHandle)
{
    struct gecko_msg_gatt_set_characteristic_notification_rsp_t *sGattRsp;

    sDev->iErr = BT_OK;
  
    if(sDev == NULL) 
    {
        sDev->iErr = BT_ERR_NULLPOINTER;
    } 
    else if(sDev->byConnState == READY)
    {
        sDev->byConnState = BUSY;
        sGattRsp = gecko_cmd_gatt_set_characteristic_notification(sDev->byConnHandle,
                                                              wNotifHandle, gatt_notification);
        sDev->iErr = sGattRsp->result;
        if(sDev->iErr == BT_OK)
        {
            sDev->byProcState = BT_PROC_NOTIF;
        }
    }
    else if(sDev->byConnState == BUSY)
    {
        sDev->iErr = BT_ERR_BUSY;
    }
    else
    {
        sDev->iErr = BT_ERR_DISCONNECTED;
    }

    return sDev->iErr;
}

bt_err_t bt_scan(void)
{
	bt_err_t sErr = BT_OK;
	struct gecko_msg_le_gap_discover_rsp_t *sGapState;

	reset_found_devices();
	sGapState = gecko_cmd_le_gap_discover(le_gap_discover_generic);
	sErr = sGapState->result;

	if(sErr == BT_OK) {
	    chVTResetI(&vt1);
	    chVTDoSetI(&vt1, MS2ST(10000), bt_stop_scan, NULL);
	}

	return sErr;
}

static void bt_stop_scan(void *p)
{
    (void)p;

    (void)gecko_cmd_le_gap_end_procedure()->result;
}

bt_err_t bt_connect(bt_device_t *sDev)
{
    struct gecko_msg_le_gap_open_rsp_t *sGapState;
     bt_err_t eErr = BT_OK;
     bd_addr sBT_Addr;

     memcpy(sBT_Addr.addr, sDev->byaBT_Address, 6);
  
    if(sDev == NULL) 
    {
        eErr = BT_ERR_NULLPOINTER;
    } 
    else
    {
        if(sDev->byConnState == DISCONNECTED)
        {
            sDev->byConnState = BUSY;
            struct gecko_msg_sm_set_bondable_mode_rsp_t *check =
                  gecko_cmd_sm_set_bondable_mode(1);
            sDev->iErr = check->result;

            if(sDev->iErr == BT_OK)
            {
                sGapState = gecko_cmd_le_gap_open(sBT_Addr, 0);
                sDev->iErr = sGapState->result;

                if(sDev->iErr == BT_OK) {
                        sDev->byConnHandle = sGapState->connection;
                        sDev->byState = BT_STATE_CONNECT;
                }
            }
        } 
        else 
        {
            sDev->iErr = BT_ERR_CONNECTED;
        }

        eErr = sDev->iErr;
    }
    return eErr;
}

bt_err_t bt_disconnect(bt_device_t *sDev) {
    struct gecko_msg_endpoint_close_rsp_t *sGapState;

    sDev->iErr = BT_OK;
  
    if(sDev == NULL) 
    {
        sDev->iErr = BT_ERR_NULLPOINTER;
    } 
    else if(sDev->byConnState == READY)
    {
        sDev->byConnState = BUSY;
        sGapState = gecko_cmd_endpoint_close(sDev->byConnHandle);
        sDev->iErr = sGapState->result;
    }
    else if(sDev->byConnState == BUSY)
    {
        sDev->iErr = BT_ERR_BUSY;
    }
    else
    {
        sDev->iErr = BT_ERR_DISCONNECTED;
    }

    return sDev->iErr;
}

bt_err_t bt_read_data(bt_device_t *sDev, uint16_t wReadHandle) {
    struct gecko_msg_gatt_read_characteristic_value_rsp_t *sGattState;
    
    sDev->iErr = BT_OK;
  
    if(sDev == NULL) 
    {
        sDev->iErr = BT_ERR_NULLPOINTER;
    } 
    else if(sDev->byConnState == READY)
    {
        sDev->byConnState = BUSY;
        sGattState = gecko_cmd_gatt_read_characteristic_value(
                                             sDev->byConnHandle,
                                             wReadHandle);
        sDev->iErr = sGattState->result;
        if(sDev->iErr == BT_OK)
        {
            sDev->byProcState = BT_PROC_CHAR_READ;
        }
    }
    else if(sDev->byConnState == BUSY)
    {
        sDev->iErr = BT_ERR_BUSY;
    }
    else
    {
        sDev->iErr = BT_ERR_DISCONNECTED;
    }

    return sDev->iErr;
}

bt_err_t bt_write_data(bt_device_t *sDev, uint32_t dwWriteHandle, uint8_t *pdata, uint16_t wLength) {
    struct gecko_msg_gatt_write_characteristic_value_rsp_t *sGattState;

    sDev->iErr = BT_OK;
  
    if(sDev == NULL) 
    {
      sDev->iErr = BT_ERR_NULLPOINTER;
    } 
    else if(sDev->byConnState == READY)
    {
        sDev->byConnState = BUSY;
        sGattState = gecko_cmd_gatt_write_characteristic_value(
                                             sDev->byConnHandle,
                                             dwWriteHandle,
                                             wLength, pdata);
        sDev->iErr = sGattState->result;
        if(sDev->iErr == BT_OK)
        {
            sDev->byProcState = BT_PROC_WRITE;
        }
    }
    else if(sDev->byConnState == BUSY)
    {
        sDev->iErr = BT_ERR_BUSY;
    }
    else
    {
        sDev->iErr = BT_ERR_DISCONNECTED;
    }

    return sDev->iErr;
}

bt_err_t bt_inc_security(bt_device_t *sDev) {
    struct gecko_msg_sm_increase_security_rsp_t *sSecurity;

    sDev->iErr = BT_OK;
  
    if(sDev == NULL) 
    {
        sDev->iErr = BT_ERR_NULLPOINTER;
    } 
    else if(sDev->byConnState == READY)
    {
        sDev->byConnState = BUSY;
        sSecurity = gecko_cmd_sm_increase_security(sDev->byConnHandle);
        sDev->iErr = sSecurity->result;
    }
    else if(sDev->byConnState == BUSY)
    {
        sDev->iErr = BT_ERR_BUSY;
    }
    else
    {
        sDev->iErr = BT_ERR_DISCONNECTED;
    }

    return sDev->iErr;
}

static void addDevice(struct gecko_msg_le_gap_scan_response_evt_t *msg) {
    uint8_t byLength = msg->data.len;
    uint8_t byI, byCount = 0;

    
    for(byI = 0; byI < NUM_DEVICES; byI++) {
        if(comp_array(sDevice[byI].byaBT_Address, msg->address.addr)) {
            break;
        } else {
            byCount++;
        }
    }

    if(byCount == NUM_DEVICES)
    {
        memcpy(sDevice[byNumDevices].byaBT_Address, msg->address.addr, 6);
		byNumDevices++;

        if(BT_DATA_MAXLENGTH >= byLength)
        {

            // Parse data
            for (byI = 0; byI < byLength;) {
                    int8_t iNameLength = msg->data.data[byI++];
                    if (!iNameLength) {
                        continue;
                    }
                    if (byI + iNameLength > byLength) {
                        break; // not enough data
                    }
                    uint8_t byType = msg->data.data[byI++];
                    switch (byType) {
                    case 0x08:
                    case 0x09:
                            memcpy(sDevice[byNumDevices].byaName, (msg->data.data + byI), iNameLength-1);
                            sDevice[byNumDevices].byaName[iNameLength - 1] = '\0';
                            break;
                    default:
                            break;
                    }

                    byI += iNameLength - 1;
            }
        }
    }
}

static void list_devices(void) {
	uint8_t byI;

	for(byI = 0; byI < byNumDevices; byI++) {
		printf("[%d]: %02X:%02X:%02X:%02X:%02X:%02X\n",
				(byI+1), 
                sDevice[byI].byaBT_Address[0],
                sDevice[byI].byaBT_Address[1],
                sDevice[byI].byaBT_Address[2],
                sDevice[byI].byaBT_Address[3],
				sDevice[byI].byaBT_Address[4],
                sDevice[byI].byaBT_Address[5]);
	}
}

static void reset_found_devices(void) {
	uint8_t byI, byP;

	byNumDevices = 0;
	for(byI = 0; byI < NUM_DEVICES; byI++) {
		for(byP = 0; byP < 6; byP++) {
			sDevice[byI].byaBT_Address[byP] = 0;
		}
	}

}

static uint8_t comp_array(uint8_t *pchar1, uint8_t *pchar2) {
    uint8_t byLength1 = sizeof(pchar1);
    uint8_t byLength2 = sizeof(pchar2);
    uint8_t byI;

    if(byLength1 != byLength2) {
        return 0;
    }

    for(byI = 0; byI < byLength1; byI++) {
        if(pchar1[byI] != pchar2[byI]) {
            return 0;
        }
    }

    return 1;
}

void bt_list_bonds(void)
{
    (void)gecko_cmd_sm_list_all_bondings();
}

void bt_delete_bonding(uint8_t bond)
{
    (void)gecko_cmd_sm_delete_bonding(bond);
}

void bt_delete_all_bondings(void)
{
    (void)gecko_cmd_sm_delete_bondings();
}

void bt_set_oob(uint32_t dwKey)
{
    uint8_t oobData[16] = {0,0,0,0,
                             0,0,0,0,
                             0,0,0,0,
                            (dwKey>>24), (dwKey>>16), (dwKey>>8), (dwKey&0xff)};

    (void)gecko_cmd_sm_set_oob_data(16, oobData);
}
