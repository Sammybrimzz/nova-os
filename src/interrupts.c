#include <stdint.h>

#define IDT_ENTRIES 256

struct idt_entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idt_descriptor;

extern void timer_irq_stub(void);
extern void irq1_stub(void);
extern void syscall_entry(void);

static void idt_set_gate(
    int number,
    uint32_t handler,
    uint8_t type_attr
)
{
    idt[number].offset_low =
        handler & 0xFFFF;

    idt[number].selector = 0x08;
    idt[number].zero = 0;
    idt[number].type_attr = type_attr;

    idt[number].offset_high =
        (handler >> 16) & 0xFFFF;
}

void irq1_handler(void)
{
    /*
     * Keyboard interrupts are not being used yet.
     *
     * The shell currently polls the keyboard controller
     * directly.
     */
}

void idt_init(void)
{
    idt_descriptor.limit =
        sizeof(idt) - 1;

    idt_descriptor.base =
        (uint32_t)&idt;

    for (int i = 0;
         i < IDT_ENTRIES;
         i++)
    {
        idt_set_gate(
            i,
            0,
            0x8E
        );
    }

    /*
     * IRQ0 — PIT timer.
     */
    idt_set_gate(
        32,
        (uint32_t)timer_irq_stub,
        0x8E
    );

    /*
     * IRQ1 — keyboard.
     */
    idt_set_gate(
        33,
        (uint32_t)irq1_stub,
        0x8E
    );

    /*
     * 0x80 — NOVA system calls.
     *
     * 0xEE = present + ring 3 callable
     *       + 32-bit interrupt gate.
     */
    idt_set_gate(
        0x80,
        (uint32_t)syscall_entry,
        0xEE
    );

    __asm__ volatile (
        "lidt %0"
        :
        : "m"(idt_descriptor)
    );
}
