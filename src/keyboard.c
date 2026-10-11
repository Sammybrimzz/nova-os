#include <stdint.h>

#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

#define STATUS_OUTPUT_FULL   0x01
#define STATUS_INPUT_FULL    0x02

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

int keyboard_has_data(void)
{
    return (inb(KEYBOARD_STATUS_PORT) & STATUS_OUTPUT_FULL) != 0;
}

uint8_t keyboard_read_scancode(void)
{
    /*
     * Wait until the keyboard controller
     * has data available.
     */
    while (!keyboard_has_data())
    {
    }

    return inb(KEYBOARD_DATA_PORT);
}