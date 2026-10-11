bits 32

section .text

global irq1_stub
extern irq1_handler

irq1_stub:
    pusha

    call irq1_handler

    popa

    mov al, 0x20
    out 0x20, al

    iretd
