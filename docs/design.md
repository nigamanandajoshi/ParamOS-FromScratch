# ParamOS — Technical Design Document

This document describes the internal architecture, design principles, and implementation details of ParamOS, a minimal 32-bit operating system built from first principles.  
The goal is to provide a clear, structured overview of every component in the system and the rationale behind its design.

---

## 1. Architectural Goals

ParamOS is designed around four principles:

### 1.1 Simplicity  
Subsystems are intentionally minimal, avoiding unnecessary abstractions.  
This ensures each layer reflects real x86 behavior.

### 1.2 Transparency  
All critical boot and kernel mechanics are implemented explicitly:
- CPU mode switching  
- Memory descriptor tables  
- Interrupt mechanics  
- Direct hardware access  

### 1.3 Modularity  
Each subsystem is isolated and can evolve independently:
- Bootloader  
- GDT  
- Kernel entry  
- Console  
- Port I/O  
- IDT / PIC  
- Memory management  
- Shell  

### 1.4 Stable Growth  
The OS grows incrementally, with each new subsystem building cleanly upon the last.

---

## 2. Boot Process Architecture

The system follows the standard PC boot chain:

```
BIOS → Bootloader → Protected Mode Entry → C Kernel → Subsystems
```

### 2.1 BIOS Stage
The BIOS:
- initializes basic hardware  
- selects a boot device  
- loads the first 512 bytes (boot sector) to `0x7C00`  
- jumps into it  

### 2.2 Bootloader (Real Mode, 16-bit)
The bootloader:
- sets up a temporary stack  
- enables the A20 line  
- loads the kernel from disk  
- installs the GDT  
- switches to protected mode  
- performs a far jump to the 32-bit entry point  

It performs no dynamic memory allocation and uses BIOS interrupts for disk access.

### 2.3 GDT (Global Descriptor Table)
ParamOS uses a **flat memory model**:
- Code segment: `0x00000000` → `0xFFFFFFFF`
- Data segment: same range  

This is simple, predictable, and aligns with modern OS practice.

---

## 3. Protected Mode Entry

Once protected mode is enabled:
- segment registers are reloaded  
- a new stack is set  
- paging is **not** enabled yet  
- interrupts are disabled until IDT is built  

`kernel_entry.asm` then calls the C kernel.

---

## 4. Freestanding Kernel Design

The kernel is compiled with:

```
-ffreestanding -nostdlib -m32
```

This ensures:
- no libc  
- no runtime  
- no automatic initialization  

### 4.1 Responsibilities of `kernel_main()`
- initialize the console  
- provide startup messages  
- prepare for later subsystem initialization  

---

## 5. VGA Console Driver

The console writes directly to video memory at `0xB8000`.

Features:
- character output  
- line wrapping  
- newline handling  
- clear screen  
- color attributes  

This forms the basis for logging and debugging.

---

## 6. Port I/O Layer

Hardware devices on x86 communicate via **I/O ports**.  
ParamOS provides:

- `inb`, `outb` (8-bit)
- `inw`, `outw` (16-bit)

These functions are implemented with GCC inline assembly.

The port layer is required for:
- PIC  
- PIT  
- Keyboard controller  
- CMOS  
- other low-level devices  

---

## 7. Interrupt and Hardware Subsystems (Planned)

### 7.1 IDT (Interrupt Descriptor Table)
Maps interrupt vectors (0–255) to handlers.

### 7.2 ISR/IRQ Stubs
64-bit compatible assembly stubs to capture CPU state and pass control to C handlers.

### 7.3 PIC Remapping
Avoids overlap between hardware IRQs and CPU exceptions.

### 7.4 Timer (PIT) Driver
Provides:
- periodic interrupts  
- kernel timing  
- foundation for task switching  

### 7.5 Keyboard Driver
Reads scancodes from port `0x60` and translates them to ASCII.

### 7.6 Kernel Menu
Interactive mode for exploring implemented subsystems.

---

## 8. Future Kernel Architecture

### 8.1 Memory Management
- Simple bump allocator  
- Page frame allocator  
- Paging with identity map  
- Kernel heap  

### 8.2 Process Subsystem
- Task switching  
- Kernel stacks  
- Scheduling primitives  

### 8.3 Shell
A simple command interpreter running in text mode.

---

## 9. Repository Structure

```
ParamOS/
│
├── src/
│   ├── boot.asm
│   ├── kernel_entry.asm
│   ├── kernel.c
│   ├── console.c / .h
│   ├── ports.c / .h
│   ├── linker.ld
│   └── stdint.h
│
├── legacy/
├── build/
└── Makefile
```

---

## 10. Summary

ParamOS is a step-by-step, transparent exploration of operating system fundamentals.  
It begins at the lowest software-visible layer of x86 hardware and builds upward into a structured kernel architecture.  
Each subsystem is intentionally small, well-scoped, and designed for clarity.

