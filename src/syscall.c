#include <stdint.h>

#include "process.h"

extern void screen_print(const char *text);
extern void screen_put_char(char c);

extern process_t *scheduler_get_current_process(void);
extern process_t *scheduler_select_next(void);

#define SYS_WRITE  1
#define SYS_GETPID 2
#define SYS_YIELD  3

static void print_number(uint32_t number)
{
    char buffer[12];
    int i = 0;

    if (number == 0)
    {
        screen_put_char('0');
        return;
    }

    while (number > 0)
    {
        buffer[i] = '0' + (number % 10);
        number /= 10;
        i++;
    }

    while (i > 0)
    {
        i--;
        screen_put_char(buffer[i]);
    }
}

uint32_t syscall_handler(uint32_t *context)
{
    /*
     * PUSHA creates this layout:
     *
     * context[0] = EDI
     * context[1] = ESI
     * context[2] = EBP
     * context[3] = original ESP
     * context[4] = EBX
     * context[5] = EDX
     * context[6] = ECX
     * context[7] = EAX
     */

    uint32_t syscall_number = context[7];
    uint32_t argument1 = context[4];

    /*
     * SYS_WRITE
     */
    if (syscall_number == SYS_WRITE)
    {
        const char *text =
            (const char *)argument1;

        if (text != 0)
        {
            screen_print(text);
        }

        return 0;
    }

    /*
     * SYS_GETPID
     */
    if (syscall_number == SYS_GETPID)
    {
        process_t *current =
            scheduler_get_current_process();

        if (current == 0)
        {
            return 0;
        }

        return current->pid;
    }

    /*
     * SYS_YIELD
     *
     * Save the current process context,
     * select the next process, and return
     * the next process's context address.
     */
    if (syscall_number == SYS_YIELD)
    {
        process_t *current =
            scheduler_get_current_process();

        if (current != 0)
        {
            current->context =
                (interrupt_context_t *)context;

            current->state =
                PROCESS_READY;
        }

        process_t *next =
            scheduler_select_next();

        if (next == 0)
        {
            return 0;
        }

        next->state =
            PROCESS_RUNNING;

        return (uint32_t)next->context;
    }

    /*
     * Unknown syscall.
     */
    return 0xFFFFFFFF;
}
