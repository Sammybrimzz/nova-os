# NOVA OS

**An experimental AI-native operating system built from scratch.**

NOVA OS is a personal systems-programming project exploring how operating systems work, from booting a kernel to hardware interrupts, memory management, process scheduling, and system calls.

The long-term goal is to explore how an AI assistant could interact directly with operating-system services. For now, the focus is on building a working foundation and learning the underlying concepts step by step.

## Current Features

- Bootable 32-bit x86 kernel using Multiboot and GRUB
- VGA text output and keyboard input
- Basic command-line shell
- Global Descriptor Table (GDT) and Interrupt Descriptor Table (IDT)
- Programmable Interrupt Controller (PIC) and timer interrupts
- Physical memory frame allocator and kernel heap
- Basic processes and round-robin scheduling
- Initial Ring 3 user-mode test
- Software system calls using `int 0x80`
- Task State Segment (TSS) setup for user-to-kernel transitions

## Technology Stack

- C
- x86 assembly (NASM)
- GCC and GNU binutils
- GRUB and Multiboot
- QEMU
- WSL 2 with Ubuntu
- Make

## Building

Install the required development tools on Ubuntu or WSL:

```bash
sudo apt update
sudo apt install build-essential nasm qemu-system-x86 grub-pc-bin xorriso
```

Build the project:

```bash
make
```

Check the kernel:

```bash
make check
```

The build produces `build/kernel.bin` and the bootable `nova.iso` image.

To launch QEMU with a terminal-based display:

```bash
qemu-system-i386 -M pc -cdrom nova.iso -display curses
```

Exit QEMU using `Ctrl+C` in the terminal if needed.

## Project Status

NOVA OS is an early educational prototype. Some components are experimental and still require further integration and testing.

In particular, paging and complete process memory isolation are not yet implemented. User-mode execution and system calls are initial milestones, not a claim of production-ready security.

## Roadmap

- [x] Boot a custom kernel
- [x] Implement basic screen output and keyboard input
- [x] Add CPU descriptor tables and hardware interrupts
- [x] Introduce basic memory management
- [x] Begin process scheduling and system calls
- [x] Test initial Ring 3 execution
- [ ] Implement paging and virtual memory
- [ ] Improve process isolation and context switching
- [ ] Build a more capable shell and filesystem
- [ ] Explore AI integration with OS services

## Why NOVA?

This project is a hands-on journey into systems programming, computer architecture, and operating-system design. The goal is to understand the system from the ground up rather than simply assemble existing components.

NOVA OS is a learning project, and its design will evolve as development continues.
