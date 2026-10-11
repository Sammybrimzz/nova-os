#include <stdint.h>

#include "process.h"

extern void *kmalloc(uint32_t size);
extern void kfree(void *address);

extern void screen_print(const char *text);
extern void screen_put_char(char c);

static process_t *process_table[MAX_PROCESSES];

static uint32_t next_pid = 1;

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

void process_init(void)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        process_table[i] = 0;
    }

    next_pid = 1;
}

process_t *process_create(uint32_t entry_point)
{
    int slot = -1;

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i] == 0)
        {
            slot = i;
            break;
        }
    }

    if (slot == -1)
    {
        return 0;
    }

    process_t *process =
        (process_t *)kmalloc(sizeof(process_t));

    if (process == 0)
    {
        return 0;
    }

    void *stack =
        kmalloc(PROCESS_STACK_SIZE);

    if (stack == 0)
    {
        kfree(process);
        return 0;
    }

    void *kernel_stack = kmalloc(PROCESS_STACK_SIZE);

    if (kernel_stack == 0)
    {
        kfree(stack);
        kfree(process);
        return 0;
    }

    process->pid = next_pid++;
    process->state = PROCESS_READY;
    process->stack = (uint32_t)stack;
    process->kernel_stack = (uint32_t)kernel_stack;
    process->instruction_pointer = entry_point;
    process->next = 0;

    /*
     * Put the synthetic interrupt context
     * at the top of the process stack.
     */
    uint32_t context_address =
        (uint32_t)stack +
        PROCESS_STACK_SIZE -
        sizeof(interrupt_context_t);

    /*
     * Align the context to 16 bytes.
     */
    context_address &= ~0x0F;

    process->context =
        (interrupt_context_t *)context_address;

    /*
     * Initial general-purpose registers.
     */
    process->context->edi = 0;
    process->context->esi = 0;
    process->context->ebp = 0;
    process->context->original_esp = context_address;
    process->context->ebx = 0;
    process->context->edx = 0;
    process->context->ecx = 0;
    process->context->eax = 0;

    /*
     * Where the CPU will begin executing.
     */
    process->context->eip =
        entry_point;

    /*
     * Kernel code segment.
     */
    process->context->cs = 0x08;

    /*
     * Enable interrupts when the process starts.
     */
    process->context->eflags = 0x202;

    process_table[slot] = process;

    return process;
}

interrupt_context_t *
process_get_context(process_t *process)
{
    if (process == 0)
    {
        return 0;
    }

    return process->context;
}

void process_destroy(process_t *process)
{
    if (process == 0)
    {
        return;
    }

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i] == process)
        {
            if (process->stack != 0)
            {
                kfree((void *)process->stack);
            }

            if (process->kernel_stack != 0)
            {
                kfree((void *)process->kernel_stack);
            }

            kfree(process);

            process_table[i] = 0;

            return;
        }
    }
}

void process_list(void)
{
    screen_print("Processes:\n");

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        process_t *process =
            process_table[i];

        if (process == 0)
        {
            continue;
        }

        screen_print("PID: ");
        print_number(process->pid);

        screen_print("  State: ");

        if (process->state == PROCESS_READY)
        {
            screen_print("READY");
        }
        else if (process->state == PROCESS_RUNNING)
        {
            screen_print("RUNNING");
        }
        else if (process->state == PROCESS_BLOCKED)
        {
            screen_print("BLOCKED");
        }
        else if (process->state == PROCESS_TERMINATED)
        {
            screen_print("TERMINATED");
        }
        else
        {
            screen_print("UNKNOWN");
        }

        screen_print("\n");
    }
}
