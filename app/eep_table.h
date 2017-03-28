/*
 * eep_table.h

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

#ifndef APP_EEP_TABLE_H_
#define APP_EEP_TABLE_H_

/**
 * @brief eeprom address definitions
 */
#define CTL_START   0x00

#define EEP_CTL_ID          CTL_START
#define EEP_CTL_IP          CTL_START+1
#define EEP_CTL_SUBNET      EEP_CTL_IP+4
#define EEP_CTL_GATEWAY     EEP_CTL_SUBNET+4
#define EEP_CTL_DHCP        EEP_CTL_GATEWAY+4
#define EEP_CTL_NAME        EEP_CTL_DHCP+1
#define EEP_CTL_DEVICES     EEP_CTL_NAME+25
#define EEP_CTL_ROOMS       EEP_CTL_DEVICES+1
#define EEP_CTL_TYPES       EEP_CTL_ROOMS+1
#define EEP_CTL_SW_VERS     EEP_CTL_TYPES+1
#define EEP_CTL_HW_REV      EEP_CTL_SW_VERS+3

#define EEP_DEV_START   0x32

#define EEP_RM_START    0x1450

#define EEP_TP_START    0x1770

/**
 * @brief controller config type
 */
typedef struct
{
  uint8_t   byID;
  uint8_t   byaIP[4];
  uint8_t   byaSubnet[4];
  uint8_t   byaGateway[4];
  uint8_t   byDHCP;
  uint8_t   byaName[25];
  uint8_t   byDevices;
  uint8_t   byRooms;
  uint8_t   byTypes;
  uint8_t   byaSW[3];
  uint8_t   byHW;
} ctl_conf_t;

/**
 * @brief device config type
 */
typedef struct
{
  uint8_t   byID;
  uint8_t   byaAddress[6];
  uint8_t   byType;
  uint8_t   bySecurity;
  uint8_t   byRoomID;
  uint8_t   byName[25];
  uint8_t   byNameLength;
  uint32_t  dwKey;
} dev_conf_t;

/**
 * @brief room config type
 */
typedef struct
{
  uint8_t   byID;
  uint8_t   byaName[25];
  uint8_t   byNameLength;
} room_conf_t;

/**
 * @brief device type config type
 */
typedef struct
{
  uint8_t   byID;
  uint8_t   byaName[25];
  uint8_t   byNameLength;
  uint8_t   byaBTAddr[3];
  uint8_t   bySecurity;
} type_conf_t;

#endif /* APP_EEP_TABLE_H_ */
