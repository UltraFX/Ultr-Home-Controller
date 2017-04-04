/**
  * @brief Bluetooth driver for EQ-3 temperature controllers
  *
  * @author     Nicolas Dammin
  */

#ifndef DRV_EQ3_H
#define DRV_EQ3_H

/********************* INCLUDES ***********************************************/
#include "app/bluetooth.h"

/********************* TYPES **************************************************/
typedef struct
{
    float   fTemperature;
}eq3_data_t;

/********************* GLOBAL FUNCTION PROTOTYPES *****************************/

/**
  * @brief Initialize bluetooth device instance for eq-3
  *
  * @param[in]  pointer to bluetooth device instance
  */
void eq3_init(bt_device_t *sDev);

/**
  * @brief connect to eq-3 device
  *
  * @param[in]  pointer to bluetooth device instance
  * @return     0 or error code
  */
int16_t eq3_connect(bt_device_t *sDev);

/**
  * @brief disconnect from eq-3 device
  *
  * @param[in]  pointer to bluetooth device instance
  * @return     0 or error code
  */
int16_t eq3_disconnect(bt_device_t *sDev);

/**
  * @brief set new temperature on eq-3 device
  *
  * @param[in]  pointer to bluetooth device instance
  * @param[in]  temperature in 0.5 °C steps
  * @return     0 or error code
  */
int16_t eq3_set_temperature(bt_device_t *sDev, float temperature);

/**
 *
 * @brief get current temperature value from eq-3 device
 *
 * @param[in] pointer to bluetooth device instance
 * @return    0 or error code
 */
int16_t eq3_get_temperature(bt_device_t *sDev);

/**
  * @brief set automatic or manual mode on eq-3 device
  *
  * @param[in]  pointer to bluetooth device instance
  * @param[in]  manual or automatic mode
  * @return     0 or error code
  */
int16_t eq3_set_mode(bt_device_t *sDev, uint8_t byMode);

/**
  * @brief handle data of received notification
  *
  * @param[in]  pointer to bluetooth device instance
  * @param[in]  event-type of notification
  */
void eq3_handle_notification(bt_device_t *sDev, uint32_t *pData);

/**
  * @brief event handler of eq-3 device containing state machine
  * @detail This has to be called periodically in superloop or thread
  *
  * @param[in]  pointer to bluetooth device instance
  */
void eq3_handler(bt_device_t *sDevice);

/**
  * @brief check the connection state of the device
  *
  * @param[in]   pointer to bluetooth device instance
  * @return     connection state (CONNECTED, DISCONNECTED, BUSY, READY)
  */
uint8_t eq3_check_state(bt_device_t *sDev);

#endif
