
bits 32

section .text

global enter_user_mode
global user_test_entry

enter_user_mode:
    cli

    ; Build the Ring 3 return frame.
    push dword 0x23
    push dword user_stack_top
    push dword 0x02
    push dword 0x1B
    push dword user_test_entry

    iretd

user_test_entry:
    ; Load Ring 3 data selectors.
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call SYS_WRITE (1).
    mov eax, 1
    mov ebx, first_message
    int 0x80

    ; SYS_WRITE should return 0 in EAX.
    cmp eax, 0
    jne .failure

    ; The first syscall returned correctly.
    mov eax, 1
    mov ebx, success_message
    int 0x80

.hang:
    jmp .hang

.failure:
    mov eax, 1
    mov ebx, failure_message
    int 0x80

    jmp .hang

section .rodata

first_message:
    db "NOVA: First Ring 3 syscall executed.", 10, 0

success_message:
    db "NOVA: Syscall return value verified!", 10, 0

failure_message:
    db "NOVA: Syscall return value FAILED!", 10, 0

section .bss

align 16
user_stack_bottom:
    resb 4096
user_stack_top:
