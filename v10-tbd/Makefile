### make things linux distri independant

CC = x86_64-linux-gnu-gcc
OBJDUMP = objdump

ifeq ($(shell lsb_release -si),Debian)
    CC = x86_64-linux-gnu-gcc-10
    OBJDUMP = x86_64-linux-gnu-objdump
endif


DISK=disk.hdd
KERNEL=kernel.elf

#CC = gcc
#CC = i686-elf-gcc

CSTYLE = -Wall -Wextra -std=c11 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition
ASMTYPE = -masm=intel
CFLAG = -ffreestanding -mno-red-zone -O0 -mno-sse -mno-sse2 $(CSTYLE) $(ASMTYPE) -fno-stack-protector
LDFLAG = -nostdlib -lgcc

CCC = $(CC) $(CFLAG)




######################
### bake the image ###
######################

image_cat_noempty:
	cat $(DISK) | xxd | grep -v '0000 0000 0000 0000 0000 0000 0000 0000'

image_rm:
	rm -f $(DISK)

image_bake: image_rm
	dd if=/dev/zero bs=1M count=64 of=$(DISK)
	parted -s $(DISK) mklabel gpt
	#parted -s $(DISK) mkpart ESP fat32 2048s 100%
	# 2048s -> 1M so we have 2M - 2048s = 1M
	parted -s $(DISK) mkpart ESP fat32 2048s 2M
	parted -s $(DISK) mkpart ESP fat32 2M 12M
	parted -s $(DISK) set 1 esp on
	make image_write_mbr
	make image_read_mbr
	make image_write_kernel
	make image_read_kernel

image_info:
	fdisk -l $(DISK)

image: image_bake image_info

image_read_mbr:
	dd if=$(DISK) bs=1 count=446 | xxd

image_read_kernel:
	cat kernel.bin | xxd | grep -v '6690 6690 6690 6690 6690 6690 6690 6690' | grep -v '0000 0000 0000 0000 0000 0000 0000 0000'

image_write_mbr:
	dd if=mbr.bin bs=1M count=64 of=$(DISK) conv=notrunc seek=0

image_write_kernel:
	@# we are cutting of the elf headder and the padding. not sure how to do this propperly via linker??
	@# 1536 -> 0x600
	###dd if=kernel.bin of=kernel.bin_cut bs=1536 skip=1
	###mn kernel.bin_cut kernel.bin
	dd if=kernel.flat of=$(DISK) bs=1M count=1 seek=1 conv=notrunc
	#dd if=kernel.bin of=$(DISK) bs=1047040 count=1 seek=1 conv=notrunc

##################
### asm to bin ###
##################

MBR_ASM = mbr.asm
MBR_BIN = mbr.bin

translate_mbr:
	nasm $(MBR_ASM) -o $(MBR_BIN)
	$(OBJDUMP) -D -b binary -mi386 -Maddr16,data16 $(MBR_BIN)


translate_modules:
	nasm -f elf64 idt.asm -o idt.o

translate_kernel:
	$(CCC) -T link.ld kernel.c lib/std.c idt.o -o kernel.bin -nostdlib -lgcc  -Wl,-no-pie
	#$(CCC) -T link.ld kernel.c lib/std.c -o kernel.bin -nostdlib -lgcc  -Wl,-no-pie
	objcopy -O binary kernel.bin kernel.flat


	@#objdump -D -b binary -mi386 -Maddr16,data16 kernel.bin
	#objdump -d kernel.bin -M intel,intel-mnemonic | grep -v 'xchg   ax,ax'

translate: translate_mbr translate_modules translate_kernel

##############################
### run QEMU in debug mode ###
##############################

Q_RAM=-m 2G
Q_DEBUG=-S -s
Q_DRIVE = -drive file=$(DISK),media=disk,format=raw

bochs:
	bochs -qf /dev/null -rc /dev/null 'megs:2000' 'boot:disk' 'ata0-master: type=disk, path="disk.hdd", mode=flat' 'magic_break: enabled=1' 'cpu: reset_on_triple_fault=1'

run:
	qemu-system-x86_64 -no-reboot -M q35 $(Q_RAM) $(Q_DEBUG) $(Q_DRIVE) -monitor stdio  -d cpu_reset,int,guest_errors

run_32:
	qemu-system-i386 -no-reboot -M q35 $(Q_RAM) $(Q_DEBUG) $(Q_DRIVE) -monitor stdio  -d cpu_reset,int,guest_errors


###########
### GDB ###
###########

#GDB_EC = --eval-command='target remote :1234; file $(KERNEL)'
GDB_EC = --eval-command='target remote :1234'
GDB_QUIET = -q
gdb:
	gdb $(GDB_QUIET) $(GDB_EC)

gdb_1:
	gdb $(GDB_QUIET) $(GDB_EC) --ex='break *0x7c00' --ex='c' --ex='info reg cs' --ex=si --ex='info reg cs'

vnc:
	vncviewer 127.0.0.1:5900

all: translate image_bake bochs
