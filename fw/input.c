#include "LPC8xx.h"
#include "input.h"

/* global interrupt flags */
volatile int pps_flag = 0;
volatile int input1_flag = 0;
volatile int input2_flag = 0;
volatile int input3_flag = 0;
volatile int input4_flag = 0;

/* generic interrupt handler */
static void input_interrupt_handler(int irq);

/* Enable external interrupts for gpio pins:
 *  - Rising edge PININT0 for PPS (PIO0_2)
 *  - Falling edge PININT1 for INPUT1 (PIO0_1)
 *  - Falling edge PININT2 for INPUT2 (PIO0_15)
 *  - Falling edge PININT3 for INPUT3 (PIO0_12)
 *  - Falling edge PININT4 for INPUT4 (PIO0_5)
 */
void input_initialize(void)
{
    /* enable clock for gpio */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << GPIO_CLK_EN);
    /* disable ACMP_I2 function (bit 1) so PIO0_1 can be used as GPIO */
    /* disable SWCLK function (bit 2) so PIO0_3 can be used as GPIO */
    /* TODO: disable RESET function (bit 6) so PIO0_5 can be used as GPIO */
    LPC_SWM->PINENABLE0 |= (1 << 1) | (1 << 2);

    /* set debug pin as output */
    LPC_GPIO_PORT->DIR0 |= (1 << DEBUG_PIN);
    LPC_GPIO_PORT->CLR0 |= (1 << DEBUG_PIN);

    /* set pins as inputs */
    LPC_GPIO_PORT->DIR0 &= ~((1 << PPS_PIN) | (1 << INPUT1_PIN) |
        (1 << INPUT2_PIN) | (1 << INPUT3_PIN) | (1 << INPUT4_PIN));
    /* set external interrupts */
    LPC_SYSCON->PINTSEL[PPS_IRQ] = PPS_PIN;
    LPC_SYSCON->PINTSEL[INPUT1_IRQ] = INPUT1_PIN;
    LPC_SYSCON->PINTSEL[INPUT2_IRQ] = INPUT2_PIN;
    LPC_SYSCON->PINTSEL[INPUT3_IRQ] = INPUT3_PIN;
    LPC_SYSCON->PINTSEL[INPUT4_IRQ] = INPUT4_PIN;
    /* set edge sensitive */
    LPC_PIN_INT->ISEL &= ~((1 << PPS_IRQ) | (1 << INPUT1_IRQ));
    /* PPS detect rising edge */
    LPC_PIN_INT->IENR |= (1 << PPS_IRQ);
    /* MODE detect falling edge */
    LPC_PIN_INT->IENF |= (1 << INPUT1_IRQ);
    /* clear any pending/leftover flags */
    LPC_PIN_INT->IST = 0xFF;
    /* enable interrupts */
    NVIC_EnableIRQ(PININT0_IRQn);
    NVIC_EnableIRQ(PININT1_IRQn);
    /* TODO: enable remaining interrupts */
    // NVIC_EnableIRQ(PININT2_IRQn);
    // NVIC_EnableIRQ(PININT3_IRQn);
    // NVIC_EnableIRQ(PININT4_IRQn);
}


/* Debounce button input. Return 1 if pressed, 0 otherwise. */
int input_debounce(int input_pin, int *flag) 
{
    if (!(*flag))
    {
        return 0;
    }

    /* individual counter for each button */
    static int counter[4] = { 0, 0, 0, 0 };
    /* get index into counter */
    int idx = 0;
    switch(input_pin)
    {
        case INPUT1_PIN:
            idx = 0;
            break;
        case INPUT2_PIN:
            idx = 1;
            break;
        case INPUT3_PIN:
            idx = 2;
            break;
        case INPUT4_PIN:
            idx = 3;
            break;
        default:
            break;
    }

    counter[idx]++;
    /* timeout */
    if (counter[idx] > DEBOUNCE_COUNT)
    {
        /* check - active low */
        int press = !(LPC_GPIO_PORT->B0[input_pin]);
        counter[idx] = 0; 
        *flag = 0;
        return press;
    }
    return 0;
}


/* Toggle debug pin output level */
void input_toggle_debug(void)
{
    LPC_GPIO_PORT->NOT0 |= (1 << DEBUG_PIN);
}


/* Generic input pin interrupt handler */
void input_interrupt_handler(int irq)
{
    /* check interrupt flag and set mode flag */
    if (LPC_PIN_INT->IST & (1 << irq))
    {
        switch(irq)
        {
            case PPS_IRQ:
                pps_flag = 1;
                break;
            case INPUT1_IRQ:
                input1_flag = 1;
                break;
            case INPUT2_IRQ:
                input2_flag = 1;
                break;
            case INPUT3_IRQ:
                input3_flag = 1;
                break;
            case INPUT4_IRQ:
                input4_flag = 1;
                break;
            default:
                break;
        }
        /* clear interrupt flag */
        LPC_PIN_INT->IST = (1 << irq);
    }
}


/* RTC PPS external interrrupt handler */
void PININT0_IRQHandler(void)
{
    input_interrupt_handler(PPS_IRQ);
}


/* INPUT1 pin external interrrupt handler */
void PININT1_IRQHandler(void)
{
    input_interrupt_handler(INPUT1_IRQ);
}


/* INPUT2 pin external interrrupt handler */
void PININT2_IRQHandler(void)
{
    input_interrupt_handler(INPUT2_IRQ);
}


/* INPUT3 pin external interrrupt handler */
void PININT3_IRQHandler(void)
{
    input_interrupt_handler(INPUT3_IRQ);
}


/* INPUT4 pin external interrrupt handler */
void PININT4_IRQHandler(void)
{
    input_interrupt_handler(INPUT4_IRQ);
}

