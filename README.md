# BareMetalMPCore
exynos 4412(cortex-a9 4cpus) multi-core bare metal demo code.
the code start all cores in soc and give every cpu a task to run.
how to:
1 compile and link with cross compile tool set; 
2 power on the development board and enter uboot command line environment,in which we can upload the mpcore.bin to it's dram; 
3 enter "dnw 40008000" in uboot command line; 
4 use "dnw2 mpcore.bin" upload the bin file to board's dram; 
5 use "go 40008000" in uboot command line to run the code;  
6 we can see leds blinking and uart3 outputing messages from cpu0 and cpu3 continuously; 
