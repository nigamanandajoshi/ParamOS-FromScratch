# ParamOS — A Minimal 32-bit Operating System

ParamOS is a compact 32-bit x86 operating system built from the ground up.  
It implements the early boot pipeline — BIOS → Real Mode → Protected Mode → C Kernel — with a focus on clarity, correctness, and low-level system understanding.

---

## Overview

ParamOS explores fundamental questions about how computers start executing code long before an operating system appears.  
Every part of the system — from enabling the A20 line to setting up the GDT and booting a freestanding C kernel — is implemented manually and kept intentionally minimal.

The project grows incrementally, with each subsystem added as an isolated, well-documented component.

---

## Current Capabilities

### Boot Pipeline
- Custom BIOS bootloader (Real Mode)
- Stack setup and disk read routines  
- A20 gate activation  
- Global Descriptor Table (GDT)  
- Switch to 32-bit protected mode  
- Far jump into C kernel entry  

### Kernel
- Freestanding ELF-linked C kernel
- VGA text-mode console driver  
- Hardware port I/O (inb, outb, inw, outw)
- Clear and modular code structure

### General
- Cross-compiler toolchain support
- Built to run cleanly in QEMU
- Legacy ASM-only version included for reference

Upcoming additions extend the kernel into interrupt handling, device interaction, and basic system services.

---

## High-Level Boot Flow

```
BIOS → Boot Sector (0x7C00)
        ↓
      Bootloader (ASM)
        • Enable A20
        • Load GDT
        • Switch to Protected Mode
        ↓
   Kernel Entry (ASM)
        ↓
   Freestanding C Kernel
```

This path is deliberately minimal and transparent, making each step easy to trace and understand.

---

## Development Roadmap

### Completed
- Bootloader  
- Protected mode transition  
- GDT  
- C kernel entry  
- VGA console  
- I/O port layer  

### In Progress
- IDT (Interrupt Descriptor Table)  
- ISR/IRQ stubs  
- PIC remapping  
- PIT timer interrupts  
- Keyboard driver  
- Kernel feature menu  

### Future Plans
- Shell  
- Kernel memory allocator  
- Paging  
- Task switching  
- Basic multitasking

---

## Repository Structure

```
ParamOS/
│
├── src/                    # Protected-mode kernel source
│   ├── boot.asm
│   ├── kernel_entry.asm
│   ├── kernel.c
│   ├── console.c/.h
│   ├── ports.c/.h
│   ├── linker.ld
│   └── stdint.h
│
├── legacy/                 # Initial ASM-only version
│
├── build/                  # Generated binaries
│
└── Makefile
```

The structure is kept intentionally simple, evolving naturally with the kernel.

---

## Building and Running

### Requirements (macOS example)

```
brew install x86_64-elf-gcc nasm qemu
```

### Build

```
make
```

### Run

```
make run
```

Expected output on boot:

```
Welcome to Param OS C Kernel!
Running in 32-bit Protected Mode.
Powered by Nigam <3
```

---

## Notes on Design

ParamOS prioritizes:

- **Simplicity** — avoiding unnecessary abstractions  
- **Transparency** — code that reflects architectural intent  
- **Incremental expansion** — each feature a clean, independent module  

This makes the project approachable as a learning tool while remaining grounded in real x86 system behavior.

---

## License

ParamOS is licensed under the MIT License.

