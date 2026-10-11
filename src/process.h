#ifndef NOVA_PROCESS_H
#define NOVA_PROCESS_H

#include <stdint.h>

#define MAX_PROCESSES 16
#define PROCESS_STACK_SIZE 4096


/*
 * Process states.
 */
typedef enum
{
    PROCESS_UNUSED = 0,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED

} process_state_t;


/*
 * CPU state exactly matching the stack
 * expected by context_restore_interrupt().
 *
 * Stack layout:
 *
 *     EDI
 *     ESI
 *     EBP
 *     original ESP
 *     EBX
 *     EDX
 *     ECX
 *     EAX
 *     EIP
 *     CS
 *     EFLAGS
 */
typedef struct interrupt_context
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t original_esp;

    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;

} interrupt_context_t;


/*
 * Process Control Block.
 */
typedef struct process
{
    /*
     * Unique process ID.
     */
    uint32_t pid;

    /*
     * Current process state.
     */
    process_state_t state;

    /*
     * Beginning of allocated kernel stack.
     */
    uint32_t stack;

    /*
     * Process entry point.
     */
    uint32_t instruction_pointer;

    /*
     * Pointer to the saved CPU context
     * located on this process's stack.
     */
    interrupt_context_t *context;

    /*
     * Next process.
     */
    struct process *next;

} process_t;


void process_init(void);

process_t *process_create(uint32_t entry_point);

void process_destroy(process_t *process);

void process_list(void);

#endif
