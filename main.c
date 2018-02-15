/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */

#ifdef DRIVER_LIB
    #include <CC1310/driverlib/prcm.h>
    #include <CC1310/driverlib/ioc.h>
    #include <CC1310/driverlib/gpio.h>

    #define LED_GREEN IOID_7
    #define LED_RED   IOID_6
#else
    #include <CC1310/inc/hw_types.h>
    #include <CC1310/inc/hw_memmap.h>
    #include <CC1310/inc/hw_prcm.h>
    #include <CC1310/inc/hw_gpio.h>
    #include <CC1310/inc/hw_ioc.h>

    #define LED_GREEN   0x00000007
    #define LED_RED     0x00000006
#endif

#define WAIT_TRUE(x) do{;}while(!(x));

/*-----------------------------------------------------------*/
void letToggle(void* params){

    /*
     * If using the DriverLIB, files needed
     *  prcm, ioc, interrupt, gpio, debug, cpu (source and header),
     * theoretically we only use prcm ioc and gpio, the rest are dependency
     */
#ifdef DRIVER_LIB
    // Power on the PERIPHERAL Domain
    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
    WAIT_TRUE(PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH) == PRCM_DOMAIN_POWER_ON);

    // Enable the GPIO Peripheral
    PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);

    // Execute the power on and enable
    PRCMLoadSet();
    WAIT_TRUE(PRCMLoadGet());

    IOCPinTypeGpioOutput(LED_GREEN);
    IOCPinTypeGpioOutput(LED_RED);
    GPIO_writeDio(LED_GREEN, 0);
    GPIO_writeDio(LED_RED, 1);
#else
    // Power on the PERIPHERAL Domain
    HWREG(PRCM_BASE + PRCM_O_PDCTL0PERIPH) = 1;
    WAIT_TRUE(HWREG(PRCM_BASE + PRCM_O_PDSTAT0) & PRCM_PDSTAT0_PERIPH_ON);

    // Enable the GPIO Peripheral
    HWREG(PRCM_BASE + PRCM_O_GPIOCLKGR) |= 1;
    // Enable the update of all load related registers, and wait for the to finish loading
    HWREG(PRCM_NONBUF_BASE + PRCM_O_CLKLOADCTL) = PRCM_CLKLOADCTL_LOAD;
    WAIT_TRUE((HWREG(PRCM_BASE + PRCM_O_CLKLOADCTL) & PRCM_CLKLOADCTL_LOAD_DONE));

    // Set the LED pin to be an output pin
    // Setup pin and port -> This is the multiplexer and port configuration
    HWREG(IOC_BASE + ( LED_GREEN << 2 )) = IOC_IOCFG0_PORT_ID_GPIO | IOC_IOCFG0_PULL_CTL_DIS | IOC_IOCFG1_IOMODE_NORMAL; // GPIO port, default values - No Pull
    // Enable output mode in the GPIO module.
    HWREGBITW( GPIO_BASE + GPIO_O_DOE31_0, LED_GREEN ) = 1;
    HWREGBITW( GPIO_BASE + GPIO_O_DOE31_0, LED_RED ) = 1;

    HWREGB( GPIO_BASE + LED_GREEN ) = 0;
    HWREGB( GPIO_BASE + LED_RED ) = 1;

#endif

    // Blink
    while(1){
#ifdef DRIVER_LIB
        GPIO_toggleDio(LED_GREEN);
        GPIO_toggleDio(LED_RED);
#else
        HWREG( GPIO_BASE + GPIO_O_DOUTTGL31_0 ) = ( 1 << LED_GREEN ) | (1 << LED_RED );
#endif
        vTaskDelay(1000);
    }
}

int main(void)
{
    xTaskCreate(letToggle, "led", 16, NULL, 4, NULL );

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details.  */
    for( ;; );
}
