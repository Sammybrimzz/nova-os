
#ifndef NOVA_CONTEXT_H
#define NOVA_CONTEXT_H

#include <stdint.h>

/*
 * CPU state pushed by the CPU when a
 * hardware interrupt occurs.
 *
 * On entry to an interrupt handler,
 * the CPU automatically pushes:
 *
 *     EFLAGS
 *     CS
 *     EIP
 *
 * Our assembly handler then uses PUSHA
 * to save the general-purpose registers.
 */
typedef struct interrupt_context
{
    /*
     * Registers saved by PUSHA.
     *
     * PUSHA pushes them in this hardware order:
     *
     * EAX
     * ECX
     * EDX
     * EBX
     * original ESP
     * EBP
     * ESI
     * EDI
     *
     * Because the stack grows downward,
     * memory is seen in the reverse order.
     */
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t original_esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    /*
     * CPU-created interrupt frame.
     */
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;

} interrupt_context_t;

#endif
