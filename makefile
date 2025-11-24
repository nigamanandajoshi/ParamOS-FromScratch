# ASM = nasm
# CC = x86_64-elf-gcc
# LD = x86_64-elf-ld
# OBJCOPY = x86_64-elf-objcopy
# QEMU = qemu-system-x86_64

# CFLAGS = -ffreestanding -m32 -O0 -nostdlib -nostdinc -fno-pic -fno-builtin -fno-stack-protector -fno-omit-frame-pointer

# LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

# all: paramos.bin

# boot.bin: boot.asm
# 	$(ASM) -f bin boot.asm -o boot.bin

# kernel_entry.o: kernel_entry.asm
# 	$(ASM) -f elf32 kernel_entry.asm -o kernel_entry.o

# kernel.o: kernel.c
# 	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

# kernel.elf: kernel_entry.o kernel.o
# 	$(LD) $(LDFLAGS) kernel_entry.o kernel.o -o kernel.elf

# kernel.bin: kernel.elf
# 	$(OBJCOPY) -O binary kernel.elf kernel.bin

# paramos.bin: boot.bin kernel.bin
# 	cat boot.bin kernel.bin > paramos.bin

# run:
# 	$(QEMU) -drive file=paramos.bin,format=raw,index=0,media=disk

# clean:
# 	rm -f *.o *.bin *.elf paramos.bin

# Top-level Makefile for ParamOS-FromScratch

SRCDIR   = src
BUILDDIR = build

ASM     = nasm
CC      = x86_64-elf-gcc
LD      = x86_64-elf-ld
OBJCOPY = x86_64-elf-objcopy
QEMU    = qemu-system-x86_64

CFLAGS  = -ffreestanding -m32 -O0 -nostdlib -nostdinc \
          -fno-pic -fno-builtin -fno-stack-protector -fno-omit-frame-pointer

LDFLAGS = -m elf_i386 -T $(SRCDIR)/linker.ld -nostdlib

OBJS = \
  $(BUILDDIR)/kernel_entry.o \
  $(BUILDDIR)/kernel.o \
  $(BUILDDIR)/console.o \
  $(BUILDDIR)/ports.o 
.PHONY: all run clean

all: $(BUILDDIR)/paramos.bin

# Ensure build directory exists
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# --- Bootloader (16-bit, raw bin) ---
$(BUILDDIR)/boot.bin: $(SRCDIR)/boot.asm | $(BUILDDIR)
	$(ASM) -f bin $< -o $@

# --- Kernel entry (32-bit ASM) ---
$(BUILDDIR)/kernel_entry.o: $(SRCDIR)/kernel_entry.asm | $(BUILDDIR)
	$(ASM) -f elf32 $< -o $@

# --- C kernel ---
$(BUILDDIR)/kernel.o: $(SRCDIR)/kernel.c $(SRCDIR)/console.h $(SRCDIR)/stdint.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Console driver ---
$(BUILDDIR)/console.o: $(SRCDIR)/console.c $(SRCDIR)/console.h $(SRCDIR)/stdint.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Link kernel ELF ---
$(BUILDDIR)/kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

# --- Convert ELF to flat binary ---
$(BUILDDIR)/kernel.bin: $(BUILDDIR)/kernel.elf
	$(OBJCOPY) -O binary $< $@

# --- Combine boot.bin + kernel.bin ---
$(BUILDDIR)/paramos.bin: $(BUILDDIR)/boot.bin $(BUILDDIR)/kernel.bin
	cat $(BUILDDIR)/boot.bin $(BUILDDIR)/kernel.bin > $@

$(BUILDDIR)/ports.o: $(SRCDIR)/ports.c $(SRCDIR)/ports.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BUILDDIR)/paramos.bin
	$(QEMU) -drive file=$(BUILDDIR)/paramos.bin,format=raw,index=0,media=disk

clean:
	rm -f $(BUILDDIR)/*.o $(BUILDDIR)/*.bin $(BUILDDIR)/*.elf
