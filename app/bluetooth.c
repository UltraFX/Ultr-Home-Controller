/*
 * bluetooth.c

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
#include "bluetooth.h"
#include "bgm11x/bgm11x.h"
#include "app/eep_table.h"
#include "lib/eeprom.h"

/********************* LOCAL VARIABLES ****************************************/
bt_device_t sDevice[NUM_DEVICES];


/********************* FUNCTION DEFINITIONS ***********************************/
uint8_t init_device(bt_device_t *sDev) {


    return 0;
}

uint8_t loadCtrlConfig(void) {
  uint8_t byaDummy[25];
  int8_t chRet = 0;

  chRet = eep_read_byte(EEP_CTL_ID, byaDummy);
  if (chRet != 0) {
    return chRet;
  }
  chRet = eep_read_array(EEP_CTL_IP, byaDummy, 4);
  if (chRet != 0) {
    return chRet;
  }
  chRet = eep_read_array(EEP_CTL_SUBNET, byaDummy, 4);
  if (chRet != 0) {
    return chRet;
  }
  chRet = eep_read_array(EEP_CTL_GATEWAY, byaDummy, 4);
  if (chRet != 0) {
    return chRet;
  }
  chRet = eep_read_byte(EEP_CTL_DHCP, byaDummy);
  if (chRet != 0) {
    return chRet;
  }
  chRet = eep_read_array(EEP_CTL_NAME, byaDummy, 25);
  if (chRet != 0) {
    return chRet;
  }
  chRet = eep_read_byte(EEP_CTL_DEVICES, byaDummy);
  if (chRet != 0) {
    return chRet;
  }
  chRet = eep_read_byte(EEP_CTL_ROOMS, byaDummy);
  if (chRet != 0) {
    return chRet;
  }
  chRet = eep_read_byte(EEP_CTL_TYPES, byaDummy);
  if (chRet != 0) {
    return chRet;
  }
  chRet = eep_read_array(EEP_CTL_SW_VERS, byaDummy, 3);
  if (chRet != 0) {
    return chRet;
  }
  chRet = eep_read_byte(EEP_CTL_HW_REV, byaDummy);

  return chRet;
}

uint8_t loadDevices(void) {

  uint8_t byDeviceNum = 0;
  uint16_t eepOffset = 0;
  int8_t chRet = 0;

  for (byDeviceNum = 0; byDeviceNum < NUM_DEVICES; byDeviceNum++) {
    eepOffset = EEP_DEV_START + (byDeviceNum * sizeof(dev_conf_t));
    chRet = eep_read_byte(eepOffset, &sDevice[byDeviceNum].byID);
    if (chRet != 0) {
      break;
    }
    eepOffset += 1;
    chRet = eep_read_array(eepOffset, sDevice[byDeviceNum].byaBT_Address, 6);
    if (chRet != 0) {
      break;
    }
    eepOffset += 6;
    chRet = eep_read_byte(eepOffset, &sDevice[byDeviceNum].byType);
    if (chRet != 0) {
      break;
    }
    eepOffset += 1;
    chRet = eep_read_byte(eepOffset, &sDevice[byDeviceNum].bySecurity);
    if (chRet != 0) {
      break;
    }
    eepOffset += 1;
    chRet = eep_read_byte(eepOffset, &sDevice[byDeviceNum].byRoomID);
    if (chRet != 0) {
      break;
    }
    eepOffset += 1;
    chRet = eep_read_array(eepOffset, sDevice[byDeviceNum].byaName, 25);
    if (chRet != 0) {
      break;
    }
    eepOffset += 25;
    chRet = eep_read_byte(eepOffset, &sDevice[byDeviceNum].byNameLength);
    if (chRet != 0) {
      break;
    }
    eepOffset += 1;
    chRet = eep_read_dword(eepOffset, &sDevice[byDeviceNum].dwKey);
    eepOffset += 4;
  }

  return chRet;
}

uint8_t loadRooms(void) {
  int8_t chRet = 0;
  uint8_t byaDummy[25];
  uint8_t byRoomNum;
  uint16_t weepOffset;

  for (byRoomNum = 0; byRoomNum < 25; byRoomNum++) {
    weepOffset = EEP_RM_START + (byRoomNum * sizeof(room_conf_t));
    chRet = eep_read_byte(weepOffset, byaDummy); //ID
    if (chRet != 0) {
      break;
    }
    weepOffset += 1;
    chRet = eep_read_array(weepOffset, byaDummy, 25); //Name
    if (chRet != 0) {
      break;
    }
    weepOffset += 6;
    chRet = eep_read_byte(weepOffset, byaDummy); //NameLength
  }

  return chRet;
}

uint8_t loadTypes(void) {
  int8_t chRet = 0;
  uint8_t byDummy[25];
  uint8_t byTypeNum;
  uint16_t weepOffset;

  for (byTypeNum = 0; byTypeNum < 25; byTypeNum++) {
    weepOffset = EEP_RM_START + (byTypeNum * sizeof(type_conf_t));
    chRet = eep_read_byte(weepOffset, byDummy); //ID
    if (chRet != 0) {
      break;
    }
    weepOffset += 1;
    chRet = eep_read_array(weepOffset, byDummy, 25); //Name
    if (chRet != 0) {
      break;
    }
    weepOffset += 6;
    chRet = eep_read_byte(weepOffset, byDummy); //NameLength
    if (chRet != 0) {
      break;
    }
    weepOffset += 1;
    chRet = eep_read_array(weepOffset, byDummy, 3); //BT Addr
    if (chRet != 0) {
      break;
    }
    weepOffset += 3;
    chRet = eep_read_byte(weepOffset, byDummy); //Security
    weepOffset += 1;
  }

  return chRet;
}
