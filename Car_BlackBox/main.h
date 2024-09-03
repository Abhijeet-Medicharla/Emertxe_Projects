#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include "adc.h"
#include "ds1307.h"
#include "clcd.h"
#include "digital_keypad.h"
#include "i2c.h"
#include "car_black_box.h"
#include "eeprom_24c04.h"
#include "timers.h"
#include "uart.h"

#define DASHBOARD_SCREEN              0x11
#define LOGIN_SCREEN                  0x12
#define LOGIN_MENU_SCREEN             0x13
#define LOGOUT_MENU_SCREEN            RETURN_BACK

#define RETURN_BACK                   0x02
#define RESET_PASSWORD                0x03
#define RESET_NOTHING                 0x04
#define LOGIN_SUCCESS                 0x05
#define RESET_LOGIN_MENU              0x06
#define VIEW_LOG                      0x07
#define CLEAR_LOG                     0x09
#define DOWNLOAD_LOG                  0x08
#define SET_TIME                      0x0A
#define CHANGE_PASSWORD               0x0B
#define RESET_SET_TIME                0x0C


#endif	/* MAIN_H */

