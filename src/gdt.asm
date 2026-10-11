
bits 32

section .text

global gdt_flush
global gdt_install_tss
global tss_flush

gdt_flush:
    lgdt [gdt_descriptor]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:.flush_done

.flush_done:
    ret


; void gdt_install_tss(uint32_t base, uint32_t limit)
; Arguments:
; [esp + 4] = TSS base address
; [esp + 8] = TSS limit

gdt_install_tss:
    push ebp
    mov ebp, esp
    push eax
    push ebx
    push ecx

    mov eax, [ebp + 8]      ; TSS base
    mov ebx, [ebp + 12]     ; TSS limit

    ; GDT entry 5 begins at gdt_start + 40.
    mov ecx, gdt_start + 40

    ; Limit, bits 0-15.
    mov word [ecx], bx

    ; Base, bits 0-15.
    mov word [ecx + 2], ax

    ; Base, bits 16-23.
    shr eax, 16
    mov byte [ecx + 4], al

    ; Present, DPL 0, 32-bit available TSS (type 0x9).
    mov byte [ecx + 5], 0x89

    ; Limit bits 16-19, with granularity and size flags clear.
    mov eax, ebx
    shr eax, 16
    and al, 0x0F
    mov byte [ecx + 6], al

    ; Base, bits 24-31.
    mov eax, [ebp + 8]
    shr eax, 24
    mov byte [ecx + 7], al

    pop ecx
    pop ebx
    pop eax
    pop ebp
    ret


; Load GDT, then load the Task Register with selector 0x28.
tss_flush:
    lgdt [gdt_descriptor]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:.tss_reload_done

.tss_reload_done:
    mov ax, 0x28
    ltr ax
    ret


section .data

align 8

gdt_start:

    ; Entry 0: Null descriptor
    dq 0x0000000000000000

    ; Entry 1: Kernel code, Ring 0 (selector 0x08)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00

    ; Entry 2: Kernel data, Ring 0 (selector 0x10)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00

    ; Entry 3: User code, Ring 3 (selector 0x1B)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xFA
    db 0xCF
    db 0x00

    ; Entry 4: User data, Ring 3 (selector 0x23)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xF2
    db 0xCF
    db 0x00

    ; Entry 5: TSS descriptor (selector 0x28).
    ; Filled in by gdt_install_tss().
    dq 0x0000000000000000

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start
