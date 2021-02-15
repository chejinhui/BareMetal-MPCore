#include "exynos_4412.h"
#include "mpcore.h"

volatile int pen_release = -1;
int ncores=1;

volatile unsigned int _uart3_lock=0;

struct  _cpu_boot_info cpu_boot_info[4];

/* scu should be enabled for cpus cache coherent */
void  scu_enable(){
  unsigned int scu_ctrl;
  unsigned int scu_base=EXYNOS4_PA_COREPERI;

  if ((read_cpuid(CPUID_ID) & 0xff0ffff0) == 0x410fc090) {
		scu_ctrl = *((unsigned int*)(scu_base + 0x30));
		if (!(scu_ctrl & 1))
	        *((unsigned int*)(scu_base + 0x30))=scu_ctrl | 0x1;
   }
   scu_ctrl = *((unsigned int*)(scu_base + SCU_CTRL));
	/* already enabled? */
	if (scu_ctrl & 1)
		return;

	scu_ctrl |= 1;
        *((unsigned int*) (scu_base+SCU_CTRL))=scu_ctrl;

}
/* get core count from snoop control unit,should be 4 */
unsigned int scu_get_core_count(void)
{
        unsigned int scu_base=EXYNOS4_PA_COREPERI; 
	unsigned int ncores =*((unsigned int*)(scu_base + SCU_CONFIG));
	return (ncores & 0x03) + 1;
}
/* get core count from L2 cache control register,should be 4 */
unsigned int exynos_get_core_count(void)
{
	unsigned int  val;

	/* Read L2 control register */
	asm volatile("mrc p15, 4, %0, c15, c0, 0" : "=r"(val));

	/* core count : [25:24] of L2 control register + 1 */
	val = ((val >> 24) & 3) + 1;

	return val;
}
/* power up cpu througn pmu*/
int exynos_power_up_cpu(unsigned int cpu){
        int timeout=100;
	unsigned int val;
	void *power_base = cpu_boot_info[cpu].power_base;

	val =*((unsigned int*) power_base);
	if (!(val & S5P_CORE_LOCAL_PWR_EN)) {
	         *((unsigned int*) power_base)=S5P_CORE_LOCAL_PWR_EN;

		/* wait max 10 ms until cpu is on */
		while (timeout) {
			val = *((unsigned int*) (power_base +4));

			if ((val & S5P_CORE_LOCAL_PWR_EN) == S5P_CORE_LOCAL_PWR_EN)
				break;

			__delay();
			timeout--;
		}

		if (timeout == 0) {
			return -1;
		}
	}

	return 0;
}
/* initialize cpu resources */
void platform_smp_prepare_cpus(unsigned int cpus){

          int i;

	/* Set up secondary boot base and core power cofiguration base address */
	for (i = 1; i < cpus; i++) {
		cpu_boot_info[i].boot_base =  EXYNOS4_PA_SYSRAM_NS_4212+ 0x1C;
			cpu_boot_info[i].boot_base += (0x4 * i);
		cpu_boot_info[i].power_base = S5P_ARM_CORE_CONFIGURATION(i);
	}
}
int boot_secondary(unsigned int cpu){
        int times=100;
	int ret;


	ret = exynos_power_up_cpu(cpu);
	if (ret) {
		return -2;
	}
	
     pen_release=cpu;

	while (times) {
        //enter security world and power up the cpu
	    exynos_smc(SMC_CMD_CPU1BOOT, cpu, 0, 0);
        /*set secondary cpu boot address.we can learn from samsung's kernel source code that the PA address is
        EXYNOS4_PA_SYSRAM_NS_4212,not EXYNOS4_PA_SYSRAM_NS*/
        *((unsigned int*)(cpu_boot_info[cpu].boot_base))=exynos_secondary_startup;
        //send secondary cpu a soft interrupt to wake it up 
	     gicv2_wakeup_secondary(cpu,0);
        //wether the cpu has been waken up  
		if (pen_release == -1)
			break;

		__delay();
                times--;
	}

	return pen_release != -1 ? -1 : 0;
}
int gicv2_cpux_enable_softirq()
{

    ICDDCR = 0x1; // Bit1: GIC monitors the peripheral interrupt signals and
    CPU1.ICCICR |= 0x1; //Global enable for signaling of interrupts
    CPU1.ICCPMR =  0xFF; //The priority mask level.Priority filter. threshold
    CPU2.ICCICR |= 0x1; //Global enable for signaling of interrupts
    CPU2.ICCPMR = 0xFF; //The priority mask level.Priority filter. threshold
    CPU3.ICCICR |= 0x1; //Global enable for signaling of interrupts
    CPU3.ICCPMR = 0xFF; //The priority mask level.Priority filter. threshold
    return  0;
}
void gicv2_wakeup_secondary(unsigned int cpu,unsigned int sgi_id){
    //send a soft innterrupt to secondary cpu
    ICDSGIR=0x01<<(cpu+16)|sgi_id;
}

unsigned int  uart3_lock(void){
  return  __raw_spin_lock(&_uart3_lock);
  
}
void uart3_unlock(void){
  __raw_spin_unlock(&_uart3_lock);

}
int do_irq()
{
  return 0;
}

