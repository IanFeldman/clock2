#include <stdint.h>
#include "LPC8xx.h"
#include "main.h"
#include "input.h"

// External symbols defined in linker script
extern uint32_t _estack;
extern uint32_t _sidata, _sdata, _edata;
extern uint32_t _sbss, _ebss;

// Function prototypes
void Reset_Handler(void);
void Default_Handler(void);
void SystemInit(void);
int main(void);

// Vector table
__attribute__((section(".isr_vector")))
const void *vector_table[] = {
    &_estack,               // Initial stack pointer
    Reset_Handler,          // Reset Handler
    Default_Handler,        // NMI
    Default_Handler,        // Hard Fault
    0, 0, 0, 0, 0, 0, 0,    // Reserved
    Default_Handler,        // SVC
    0, 0,                   // Reserved
    Default_Handler,        // PendSV
    Default_Handler,        // SysTick
    // IRQs (fill as needed)
    Default_Handler,        // SPI0
    Default_Handler,        // SPI1
    Default_Handler,        // Reserved
    UART0_IRQHandler,       // UART0
    Default_Handler,        // UART1
    Default_Handler,        // UART2
    Default_Handler,        // Reserved
    Default_Handler,        // Reserved
    Default_Handler,        // I2C0
    Default_Handler,        // SCT
    Default_Handler,        // MRT
    Default_Handler,        // CMP
    Default_Handler,        // WDT
    Default_Handler,        // BOD
    0,                      // Reserved
    Default_Handler,        // WKT
    0, 0, 0, 0, 0, 0, 0, 0, // Reserved
    PININT0_IRQHandler,     // PININT0
    PININT1_IRQHandler,     // PININT1
    PININT2_IRQHandler,     // PININT2
    PININT3_IRQHandler,     // PININT3
    PININT4_IRQHandler,     // PININT4
    Default_Handler,        // PININT5
    Default_Handler,        // PININT6
    Default_Handler         // PININT7
};

// Clock initialization
void SystemInit(void) {
    // Clock setup: internal RC oscillator (12 MHz) → PLL → 48 MHz
    // MSEL = 3 (x4), PSEL = 1 (÷2), PLL_OUT = 12 * 4 = 48 MHz
    LPC_SYSCON->SYSPLLCLKSEL = 0;               // Select IRC as PLL input
    LPC_SYSCON->SYSPLLCLKUEN = 0;               // Toggle update register
    LPC_SYSCON->SYSPLLCLKUEN = 1;

    LPC_SYSCON->SYSPLLCTRL = (3 << 0) | (1 << 5); // MSEL=3, PSEL=1
    LPC_SYSCON->PDRUNCFG &= ~(1 << 7);          // Power up PLL

    while (!(LPC_SYSCON->SYSPLLSTAT & 1));      // Wait for PLL lock

    LPC_SYSCON->MAINCLKSEL = 3;                 // Select PLL output
    LPC_SYSCON->MAINCLKUEN = 0;                 // Toggle update register
    LPC_SYSCON->MAINCLKUEN = 1;
}

// Reset interrupt routine
void Reset_Handler(void) {
    // Copy .data from Flash to RAM
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // Zero .bss section
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    SystemInit();  // Setup clock
    main();        // Enter user application

    while (1);     // Loop if main returns
}

// Default interrupt routine
void Default_Handler(void) {
    while (1);
}

