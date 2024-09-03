#ifndef CAR_BLACK_BOX_H
#define	CAR_BLACK_BOX_H

void display_dashboard(unsigned char event[], unsigned char speed);
void get_time(void);
void display_time(void);
void log_event(unsigned char event[], unsigned char speed);
void clear_screen(void);
unsigned char login(unsigned char key, unsigned char reset_flag);
unsigned char display_menu(unsigned char key, unsigned char reset_flag);

unsigned char change_passwd(unsigned char key, unsigned char reset_flag);
unsigned char clear_log(void);
unsigned char download_log(void);
unsigned char view_log(unsigned char key);
unsigned char set_time(unsigned char key);


#endif	/* CAR_BLACK_BOX_H */

