#include "uart.h"

#ifndef __CPUTASK_
#define _CPUTASK_

//GPL2_0, GPK1_1
#define         GPL2CON         (*(volatile unsigned long *) 0x11000100)
#define         GPL2DAT         (*(volatile unsigned long *) 0x11000104)
#define         GPK1CON         (*(volatile unsigned long *) 0x11000060)
#define         GPK1DAT         (*(volatile unsigned long *) 0x11000064)

void task_ledblink(int which);
void task_print(char* info);
#endif
