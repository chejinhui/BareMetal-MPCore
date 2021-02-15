#include "exynos_4412.h"
#include "uart.h"
void uart3_gpio_conf(void)
{
//GPA0.CON = (GPA0.CON & ~0xff0000) | 0x220000;
//PA0.CON = 0x22222222;
//GPA1.CON = (GPA1.CON & ~0xFF0000 ) | (0x220000);
}

void uart3_init(void)
{
      

	uart3_gpio_conf();

  MPLL_CON0 = (1<<31 | 0x64<<16 | 0x3<<8 | 0x0);
  CLK_SRC_DMC = 0x00011000;
  CLK_SRC_TOP1 = 0x01111000;
  __REG(0x10030000+0xc250)=0x666666;
  __REG(0x10030000+0xc550)=0x777777;

  UART3.ULCON = 0x3;   
  UART3.UCON =0x3c5;// 0x5;  
  UART3.UFCON=0x111;
  UART3.UMCON=0x00;
  UART3.UBRDIV =53;
  UART3.UFRACVAL = 3;
  UART3.UINTP=0x05;
  UART3.UINTSP=0x05;
}

int uart3_getc(void)
{
	int ch;
	while( !(UART3.UTRSTAT & 0x1));

	ch = UART3.URXH & 0xFF;

	return ch;
}

int uart3_putc(int ch)
{
	while( !(UART3.UTRSTAT & 0x2));
	UART3.UTXH = ch & 0xFF;

	return ch & 0xFF;
}

int puts(const char *str)
{
	int cnt = 0;
	while(*str) {
		uart3_putc(*str);

		++str;
		++cnt;
	}

	return cnt;
} 
  
void gets(char *p)  
{  
    char data;  
    while((data = uart3_getc())!= '\r')  
        *p++ = data;  
    if(data == '\r')  
        *p++ = '\r';  
    *p = '\0';  
}  

void put_debug(char* info){
 while(*info != '\0')
        uart3_putc(*info++);
}
