bits 32

section .text

global context_restore_interrupt

context_restore_interrupt:

    mov eax, [esp + 4]

    mov esp, eax

    popa

    iretd
