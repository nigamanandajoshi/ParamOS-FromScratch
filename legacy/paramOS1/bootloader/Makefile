# Param OS - Bootloader + Stage-2 Kernel (real mode)

NASM = nasm
QEMU = qemu-system-x86_64

all: paramos.bin

boot.bin: boot.asm
	$(NASM) -f bin boot.asm -o boot.bin

kernel.bin: kernel.asm
	$(NASM) -f bin kernel.asm -o kernel.bin
	# pad kernel to next 512-byte boundary
	dd if=/dev/zero bs=1 count=$$((512 - ($$(stat -f%z kernel.bin) % 512))) 2>/dev/null >> kernel.bin

paramos.bin: boot.bin kernel.bin
	cat boot.bin kernel.bin > paramos.bin

run: paramos.bin
	$(QEMU) -drive format=raw,file=paramos.bin

clean:
	rm -f boot.bin kernel.bin paramos.bin
