#ifndef __MPCORE__
#define __MPCORE__

typedef unsigned int u32;

#define __stringify_1(x) #x
#define __stringify(x)  __stringify_1(x)

#define EXYNOS4_PA_SYSRAM		0x02020000
#define EXYNOS4_PA_SYSRAM_NS		0x0203F000
#define EXYNOS4_PA_SYSRAM_NS_4212	0x0204F000


#define EXYNOS4_PA_PMU			0x10020000
#define S5P_PMUREG(x)				(EXYNOS4_PA_PMU + (x))
#define S5P_ARM_CORE0_CONFIGURATION		S5P_PMUREG(0x2000)
#define S5P_ARM_CORE_CONFIGURATION(_nr)		(S5P_ARM_CORE0_CONFIGURATION + ((_nr) * 0x80))

#define S5P_CORE_LOCAL_PWR_EN   0x03
#define SCU_CTRL		0x00
#define SCU_CONFIG		0x04
#define SCU_CPU_STATUS		0x08
#define SCU_INVALIDATE		0x0c
#define SCU_FPGA_REVISION	0x10
#define SCU_FILTER_START	0x40
#define SCU_FILTER_END		0x44
#define EXYNOS4_PA_COREPERI		0x10500000
#define CPUID_ID 0

#define EINVAL 1
#define locked 1
#define unlocked 0
#define DEFAULT_MAX_IRQ 160
#define CPU_TARGET_MASK 0xF

#define SMC_CMD_CPU1BOOT	(-4)

extern volatile int pen_release;

void gicv2_init(void);
int do_irq(void);

int gicv2_cpu0_set_pending(int irq_num);

void gicv2_wakeup_secondary(unsigned int cpu,unsigned int sgi_id);
int gicv2_cpux_enable_softirq(void);
void  scu_enable();
unsigned int exynos_get_core_count(void);
unsigned int scu_get_core_count(void);
int exynos_power_up_cpu(unsigned int cpu);
void platform_smp_prepare_cpus(unsigned int max_cpus);
void smp_init_cpus(void);
int boot_secondary(unsigned int cpu);
/*spin lock for cpu0 and cpu3 which both use uart3 to print out text*/ 
unsigned int  uart3_lock(void);
void uart3_unlock(void);

extern void __delay(void);
extern void exynos_secondary_startup(void);

#define read_cpuid(reg)							\
	({								\
		unsigned int __val;					\
		asm("mrc	p15, 0, %0, c0, c0, " __stringify(reg)	\
		    : "=r" (__val)					\
		    :							\
		    : "cc");						\
		__val;							\
	})


static inline unsigned int  exynos_smc(unsigned int  cmd, unsigned int arg1, unsigned int arg2, unsigned int arg3)
{
	register u32 reg0 __asm__("r0") = cmd;
	register u32 reg1 __asm__("r1") = arg1;
	register u32 reg2 __asm__("r2") = arg2;
	register u32 reg3 __asm__("r3") = arg3;

	__asm__ volatile (
                ".arch_extension sec\n" 
		"smc	0\n"
		: "+r"(reg0), "+r"(reg1), "+r"(reg2), "+r"(reg3)
	);

	return reg0;
}
/*use ldrex and strex to realize atomic oprations*/
static inline unsigned int __raw_spin_lock(unsigned int* lock)
{
    unsigned int tmp;

    __asm__ __volatile__(
     "1:ldrex    %0, [%1]\n"
     "teq    %0, #0\n"
     "strexeq    %0, %2, [%1]\n"
     "teqeq    %0, #0\n"
     "bne    1b\n"
     "dmb\n"
     : "=&r" (tmp)
     : "r" (lock), "r" (locked)
     : "cc");
    return tmp;
}
static inline void __raw_spin_unlock(unsigned int* lock){
     __asm__ __volatile__(
     "dmb\n"
     "str %1,[%0]\n"
     :
     :"r" (lock),"r" (unlocked)
     :"cc");
}

struct _cpu_boot_info {
	void *power_base;
	void *boot_base;
};


#endif
