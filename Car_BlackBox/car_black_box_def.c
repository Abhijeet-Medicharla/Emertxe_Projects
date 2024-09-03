#include "main.h"

unsigned char clock_reg[3];
char time[7];
char log[11];
short int pos = -1;
short int sec;
short int min;
unsigned char return_time;

char *menu[] = {"View log", "Download log", "Clear log", "Set time", "Change passwd"};

void display_time(void) {
    get_time();

    clcd_putch(time[0], LINE2(2));
    clcd_putch(time[1], LINE2(3));
    clcd_putch(':', LINE2(4));
    clcd_putch(time[2], LINE2(5));
    clcd_putch(time[3], LINE2(6));
    clcd_putch(':', LINE2(7));
    clcd_putch(time[4], LINE2(8));
    clcd_putch(time[5], LINE2(9));
}

void get_time(void) {
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 

    // HH -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';
    // MM 
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    // SS
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
}

void log_car_event() {
    char addr;
    pos++;
    if (pos == 10) {
        pos = 0;
    }
    addr = (unsigned char) (pos * 10 + 5);

    str_write_eeprom_24c04(addr, log);
}

void log_event(unsigned char event[], unsigned char speed) {
    get_time();
    strncpy(log, time, 6);
    strncpy(&log[6], (const char *)event, 2);
    log[8] = speed / 10 + '0';
    log[9] = speed % 10 + '0';
    log[10] = '\0';
    log_car_event();
}

void display_dashboard(unsigned char event[], unsigned char speed) {
    clcd_print("  TIME     E  SP", LINE1(0));
    // Display time
    display_time();

    clcd_print((const char *)event, LINE2(11));

    clcd_putch(speed / 10 + '0', LINE2(14));
    clcd_putch(speed % 10 + '0', LINE2(15));

}

unsigned char login(unsigned char key, unsigned char reset_flag) {
    static char upasswd[4];
    static unsigned char i;
    char spasswd[4];
    unsigned char attempt_left;
    
    
    if (reset_flag == RESET_PASSWORD) {
        i = 0;
        upasswd[0] = '\0';
        upasswd[1] = '\0';
        upasswd[2] = '\0';
        upasswd[3] = '\0';
        attempt_left = 3;
        key = ALL_RELEASED;
        sec = 0;
        min = 0;
        return_time = 5;
    }

    if (return_time == 0) {
        return RETURN_BACK;
    }

    if (key == SW4 && i < 4) {
        upasswd[i] = '1';
        clcd_putch('*', LINE2(4 + i));
        i++;
        return_time = 5;
    } else if (key == SW5 && i < 4) {
        upasswd[i] = '0';
        clcd_putch('*', LINE2(4 + i));
        i++;
        return_time = 5;
    }

    if (i == 4) {
        for (unsigned char j = 0; j < 4; j++) {
            spasswd[j] = read_eeprom_24c04(j);
        }
        if (strncmp(spasswd, upasswd, 4) == 0) {
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            clcd_print("Login success", LINE1(2));
            __delay_ms(1500);
            clear_screen();
            return LOGIN_SUCCESS;
        } else {
            attempt_left--;
            if (attempt_left == 0) {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                clcd_print("You are blocked", LINE1(0));
                clcd_print("wait for", LINE2(0));
                clcd_putch(':', LINE2(11));
                sec = 59;
                min = 14;
                /* BLOCK LOGIN SCREEN FOR 15 MINS */
                while (min != 0 || sec != 0) {
                    clcd_putch((unsigned char) (min / 10) + '0', LINE2(9));
                    clcd_putch(min % 10 + '0', LINE2(10));
                    clcd_putch((unsigned char) (sec / 10) + '0', LINE2(12));
                    clcd_putch(sec % 10 + '0', LINE2(13));
                    return_time = 5;
                }
                attempt_left = 3;

            } else {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                clcd_print("Wrong Password", LINE1(0));
                clcd_putch(attempt_left + '0', LINE2(0));
                clcd_print("Attempts left", LINE2(2));
                __delay_ms(2000);
            }
            clear_screen();
            clcd_print("Enter Password", LINE1(1));
            clcd_write(LINE2(4), INST_MODE);
            clcd_write(CURSOR_ON_AND_DISP_ON, INST_MODE);
            i = 0;
            return_time = 5;
        }
    }

    return LOGIN_SCREEN;
}

