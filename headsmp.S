/*
 *
 *  Cloned from linux/arch/arm/mach-realview/headsmp.S
 *
 *  Copyright (c) 2003 ARM Limited
 *  All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * exynos4 specific entry point for secondary CPUs.  This provides
 * a "holding pen" into which all secondary cores are held until we're
 * ready for them to initialise.
 */
.global exynos_secondary_startup
.global secondary_startup 
exynos_secondary_startup:

	mrc	p15, 0, r0, c0, c0, 5
	and	r0, r0, #15
	adr	r4, 1f
	ldmia	r4, {r5, r6}
	sub	r4, r4, r5
	add	r6, r6, r4
pen:	ldr	r7, [r6]
	cmp	r7, r0
	bne	pen

	/*
	 * we've been released from the holding pen: secondary_stack
	 * should now contain the SVC stack for these cores
     * edited by chejinhu at 2021.02.10
	 */
     cmp r7,#1
     beq cpu1_stack_init
     cmp r7,#2
     beq cpu2_stack_init
     cmp r7,#3
     beq cpu3_stack_init
     b .
cpu1_stack_init: 
     ldr sp,=cpu1_svc_stack_top
     b final
cpu2_stack_init:
     ldr sp,=cpu2_svc_stack_top
     b final
cpu3_stack_init:
     ldr sp,=cpu3_svc_stack_top

final:  b secondary_startup
     b .
   
	.align 2
1:	.long	.
	.long	pen_release
    .data
    .space 1024
    cpu1_svc_stack_top:
    .space 1024
    cpu2_svc_stack_top:
    .space 1024
    cpu3_svc_stack_top:

