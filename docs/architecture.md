# ParamOS Architecture

This document visualizes the high-level architecture of ParamOS, from the boot process to the kernel shell.

```mermaid
graph TD
    %% Global Graph Settings
    %% Use rounded nodes and explicit phases

    subgraph Hardware ["🔌 Hardware Initialization"]
        BIOS("BIOS / UEFI Hand-off")
        HW_Resources("CPU, RAM & I/O")
    end

    subgraph Boot ["🚀 Boot Phase"]
        Boot16("16-bit Bootloader")
        ProtMode("Protected Mode Switch (32-bit)")
    end

    subgraph Kernel ["⚙️ Kernel Space"]
        direction TB
        KEntry("Assembly Entry Stub")
        KMain("Kernel Main (C)")
        
        subgraph Subsystems ["Core Subsystems"]
            IDT("Interrupt Handling (IDT)")
            Memory("Memory Manager (PMM/Heap)")
            Sched("Task Scheduler")
        end
        
        subgraph DriverLayer ["Device Drivers"]
            Keyboard("Keyboard Driver")
            Display("VGA Console")
        end
    end

    subgraph User ["💻 User Interaction"]
        Shell("Command Shell")
    end

    %% Main Execution Flow
    BIOS ==> Boot16
    Boot16 ==> ProtMode
    ProtMode ==> KEntry
    KEntry ==> KMain
    KMain ==> Subsystems
    KMain ==> DriverLayer
    
    %% Operational Logic
    Sched -.-> Shell
    Keyboard -- Input --> Shell
    Shell -- Output --> Display

    %% Styling - Modern Pastel Palette with High Contrast Text
    classDef hardware fill:#f3f4f6,stroke:#4b5563,stroke-width:2px,color:#000;
    classDef boot fill:#fff7ed,stroke:#c2410c,stroke-width:2px,color:#000;
    classDef kernel fill:#eff6ff,stroke:#2563eb,stroke-width:2px,color:#000;
    classDef user fill:#ecfdf5,stroke:#059669,stroke-width:2px,color:#000;

    class BIOS,HW_Resources hardware;
    class Boot16,ProtMode boot;
    class KEntry,KMain,IDT,Memory,Sched,Keyboard,Display kernel;
    class Shell user;
```

## Description

1.  **Bootloader**: A minimal 16-bit bootloader (`boot.asm`) loads the kernel, enables the A20 line, and switches the CPU to 32-bit protected mode.
2.  **Kernel Entry**: An assembly stub (`kernel_entry.asm`) sets up the segment registers and stack before jumping to the C kernel.
3.  **Kernel Core**: `kernel.c` initializes the primary subsystems:
    *   **Interrupts**: Sets up the IDT and remaps the PIC to handle hardware interrupts (Timer, Keyboard).
    *   **Memory**: Initializes physical frame allocation and a simple kernel heap (`kmalloc`).
    *   **Drivers**: Configures the VGA console for output and the PS/2 keyboard for input.
4.  **Task Management**: Creates task structures and facilitates context switching. The scheduler currently uses cooperative multitasking or basic preemption.
5.  **Shell**: The primary interactive task runs in kernel mode (for now), accepting user input and executing commands.
