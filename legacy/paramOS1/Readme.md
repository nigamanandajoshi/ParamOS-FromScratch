# Param OS – Systems & OS Experiments

This repository contains low-level systems projects built to explore how operating systems and Linux internals work.

## Projects

1. **Bootloader (`bootloader/`)**
   - 16-bit x86 boot sector written in NASM assembly.
   - Prints a welcome message from Param OS using BIOS interrupts.
   - Boots in QEMU without touching your real disk.

2. **Linux Kernel Module (`kernel-module/`)**
   - Simple LKM that creates `/proc/param_osinfo`.
   - Shows kernel version, uptime (jiffies), and info about a selected PID.
   - Demonstrates basic kernel programming, procfs, and module parameters.

3. **Mini Shell (`minishell/`)**
   - A small shell implemented in C.
   - Supports built-in commands: `cd`, `exit`.
   - Runs external programs using `fork` + `execvp`.
   - Supports simple pipelines: `ls | wc -l`.
   - Supports background jobs using `&`.

## Quick Start

```bash
# run all builds
chmod +x test_all.sh
./test_all.sh
