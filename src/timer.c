#include <stdint.h>

#include "process.h"


#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40

#define PIT_BASE_FREQUENCY 1193182

#define TIMER_FREQUENCY 100


static volatile uint32_t timer_ticks = 0;


/*
 * Scheduler function implemented in scheduler.c.
 *
 * It receives the current CPU context and returns
 * the context address of the process that should run.
 */
extern uint32_t scheduler_tick(
    uint32_t current_context
);


/*
 * Write one byte to an I/O port.
 */
static inline void outb(
    uint16_t port,
    uint8_t value
)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}


/*
 * Timer handler.
 *
 * This is called by the timer interrupt assembly.
 */
void timer_handler(void)
{
    timer_ticks++;
}


/*
 * Return number of timer ticks.
 */
uint32_t timer_get_ticks(void)
{
    return timer_ticks;
}


/*
 * Configure the PIT.
 */
void timer_init(uint32_t frequency)
{
    uint32_t divisor =
        PIT_BASE_FREQUENCY / frequency;


    /*
     * Channel 0
     *
     * Mode 3
     *
     * Low byte followed by high byte.
     */
    outb(
        PIT_COMMAND_PORT,
        0x36
    );


    outb(
        PIT_CHANNEL0_PORT,
        divisor & 0xFF
    );


    outb(
        PIT_CHANNEL0_PORT,
        (divisor >> 8) & 0xFF
    );


    timer_ticks = 0;
}
