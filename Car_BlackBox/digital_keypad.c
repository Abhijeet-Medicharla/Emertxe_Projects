#include "main.h"

void init_digital_keypad(void)
{
    /* Set Keypad Port as input */
    KEYPAD_PORT_DDR = KEYPAD_PORT_DDR | INPUT_LINES;
}

unsigned char read_digital_keypad(unsigned char mode)
{
    static unsigned char once = 1;
    static unsigned short int delay = 0;
    static unsigned short int delay1 = 0;
    
    if (mode == LEVEL_DETECTION)
    {
        if((KEYPAD_PORT & INPUT_LINES) == SW4)
        {
            delay++;
            if(delay > 20)
            {
                delay = 0;
                return LONG_SW4;
            }
        }
        else if((KEYPAD_PORT & INPUT_LINES) == SW5)
        {
            delay1++;
            if(delay1 > 20)
            {
                delay1 = 0;
                return LONG_SW5;
            }     
        }
        else if((KEYPAD_PORT & INPUT_LINES) == ALL_RELEASED)
        {
            if(delay <= 20 && delay != 0)
            {
                delay = 0;
                return SW4;
            }
            else if(delay1 <= 20 && delay1 != 0)
            {
                delay1 = 0;
                return SW5;
            }
            else 
            {
                delay = 0;
                delay1 = 0; 
                return ALL_RELEASED;
            }
        }
        else
        {
            return KEYPAD_PORT & INPUT_LINES;
        }     
    }
    else
    {
        if (((KEYPAD_PORT & INPUT_LINES) != ALL_RELEASED) && once)
        {
            once = 0;
            
            return KEYPAD_PORT & INPUT_LINES;
        }
        else if ((KEYPAD_PORT & INPUT_LINES) == ALL_RELEASED)
        {
            once = 1;
        }
    }
    
    return ALL_RELEASED;
}