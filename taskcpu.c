
#include "taskcpu.h"

static void delay(int r0)
{
    volatile int count = r0;
    while (count--);
}
/*cpu1's task is  blinking a led,and cpu2's task is blinking the other led */
void task_ledblink(int which)
{

        if(which==0)
        GPL2CON = 0x00000001;
        else
	GPK1CON = 0x00000010;
	
	while(1)							
	{
	        if(which==0)
         	  GPL2DAT = 1;
                else{
		  GPK1DAT = 0;
                  delay(0x80000);
                } 
                 
		delay(0x80000);

	       	if(which==0)
                  GPL2DAT = 0;
                else
		  {
                   GPK1DAT = 0x2;
                   delay(0x80000);
                  }
		delay(0x80000);
	}
}
/*cpu0's and cpu3's task is print text to uart3.each cpu must acquire the spin lock before it access to this shared resource*/
void task_print(char* info){
 
  while(1){
    uart3_lock();
    put_debug(info);
    uart3_unlock();
    delay(0x1f0000); 
    
  }
}

