CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -Wall -Wextra
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

BUILD = build

ASM_SOURCES = boot gdt interrupts timer context syscall user_mode
C_SOURCES = kernel gdt interrupts pic keyboard screen timer memory heap process scheduler shell test_process tss syscall

ASM_OBJECTS = $(addprefix $(BUILD)/,$(addsuffix .o,$(ASM_SOURCES)))
C_OBJECTS = $(addprefix $(BUILD)/,$(addsuffix _c.o,$(filter gdt interrupts syscall,$(C_SOURCES))))
C_OBJECTS += $(addprefix $(BUILD)/,$(addsuffix .o,$(filter-out gdt interrupts syscall,$(C_SOURCES))))

OBJECTS = $(BUILD)/boot.o \
          $(BUILD)/kernel.o \
          $(BUILD)/gdt.o \
          $(BUILD)/gdt_c.o \
          $(BUILD)/tss.o \
          $(BUILD)/keyboard.o \
          $(BUILD)/screen.o \
          $(BUILD)/shell.o \
          $(BUILD)/pic.o \
          $(BUILD)/interrupts.o \
          $(BUILD)/interrupts_c.o \
          $(BUILD)/timer_asm.o \
          $(BUILD)/timer_c.o \
          $(BUILD)/memory.o \
          $(BUILD)/heap.o \
          $(BUILD)/process.o \
          $(BUILD)/scheduler.o \
          $(BUILD)/context.o \
          $(BUILD)/test_process.o \
          $(BUILD)/syscall.o \
          $(BUILD)/syscall_c.o \
          $(BUILD)/user_mode.o

.PHONY: all iso run clean check

all: $(BUILD)/kernel.bin

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: src/boot.asm | $(BUILD)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD)/gdt.o: src/gdt.asm | $(BUILD)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD)/interrupts.o: src/interrupts.asm | $(BUILD)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD)/timer_asm.o: src/timer.asm | $(BUILD)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD)/context.o: src/context.asm | $(BUILD)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD)/syscall.o: src/syscall.asm | $(BUILD)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD)/user_mode.o: src/user_mode.asm | $(BUILD)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD)/gdt_c.o: src/gdt.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/interrupts_c.o: src/interrupts.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/syscall_c.o: src/syscall.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel.bin: $(OBJECTS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

check: $(BUILD)/kernel.bin
	grub-file --is-x86-multiboot $(BUILD)/kernel.bin

iso: check
	cp $(BUILD)/kernel.bin iso/boot/kernel.bin
	grub-mkrescue -o nova.iso iso

run: iso
	qemu-system-i386 -M pc -cdrom nova.iso

clean:
	rm -f $(BUILD)/*.o $(BUILD)/kernel.bin nova.iso
