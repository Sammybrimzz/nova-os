bits 32

section .text

global syscall_entry
extern syscall_handler

%define SYS_YIELD 3

syscall_entry:
    pusha

    ; Pass the saved CPU context to C.
    push esp
    call syscall_handler
    add esp, 4

    ; Check which syscall was requested.
    ;
    ; PUSHA layout:
    ; [esp + 28] = saved EAX
    ;
    ; The syscall number was originally placed
    ; in EAX by the process.
    cmp dword [esp + 28], SYS_YIELD
    je .yield

    ; Normal syscall.
    ;
    ; The C return value is in EAX.
    ; Store it in the saved EAX slot so
    ; POPA restores the return value.
    mov [esp + 28], eax

    popa

    iretd

.yield:

    ; For SYS_YIELD, EAX contains the address
    ; of the next process's saved context.
    mov edx, eax

    ; If no next context was returned,
    ; continue with the current process.
    cmp edx, 0
    je .yield_current

    ; Switch to the next process's context.
    mov esp, edx

    popa

    iretd

.yield_current:

    ; No context switch.
    mov [esp + 28], eax

    popa

    iretd
