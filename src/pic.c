#include <stdint.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21

#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define ICW1_INIT    0x10
#define ICW1_ICW4    0x01

#define ICW4_8086    0x01

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static void io_wait(void)
{
    outb(0x80, 0);
}

void pic_remap(void)
{
    /*
     * Mask all hardware interrupts while
     * configuring the PIC.
     */
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);

    /*
     * Start initialization.
     */
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    /*
     * Master PIC:
     *
     * IRQ0-7 → interrupts 32-39
     */
    outb(PIC1_DATA, 0x20);
    io_wait();

    /*
     * Slave PIC:
     *
     * IRQ8-15 → interrupts 40-47
     */
    outb(PIC2_DATA, 0x28);
    io_wait();

    /*
     * Tell master that slave is connected
     * through IRQ2.
     */
    outb(PIC1_DATA, 4);
    io_wait();

    /*
     * Tell slave that its cascade identity
     * is IRQ2.
     */
    outb(PIC2_DATA, 2);
    io_wait();

    /*
     * Use 8086/88 mode.
     */
    outb(PIC1_DATA, ICW4_8086);
    io_wait();

    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    /*
     * Enable ONLY IRQ0.
     *
     * Bit 0 = IRQ0
     * 0 = enabled
     * 1 = masked
     *
     * Therefore:
     *
     * 11111110
     *       ↑
     *      IRQ0
     */
    outb(PIC1_DATA, 0xFE);

    /*
     * Keep every slave IRQ disabled.
     */
    outb(PIC2_DATA, 0xFF);
}