unsigned char display_menu(unsigned char key, unsigned char reset_flag) {
    static unsigned char menu_pos;
    TMR2ON = 1;

    if (reset_flag == RESET_LOGIN_MENU) {
        clear_screen();
        menu_pos = 0;
        return_time = 5;
    }

    if (return_time == 0) {
        TMR2ON = 0;
        return LOGOUT_MENU_SCREEN;
    }

    if (key == SW4 && menu_pos > 0) {
        clear_screen();
        menu_pos--;
        return_time = 5;
    } else if (key == SW5 && menu_pos < 4) {
        clear_screen();
        menu_pos++;
        return_time = 5;
    } else if (key == LONG_SW4) {
        TMR2ON = 0;
        switch (menu_pos) {
            case 0:
                return VIEW_LOG;
                break;
            case 1:
                return DOWNLOAD_LOG;
                break;
            case 2:
                return CLEAR_LOG;
                break;
            case 3:
                return SET_TIME;
                break;
            case 4:
                return CHANGE_PASSWORD;
                break;
        }
    } else if (key == LONG_SW5) {
        TMR2ON = 0;
        return LOGOUT_MENU_SCREEN;
    }


    if (menu_pos == 4) {
        clcd_putch('*', LINE2(0));
        clcd_print(menu[menu_pos - 1], LINE1(2));
        clcd_print(menu[menu_pos], LINE2(2));
    } else {
        clcd_putch('*', LINE1(0));
        clcd_print(menu[menu_pos], LINE1(2));
        clcd_print(menu[menu_pos + 1], LINE2(2));
    }

    return LOGIN_MENU_SCREEN;
}

unsigned char view_log(unsigned char key) {
    static unsigned char log_no = 0;

    if (key == SW4) {
        if (log_no == 0)
            log_no = 9;
        else
            --log_no;

    } else if (key == SW5) {
        ++log_no;
        log_no = log_no % 10;
    } else if (key == LONG_SW4) {
        return LOGIN_MENU_SCREEN;
    } else if (key == LONG_SW5) {
        return DASHBOARD_SCREEN;
    }

    // DISPLAY TIME
    clcd_putch(log_no + '0', LINE1(10));

    clcd_putch(read_eeprom_24c04(log_no * 10 + 5 + 0), LINE2(2));
    clcd_putch(read_eeprom_24c04(log_no * 10 + 5 + 1), LINE2(3));
    clcd_putch(':', LINE2(4));
    clcd_putch(read_eeprom_24c04(log_no * 10 + 5 + 2), LINE2(5));
    clcd_putch(read_eeprom_24c04(log_no * 10 + 5 + 3), LINE2(6));
    clcd_putch(':', LINE2(7));
    clcd_putch(read_eeprom_24c04(log_no * 10 + 5 + 4), LINE2(8));
    clcd_putch(read_eeprom_24c04(log_no * 10 + 5 + 5), LINE2(9));

    // DISPLAY EVENT
    clcd_putch(read_eeprom_24c04(log_no * 10 + 5 + 6), LINE2(11));
    clcd_putch(read_eeprom_24c04(log_no * 10 + 5 + 7), LINE2(12));

    // DISPLAY SPEED
    clcd_putch(read_eeprom_24c04(log_no * 10 + 5 + 8), LINE2(14));
    clcd_putch(read_eeprom_24c04(log_no * 10 + 5 + 9), LINE2(15));

    return VIEW_LOG;
}

