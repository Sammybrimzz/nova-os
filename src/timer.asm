bits 32

section .text

global timer_irq_stub

extern timer_handler
extern scheduler_tick

timer_irq_stub:
    pusha

    ; Save the current CPU context.
    ; ESP points to the PUSHA frame.
    push esp

    ; Let the C scheduler save the current process
    ; and choose the next process.
    call scheduler_tick

    add esp, 4

    ; EAX contains the context we should restore.
    mov esp, eax

    ; Update the timer tick counter.
    call timer_handler

    ; Send End Of Interrupt to the master PIC.
    mov al, 0x20
    out 0x20, al

    ; Restore registers from the selected process context.
    popa

    ; Restore EIP, CS and EFLAGS.
    iretd
