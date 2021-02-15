#include "exynos_4412.h"
#include "taskcpu.h"
#include "uart.h"
#include "mpcore.h"
extern void itoh(unsigned int n,char *s);
extern unsigned int _uart3_lock;
int main (void)  
{  
unsigned int value =0;
char hex[20];  

uart3_init();
put_debug("initialized uart3\n\r");

put_debug("core count from scu:");
scu_enable();
value=scu_get_core_count();
itoh(value,hex);
put_debug(hex);
put_debug("\n\r");

put_debug("prepare cpus...\n\r");
platform_smp_prepare_cpus(4);

put_debug("enable softirq ...\n\r");
gicv2_cpux_enable_softirq();

put_debug("boot secondary...\n\r");
for(int i=1;i<4;i++){
  value=boot_secondary(i);
  itoh(i,hex);
  put_debug(hex);
  if(value==0)put_debug(" boot secondary ok\n\r");
  else if(value==-2) put_debug(" power up secondary fail\n\r");
  else put_debug(" secondary boot fail\n\r");
}
task_print("info from cpu0\n\r");

while(1) {}  
return 0;  
}  