unsigned char download_log(void) {
    uart_send_str("No   TIME    E   SP\n\r");

    unsigned char i, j, addr;

    char *download_log = (char *) malloc(11 * sizeof (char));
    char *logs = (char *) malloc(20 * sizeof (char));

    for (i = 0; i < 10; i++) {
        addr = (unsigned char) (i * 10 + 5);
        for (j = 0; j < 10; j++) {
            download_log[j] = read_eeprom_24c04(addr + j);
        }
        download_log[11] = '\0';

        logs[0] = i + '0';
        logs[1] = ' ';
        logs[2] = ' ';
        strncpy(&logs[3], download_log, 2);
        logs[5] = ':';
        strncpy(&logs[6], &download_log[2], 2);
        logs[8] = ':';
        strncpy(&logs[9], &download_log[4], 2);
        logs[11] = ' ';
        logs[12] = ' ';
        strncpy(&logs[13], &download_log[6], 2);
        logs[15] = ' ';
        logs[16] = ' ';
        strncpy(&logs[17], &download_log[8], 2);
        logs[19] = '\0';

        uart_send_str(logs);
        uart_send_str("\n\n\r");
    }

    free(download_log);
    free(log);

    clcd_print("Log Downloaded", LINE1(0));
    clcd_print("Successfully...", LINE2(0));
    __delay_ms(1500);
    clear_screen();

    return LOGIN_MENU_SCREEN;
}

unsigned char clear_log(void) {

    unsigned char i, j, addr;

    for (i = 0; i < 10; i++) {
        addr = (unsigned char) (i * 10 + 5);
        for (j = 0; j < 10; j++) {
            byte_write_eeprom_24c04(addr + j, '0');
        }
    }

    clear_screen();
    clcd_print("Log Cleared", LINE1(2));
    clcd_print("Successfully", LINE2(2));
    __delay_ms(2000);
    
    return LOGIN_MENU_SCREEN;
}

unsigned char set_time(unsigned char key) {
    
    static unsigned char i = 5;
 
    if (key == SW4) {
        switch (i) {
            case 5:
                if (time[i] == '9') {
                    time[i] = '0';
                } else
                    time[i] = time[i] + 1;

                clcd_putch(time[i], LINE2(9));
                SHIFT_CURSOR_LEFT;
                break;

            case 4:
                if (time[i] == '5') {
                    time[i] = '0';
                } else
                    time[i] = time[i] + 1;

                clcd_putch(time[i], LINE2(8));
                SHIFT_CURSOR_LEFT;
                break;
                
            case 3:
                if (time[i] == '9') {
                    time[i] = '0';
                } else
                    time[i] = time[i] + 1;

                clcd_putch(time[i], LINE2(6));
                SHIFT_CURSOR_LEFT;
                break;
                
            case 2:
                if (time[i] == '5') {
                    time[i] = '0';
                } else
                    time[i] = time[i] + 1;

                clcd_putch(time[i], LINE2(5));
                SHIFT_CURSOR_LEFT;
                break;
                
            case 1:
                if (time[i] == '9') {
                    time[i] = '0';
                } else
                    time[i] = time[i] + 1;

                clcd_putch(time[i], LINE2(3));
                SHIFT_CURSOR_LEFT;
                break;
                
            case 0:
                if (time[i] == '2') {
                    time[i] = '0';
                } else
                    time[i] = time[i] + 1;

                clcd_putch(time[i], LINE2(2));
                SHIFT_CURSOR_LEFT;
                break;
        }

    } else if (key == SW5) {

        if (i == 0) {
            i = 5;
        } else {
            --i;
        }

        switch (i) {
            case 0:
                clcd_write(LINE2(2), INST_MODE);
                break;
            case 1:
                clcd_write(LINE2(3), INST_MODE);
                break;
            case 2:
                clcd_write(LINE2(5), INST_MODE);
                break;
            case 3:
                clcd_write(LINE2(6), INST_MODE);
                break;
            case 4:
                clcd_write(LINE2(8), INST_MODE);
                break;
            case 5:
                clcd_write(LINE2(9), INST_MODE);
                break;
        }
    } else if (key == SW6) {
        clear_screen();
        write_ds1307(SEC_ADDR, (unsigned char)((time[5] & 0x0F) | ((time[4] & 0x0F) << 4)));
        write_ds1307(MIN_ADDR, (unsigned char)((time[3] & 0x0F) | ((time[2] & 0x0F) << 4)));
        write_ds1307(HOUR_ADDR,(unsigned char)((time[1] & 0x0F) | ((time[0] & 0x0F) << 4)));
        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
        return LOGIN_MENU_SCREEN;
    } 
    
    return SET_TIME;
}

