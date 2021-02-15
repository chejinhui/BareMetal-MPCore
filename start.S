	.arch armv7-a
	.arm


	.text
	.align 2

	.globl	_start
        .globl get_cpsr
        .global __delay

_start:
	B	__reset
	LDR	PC, __undefined_instruction
	LDR	PC, __swi_handler
	LDR	PC, __prefetch_abort
	LDR	PC, __data_abort
	LDR	PC, __reserved_handler
	LDR	PC, __irq_handler
	LDR	PC, __fiq_handler

	.align 5
__reset:			B	real_reset
__undefined_instruction: 	.word __undefined_instruction
__swi_handler: 			.word __swi_handler
__prefetch_abort: 		.word __prefetch_abort
__data_abort: 			.word data_abort
__reserved_handler:		.word __reserved_handler
__irq_handler:			.word irq_handler
__fiq_handler:			.word __fiq_handler

	.align 5
real_reset:
	ldr		R0, =0x40008000
	mcr		p15,0,R0,c12,c0,0

	/* Switch Into SVC Mode */
	MRS		R0, CPSR
	BIC		R0, R0, #0x1F
	ORR		R0, R0, #0x0D3 @@ Disable IRQ and Disable FIQ
	MSR		CPSR_c, R0
	/* svc Mode Stack Initialization */
	LDR		SP, =svc_stack_top


	/* Switch Into IRQ Mode */
	MSR		CPSR_c, #0x0D2 @@ 0x110_10010: 0x1101_0010
	/* irq Mode Stack Initialization */
	LDR		SP, =irq_stack_top

	/* Switch Into USR Mode and Disable IRQ */
	//MSR		CPSR_c, #0x50 @@ 0b0(I)1(F)0(T)_10000
	//LDR		SP, =usr_stack_top
       
	MRS		R0, CPSR
	BIC		R0, R0, #0x1F
	ORR		R0, R0, #0x0D3 @@ Disable IRQ and Disable FIQ
	MSR		CPSR_c, R0

        mov	r0, #0			@ set up for MCR
	mcr	p15, 0, r0, c8, c7, 0	@ invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	@ clear bits 13 (--V-)
	bic	r0, r0, #0x00000003	@ clear bits 2:0 (-CAM)
	orr	r0, r0, #0x00001000	@ set bit 12 (---I) Icache
	orr	r0, r0, #0x00000002	@ set bit 1  (--A-) Align
	orr	r0, r0, #0x00000800	@ set bit 11 (Z---) BTB
	mcr	p15, 0, r0, c1, c0, 0

clear_bss:
       ldr    r0, _bss_start        
       ldr    r1, _bss_end      
       mov r2, #0x00000000  

clbss_l:  
       str    r2, [r0] 
       add    r0, r0, #4
       cmp    r0, r1
       ble    clbss_l

       BL		main
	
__die:
	B		__die

	.align 4
irq_handler:
	SUB		LR, LR, #0x4
	STMFD		SP!, {R0-R12, LR}
	BL		do_irq
	LDMFD		SP!, {R0-R12, PC}^
data_abort:
        // bl led_on
         b .

	/** void __delay(void)*/
__delay:
	PUSH	{R2, LR}

	LDR		R2, =0x100000
loop1:
	SUBS	R2, R2, #0x1
	BNE		loop1

	POP		{R2, LR}
	MOV		PC, LR
get_cpsr:
        MRS             R0, CPSR
        mov  pc,lr

.globl _bss_start
     _bss_start:
    .word __bss_start

.globl _bss_end
     _bss_end:
    .word __end
.data
	.space 8192
usr_stack_top:
	.space 1024
__reserved:
	.space 4096
svc_stack_top:
	.space 4096
irq_stack_top:

	.end
