<!-- # ParamOS — A Minimal 32-bit Operating System

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
 -->

# ParamOS — A Minimal 32-bit Operating System

ParamOS is a compact 32-bit x86 operating system built from scratch.  
It implements the complete early boot pipeline — BIOS → Real Mode → Protected Mode → C Kernel — with a focus on architectural clarity and low-level system exploration.

The project is structured to grow subsystem by subsystem, with each component designed to be small, readable, and educational.

---

## Overview

Most software runs far above the hardware, abstracted from how a computer actually starts.  
ParamOS begins at the opposite end of the stack: at reset, in real mode, with nothing but 512 bytes of boot sector code and complete responsibility for what happens next.

This repository builds that journey step-by-step:

**Bootloader → A20 → GDT → Protected Mode → Kernel Entry → Console → Hardware I/O**

Each milestone reveals another piece of how computers transition from firmware to a running operating system.

---

## Booting a Machine: The 16-Bit World

When a PC powers on, the CPU starts in **Real Mode** and the BIOS loads the first 512 bytes of a disk — the *boot sector* — into memory at `0x7C00`.

Inside this tiny environment:

- No memory protection  
- No standard library  
- No built-in debugging  
- No C runtime  
- Only BIOS interrupts and raw x86 instructions  

ParamOS begins here.

### Responsibilities of the bootloader
- Initialize a stack  
- Enable the A20 line (access beyond 1MB)  
- Prepare a Global Descriptor Table (GDT)  
- Load the kernel from disk  
- Switch the CPU into **32-bit protected mode**  
- Jump to the kernel’s entry point  

The entire loader fits in one file and is intentionally transparent, showing exactly what an OS must do to take control of the machine.

---

## Constructing the GDT

The Global Descriptor Table defines how the CPU views memory once in protected mode.

ParamOS uses a minimal configuration:
- Null descriptor  
- 32-bit code segment  
- 32-bit data segment  

This creates a **flat memory model**, mapping the full 4GB address space equally for all segments — the same model used by most modern kernels.

Loading the GDT and flipping the protected-mode bit in CR0 marks one of the major transitions in the boot process.

---

## Transitioning to Protected Mode

Entering protected mode changes nearly everything:

- Memory segmentation rules  
- Instruction set  
- Privilege levels  
- Exception handling  
- Interrupt mechanics  
- Address sizes  

After enabling protected mode, the bootloader performs a far jump into the new code segment, sets up flat segments, a stack, and passes control to the kernel.

ParamOS executes this transition cleanly and in full transparency through a short assembly routine.

---

## Building a Freestanding C Kernel

Once in protected mode, the kernel begins execution from `kernel_entry.asm`.  
This file sets up registers and calls `kernel_main()` — written in **freestanding C**, meaning:

- No standard library  
- No heap  
- No libc headers  
- No runtime initialization  

The compiler is invoked with `-ffreestanding` and `-nostdlib`, ensuring full control over the environment.

This is the base for implementing actual kernel subsystems.

---

## Text-Mode VGA Console

Early kernels rely on direct hardware access for visuals.  
ParamOS provides a simple VGA text console writing to `0xB8000`.

Supported:
- Character output  
- Newlines  
- Line wrapping  
- Clearing the screen  
- Text coloring  

This console becomes the primary debugging mechanism for all upcoming kernel features.

---

## Hardware I/O (Ports)

Hardware devices like the keyboard controller, PIC, PIT, CMOS, and more use **port-mapped I/O**.

ParamOS implements:

- `inb`, `outb`  
- `inw`, `outw`  

using inline assembly.  
These functions unlock the entire interrupt and device-driver ecosystem that follows.

This marks the transition from a static booting kernel to a hardware-aware one.

---

## Repository Structure

```
ParamOS/
│
├── src/
│   ├── boot.asm              # Bootloader (real mode)
│   ├── kernel_entry.asm      # Protected mode entry stub
│   ├── kernel.c              # Main kernel code
│   ├── console.c/h           # VGA text console
│   ├── ports.c/h             # I/O port utilities
│   ├── linker.ld             # Kernel linker script
│   └── stdint.h
│
├── legacy/                   # Original real-mode ASM-only version
├── build/                    # Output binaries
└── Makefile
```

The structure is kept intentionally straightforward as the system expands.

---

## Current Capabilities

### Bootloader
- Disk loading  
- A20 enable  
- GDT setup  
- Protected-mode switch  

### Kernel
- ELF linking  
- Freestanding C environment  
- VGA console  
- Hardware port I/O  

### General
- Cross-compiler build support  
- Fully QEMU bootable  
- Clean, modular codebase  

---

## Development Roadmap

### In Progress
- IDT (Interrupt Descriptor Table)  
- ISR/IRQ entry stubs  
- PIC remapping  
- PIT timer interrupts  
- Keyboard driver  
- Kernel feature menu  

### Planned
- Shell  
- Memory allocation  
- Paging  
- Task switching  
- Multitasking fundamentals  

---

## Building and Running

### Prerequisites (macOS)

```sh
brew install x86_64-elf-gcc nasm qemu
```

### Build the OS

```sh
make
```

### Run in QEMU

```sh
make run
```

Expected output:

```
Welcome to Param OS C Kernel!
Running in 32-bit Protected Mode.
Powered by Nigam <3
```

---

## Closing Thoughts

ParamOS is an ongoing exploration of how operating systems evolve from the moment a CPU starts executing instructions.  
By building the system from first principles, each milestone—booting, switching modes, printing text, talking to hardware—reveals a clear layer of system architecture.

Every subsystem is implemented with simplicity and predictability in mind, creating a foundation for experimenting with interrupts, device drivers, memory management, and task scheduling as the project grows.

---

## License

MIT License