unsigned char change_passwd(unsigned char key, unsigned char reset_flag) {
    static char upasswd[4];
    char spasswd[4];
    static char newpasswd[4];
    static char re_enter[4];
    static char once = 1, once0 = 1;
    static unsigned char i = 0, n = 0, r = 0;

    if (reset_flag == RESET_PASSWORD) {
        i = 0;
        n = 0;
        r = 0;
        upasswd[0] = '\0';
        upasswd[1] = '\0';
        upasswd[2] = '\0';
        upasswd[3] = '\0';
        newpasswd[0] = '\0';
        newpasswd[1] = '\0';
        newpasswd[2] = '\0';
        newpasswd[3] = '\0';
        re_enter[0] = '\0';
        re_enter[1] = '\0';
        re_enter[2] = '\0';
        re_enter[3] = '\0';

        key = ALL_RELEASED;
        return_time = 5;
        once = once0 = 1;
    }

    if (return_time == 0) {
        return RETURN_BACK;
    }

    if (key == SW4 && i < 4) {
        upasswd[i] = '1';
        clcd_putch('*', LINE2(4 + i));
        i++;
        return_time = 5;
    } else if (key == SW5 && i < 4) {
        upasswd[i] = '0';
        clcd_putch('*', LINE2(4 + i));
        i++;
        return_time = 5;
    }

    if (i == 4) {
        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
        for (unsigned char j = 0; j < 4; j++) {
            spasswd[j] = read_eeprom_24c04(j);
        }
        if (strncmp(spasswd, upasswd, 4) == 0) {
            if(once)
            {
                clear_screen();
                clcd_print("Enter New Passwd", LINE1(0));
                clcd_write(LINE2(4), INST_MODE);
                clcd_write(CURSOR_ON_AND_DISP_ON, INST_MODE);
                once = 0;
            }
            else
            {
               if (key == SW4 && n < 4) {
                    newpasswd[n] = '1';
                    clcd_putch('*', LINE2(4 + n));
                    n++;
                    return_time = 5;
                } else if (key == SW5 && n < 4) {
                    newpasswd[n] = '0';
                    clcd_putch('*', LINE2(4 + n));
                    n++;
                    return_time = 5;
                } 
            }

            if (n == 4) {
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                if(once0)
                {
                    clear_screen();
                    clcd_print("Re-enter Passwd", LINE1(0));
                    clcd_write(LINE2(4), INST_MODE);
                    clcd_write(CURSOR_ON_AND_DISP_ON, INST_MODE);
                    once0 = 0;
                }
                else
                {
                    if (key == SW4 && r < 4) {
                        re_enter[r] = '1';
                        clcd_putch('*', LINE2(4 + r));
                        r++;
                        return_time = 5;
                    } else if (key == SW5 && r < 4) {
                        re_enter[r] = '0';
                        clcd_putch('*', LINE2(4 + r));
                        r++;
                        return_time = 5;
                    }
                }

                if (r == 4) {
                    if (strncmp(newpasswd, re_enter, 4) == 0) {
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        str_write_eeprom_24c04(0x00, newpasswd);
                        clcd_print("Password", LINE1(2));
                        clcd_print("Changed...", LINE2(2));
                        __delay_ms(2000);
                        return LOGIN_MENU_SCREEN;

                    } else {
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        clcd_print("Passwords", LINE1(0));
                        clcd_print("Not Matching", LINE2(0));
                        __delay_ms(1500);
                        clcd_print("Password", LINE1(0));
                        clcd_print("Not Changed", LINE2(0));
                        __delay_ms(1500);
                        clear_screen();
                        return LOGIN_MENU_SCREEN;
                    }
                }
            }
        } else {
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            clcd_print("Wrong Password", LINE1(0));
            __delay_ms(1500);
            clcd_print("Logging Out...", LINE2(2));
            __delay_ms(1500);
            clear_screen();
            return DASHBOARD_SCREEN;
        }
    }
    return CHANGE_PASSWORD;
}

void clear_screen(void) {
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);
}