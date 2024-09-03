#include <xc.h>

extern short int sec;
extern unsigned char return_time;
extern short int min;

void __interrupt() isr(void)
{
    static unsigned int count = 0;
    
    if (TMR2IF == 1)
    {
        if (++count == 1250)
        {
            count = 0;
            
            if(--sec < 0)
            {
                sec = 59;
                if(--min < 0)
                {
                    min = 14;
                }
            }
            
            if(return_time > 0)
            {
                return_time--;
            }   
        }
        TMR2IF = 0;
    }
}