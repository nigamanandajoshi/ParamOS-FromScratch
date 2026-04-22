# Contributing to ParamOS

ParamOS is a research-grade educational operating system built from scratch on 32-bit x86. Contributions that improve correctness, clarity, or extend functionality are welcome.

## Build requirements

```bash
# Ubuntu / Debian
sudo apt-get install nasm gcc binutils make xorriso grub-pc-bin qemu-system-x86

# macOS (via Homebrew)
brew install nasm x86_64-elf-gcc x86_64-elf-binutils xorriso qemu
```

## Build and run

```bash
# Clone
git clone https://github.com/nigamanandajoshi/ParamOS-FromScratch
cd ParamOS-FromScratch

# Build
make all

# Run in QEMU
make run
# or manually:
qemu-system-i386 -kernel kernel.bin -nographic -serial stdio
```

## Project structure

```
src/
  boot/        — Bootloader (NASM), GDT, IDT setup
  kernel/      — Kernel entry, interrupt handlers, kernel shell
  mm/          — Memory management (paging, physical allocator)
  task/        — Task creation and basic scheduling
docs/          — Architecture diagrams and boot screenshots
legacy/        — Earlier versions preserved for reference
```

## Areas open for contribution

| Area | Difficulty | Description |
|------|-----------|-------------|
| Virtual memory (paging) | Medium | Extend the current flat memory model with proper page tables |
| ELF loader | Medium | Load and execute ELF binaries from the kernel shell |
| Basic filesystem | Hard | Implement a simple in-memory filesystem (FAT-like) |
| Syscall interface | Medium | Add a minimal syscall layer between user/kernel space |
| VESA framebuffer | Easy-Medium | Switch from text-mode VGA to a basic graphics framebuffer |
| Keyboard driver | Easy | Improve scancode translation and key repeat handling |
| Documentation | Easy | Improve inline comments and add architecture diagrams |

## Contribution workflow

1. Fork this repository
2. Create a branch: `git checkout -b feature/your-feature-name`
3. Make changes. Keep commits small and well-described.
4. Test your changes with `make all && make run`
5. Open a Pull Request with a clear description of what you changed and why.

## Code style

- C code follows Linux kernel style: tabs for indentation, 80-char line limit
- All assembly files use NASM syntax
- Comment non-obvious decisions, not obvious ones

## Questions

Open an issue with the `question` label or reach out at nigamanandajoshi@gmail.com.
