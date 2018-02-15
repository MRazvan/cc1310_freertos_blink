# CC1310 FreeRTOS Blink
Basic project demonstrating the usage of FreeRTOS on the Simplelink CC1310 mcu without TI SDK's. It uses the GCC compiler to build.
There is no need to install any TI SDK's, except what is needed to run CCS and debug with XDS110. 
As a note is also includes only what is needed, if more functionality is needed then CC1310 driverlib must be aquired (usually from TI SDK's)

There are two way's to build the project depending on DRIVER_LIB define
If the DRIVER_LIB is undefined, the only files needed to get blinking on CC1310 are:
* hw_chip_def.h
* hw_gpio.h
* hw_ioc.h
* hw_memmap.h
* hw_nvic.h
* hw_prcm.h
* hw_types.h

The default Cortex M3 integration for FreeRTOS does not work because CC1310 has built in functionality in ROM, after it starts, the NVIC is pointing to a location in ROM, meaning even if we add our Interrupt Handlers for SVC and SysTick they will not execute. We have to move the NVIC Vector table to the correct address at the begining of FLASH or in RAM if required. (Check the startup_cc13xx_gcc.c file for NVIC_VTABLE)

This project is distributed under "FreeRTOS Open Source License". Full license text is available on the following link: http://www.freertos.org/a00114.html

The FreeRTOS version used is 10.0.1