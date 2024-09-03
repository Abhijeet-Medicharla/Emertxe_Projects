/*
 * File : main.c
 * Name : Abhijeet Medicharla
 * Id   : 23031_003 
 */

#include "main.h"

#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled)

static void init_config(void) {
    // Add all init functions
    init_i2c(100000);
    init_ds1307();
    init_clcd();
    init_adc();
    init_digital_keypad();
    init_timer2();
    init_uart(9600);

    GIE = 1;
    PEIE = 1;
}

void main(void) {

    init_config();

    str_write_eeprom_24c04(0x00, "1111");

    char event[3] = "ON";
    unsigned char speed = 0;
    unsigned char control_flag = DASHBOARD_SCREEN, key, reset_flag;
    
    char *gear[] = {"GN", "GR", "G1", "G2", "G3", "G4"};
    unsigned char gr = 0;
    log_event(event, speed);

    while (1) {
        
        if(control_flag == CHANGE_PASSWORD || control_flag == LOGIN_SCREEN)
        {
            key = read_digital_keypad(STATE);
        }
        else
        {
            key = read_digital_keypad(LEVEL);
        }
        
        if (key == SW1) {
            strcpy(event, "C ");
            log_event(event, speed);
        } else if (key == SW2 && gr < 6) {
            strcpy(event, gear[gr]);
            gr++;
            log_event(event, speed);
        } else if (key == SW3 && gr > 0) {
            gr--;
            strcpy(event, gear[gr]);
            log_event(event, speed);
        } else if ((key == SW4 || key == SW5) && control_flag == DASHBOARD_SCREEN) {
            control_flag = LOGIN_SCREEN;
            clear_screen();
            clcd_print("Enter Password", LINE1(1));
            clcd_write(LINE2(4), INST_MODE);
            clcd_write(CURSOR_ON_AND_DISP_ON, INST_MODE);
            reset_flag = RESET_PASSWORD;
            /* Switching on the Timer2 */
            TMR2ON = 1;
        }

        switch (control_flag) {
            case DASHBOARD_SCREEN:
                display_dashboard(event, speed);
                break;

            case LOGIN_SCREEN:
                TMR2ON = 1;
                switch (control_flag = login(key, reset_flag)) {
                    case RETURN_BACK:
                        clear_screen();
                        control_flag = DASHBOARD_SCREEN;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        TMR2ON = 0;
                        break;
                    case LOGIN_SUCCESS:
                        control_flag = LOGIN_MENU_SCREEN;
                        reset_flag = RESET_LOGIN_MENU;
                        
                        break;
                }
                break;

            case LOGIN_MENU_SCREEN:
                switch (display_menu(key, reset_flag)) {
                    case LOGOUT_MENU_SCREEN:
                        clear_screen();
                        control_flag = DASHBOARD_SCREEN;
                        break;
                    case VIEW_LOG:
                        clear_screen();
                        control_flag = VIEW_LOG;
                        break;

                    case DOWNLOAD_LOG:
                        clear_screen();
                        control_flag = DOWNLOAD_LOG;
                        break;

                    case CLEAR_LOG:
                        clear_screen();
                        control_flag = CLEAR_LOG;
                        clcd_print("Clearing log...", LINE1(0));
                        break;

                    case SET_TIME:
                        clear_screen();
                        clcd_print("Setting Time", LINE1(2));
                        __delay_ms(1500);
                        control_flag = SET_TIME;
                        display_time();
                        clcd_write(LINE2(9), INST_MODE);
                        clcd_write(CURSOR_ON_AND_DISP_ON, INST_MODE);
                        set_time(ALL_RELEASED);
                        break;

                    case CHANGE_PASSWORD:
                        clear_screen();
                        control_flag = CHANGE_PASSWORD;
                        reset_flag = RESET_PASSWORD;
                        clcd_print("Enter Password", LINE1(1));
                        clcd_write(LINE2(4), INST_MODE);
                        clcd_write(CURSOR_ON_AND_DISP_ON, INST_MODE);
                        break;
                }
                break;
                
            case VIEW_LOG:
                clcd_print("  LOG: ", LINE1(0));
                control_flag = view_log(key);
                if(control_flag == LOGIN_MENU_SCREEN)
                {
                    reset_flag = RESET_LOGIN_MENU;
                    display_menu(key, reset_flag);
                }
                break;
                
            case DOWNLOAD_LOG:
                control_flag = download_log();
                if(control_flag == LOGIN_MENU_SCREEN)
                {
                    reset_flag = RESET_LOGIN_MENU;
                    display_menu(key, reset_flag);
                }
                break;
            
            case CLEAR_LOG:
                control_flag = clear_log();
                clear_screen();
                if(control_flag == LOGIN_MENU_SCREEN)
                {
                    reset_flag = RESET_LOGIN_MENU;
                    display_menu(key, reset_flag);
                }
                break;
                
            case SET_TIME:
                control_flag = set_time(key);
                if(control_flag == LOGIN_MENU_SCREEN)
                {
                    reset_flag = RESET_LOGIN_MENU;
                    display_menu(key, reset_flag);
                }
                break;
                
            case CHANGE_PASSWORD:
                control_flag = change_passwd(key, reset_flag);
                if(control_flag == LOGIN_MENU_SCREEN)
                {
                    reset_flag = RESET_LOGIN_MENU;
                    display_menu(key, reset_flag);
                }
                break;
        }

        reset_flag = RESET_NOTHING;

        speed = (unsigned char) (read_adc() / 10);
        if (speed > 99) {
            speed = 99;
        }

    }
    return;
}
