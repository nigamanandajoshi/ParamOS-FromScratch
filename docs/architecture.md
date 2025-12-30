# ParamOS Architecture

This document visualizes the high-level architecture of ParamOS, from the boot process to the kernel shell.

```mermaid
graph TD
    subgraph Hardware ["Hardware Layer"]
        BIOS["BIOS / UEFI"]
        CPU["x86 CPU"]
        RAM["Physical Memory"]
        IO["I/O Ports"]
        VGA["VGA Buffer"]
    end

    subgraph Boot ["Boot Sector (16-bit)"]
        Bootloader["boot.asm"]
        RealMode["Real Mode"]
        ProtMode["Protected Mode Switch"]
    end

    subgraph KernelEntry ["Kernel Entry (Assembly)"]
        EntryStub["kernel_entry.asm"]
    end

    subgraph KernelCore ["Kernel Core (C)"]
        KMain["kernel.c: kernel_main"]
        
        subgraph Subsystems
            IDT["IDT (Interrupts)"]
            PIC["PIC Remapping"]
            IRQ["IRQ Dispatcher"]
            Timer["PIT Timer"]
        end

        subgraph Memory ["Memory Management"]
            PMM["Frame Allocator (frames.c)"]
            KHeap["Kernel Heap (kmalloc.c)"]
        end

        subgraph Drivers
            Kbd["Keyboard Driver"]
            Console["VGA Console"]
            Ports["Port I/O"]
        end

        subgraph TaskManagement ["Task Management"]
            Scheduler["Scheduler"]
            ContextSwitch["Context Switch (switch.asm)"]
            TaskStruct["Task Structures"]
        end
    end

    subgraph UserSpace ["User Space (Simulated)"]
        Shell["Kernel Shell Task"]
    end

    %% Flow connections
    BIOS --> Bootloader
    Bootloader --> RealMode
    RealMode --> ProtMode
    ProtMode --> EntryStub
    EntryStub --> KMain

    %% Kernel Initialization Flow
    KMain --> Console
    KMain --> IDT
    KMain --> PIC
    KMain --> Timer
    KMain --> Kbd
    KMain --> PMM
    KMain --> KHeap
    KMain --> TaskManagement

    %% Subsystem Interactions
    IDT --> IRQ
    IRQ --> Kbd
    IRQ --> Timer
    Kbd --> Shell
    TaskManagement --> Shell
    Shell --> Console
    PMM --> KHeap
    
    %% Hardware Interactions
    Console -.-> VGA
    Kbd -.-> IO
    Timer -.-> IO
    PMM -.-> RAM

    classDef hardware fill:#f9f,stroke:#333,stroke-width:2px;
    classDef boot fill:#ff9,stroke:#333,stroke-width:2px;
    classDef kernel fill:#9cf,stroke:#333,stroke-width:2px;
    classDef user fill:#9f9,stroke:#333,stroke-width:2px;

    class BIOS,CPU,RAM,IO,VGA hardware;
    class Bootloader,RealMode,ProtMode boot;
    class EntryStub,KMain,IDT,PIC,IRQ,Timer,PMM,KHeap,Kbd,Console,Ports,Scheduler,ContextSwitch,TaskStruct kernel;
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
