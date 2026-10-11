#include "process.h"

extern void gdt_init(void);
extern void idt_init(void);
extern void pic_remap(void);

extern void timer_init(unsigned int frequency);

extern void memory_init(void);
extern void heap_init(void);

extern void process_init(void);
extern void scheduler_init(void);

extern process_t *process_create(
    unsigned int entry_point
);

extern void scheduler_add_process(
    void *process
);

extern interrupt_context_t *
process_get_context(
    process_t *process
);

extern void context_restore_interrupt(
    unsigned int context
);

extern void nova_process_test(void);
extern void nova_process_test_2(void);

extern void screen_print(const char *text);

void kernel_main(void)
{
    gdt_init();
    pic_remap();
    idt_init();
    timer_init(100);

    memory_init();
    heap_init();

    process_init();
    scheduler_init();

    /*
     * Create Process 1.
     */
    process_t *process1 =
        process_create(
            (unsigned int)nova_process_test
        );

    if (process1 == 0)
    {
        screen_print(
            "ERROR: Process 1 creation failed\n"
        );

        while (1)
        {
        }
    }

    scheduler_add_process(process1);

    /*
     * Create Process 2.
     */
    process_t *process2 =
        process_create(
            (unsigned int)nova_process_test_2
        );

    if (process2 == 0)
    {
        screen_print(
            "ERROR: Process 2 creation failed\n"
        );

        while (1)
        {
        }
    }

    scheduler_add_process(process2);

    /*
     * Start Process 1.
     */
    interrupt_context_t *context =
        process_get_context(process1);

    process1->state =
        PROCESS_RUNNING;

    __asm__ volatile ("cli");

    context_restore_interrupt(
        (unsigned int)context
    );

    /*
     * We should never return here.
     */
    while (1)
    {
    }
}
