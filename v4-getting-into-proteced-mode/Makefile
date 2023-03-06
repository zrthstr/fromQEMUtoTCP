DISK=disk.hdd
KERNEL=kernel.elf

#CC = gcc
CC = i686-elf-gcc

# https://wiki.osdev.org/Creating_a_64-bit_kernel

CSTYLE = -Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition
ASMTYPE = -masm=intel
CFLAG = -ffreestanding -mno-red-zone -O0 -mno-sse -mno-sse2 $(CSTYLE) $(ASMTYPE)

CCC = $(CC) $(CFLAG)

LDFLAG = -nostdlib -lgcc

######################
### bake the image ###
######################

DISK_PATTERN = disk_pattern

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
	#dd if=$(DISK_PATTERN) of=$(DISK) bs=1MB count=1 seek=1 conv=notrunc
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
	dd if=kernel.bin of=$(DISK) bs=1M count=1 seek=1 conv=notrunc
	#dd if=kernel.bin of=$(DISK) bs=1047040 count=1 seek=1 conv=notrunc

#image_disk_pattern:
#	python -c 'import string;print(string.ascii_letters*20)' > $(DISK_PATTERN)

##################
### asm to bin
#################

MBRASM = mbr.asm

translate_mbr:
	nasm mbr.asm -o mbr.bin
	objdump -D -b binary -mi386 -Maddr16,data16 mbr.bin

translate_kernel:
	@#i686-elf-gcc -O0 -c kernel.c -o kernel.o
	@#$(CCC) -T link.ld kernel.o -o kernel.bin -nostdlib -lgcc
	$(CCC) -T link.ld kernel.c -o kernel.bin -nostdlib -lgcc

	@#objdump -D -b binary -mi386 -Maddr16,data16 kernel.bin
	@#objdump -d kernel.bin -M intel intel-mnemonic
	@#i686-elf-gcc  -ffreestanding -mcmodel=large   -mno-red-zone  -mno-mmx -mno-sse -mno-sse2  -T link.ld  kernel.o   -o kernel.bin -nostdlib -lgcc

translate: translate_mbr translate_kernel

##############################
### run QEMU in debug mode ###
##############################

## see https://wiki.osdev.org/QEMU

Q_RAM=-m 2G

# -s   startes GBD stub (port)
# -S   mean wait for GDB to run. Usefull for debugging startup
Q_DEBUG=-S -s

Q_DRIVE = -drive file=$(DISK),media=disk,format=raw

bochs:
	#bochs -qf /dev/null -rc /dev/null 'megs:2000' 'boot:disk' 'ata0-master: type=disk, path="disk.hdd", mode=flat' 'display_library: nogui' 'magic_break: enabled=1' 'cpu: reset_on_triple_fault=1'
	bochs -qf /dev/null -rc /dev/null 'megs:2000' 'boot:disk' 'ata0-master: type=disk, path="disk.hdd", mode=flat' 'magic_break: enabled=1' 'cpu: reset_on_triple_fault=1'


run:
	#qemu-system-x86_64 -no-reboot -M q35 $(Q_RAM) $(Q_DEBUG) $(Q_DRIVE) -monitor stdio -no-shutdown 
	# qemu-system-i386
	qemu-system-x86_64 -no-reboot -M q35 $(Q_RAM) $(Q_DEBUG) $(Q_DRIVE) -monitor stdio  -d cpu_reset,int,guest_errors


###########
### GDB ###
###########

## > file <my-kernel-binary>
## # Inspect page tables
## x /8wg &page_tables_start
##
## gdb --eval-command='target remote :1234'

## layout asm ; change layout...
## layout regs
## display/i $pc
## x/i $pc
## set  disassemble-next-line on
## show disassemble-next-line
## stepi

## gdb -x <command-file>

#GDB_EC = --eval-command='target remote :1234; file $(KERNEL)'
GDB_EC = --eval-command='target remote :1234'
GDB_QUIET = -q
gdb:
	gdb $(GDB_QUIET) $(GDB_EC)

gdb_1:
	gdb $(GDB_QUIET) $(GDB_EC) --ex='break *0x7c00' --ex='c' --ex='info reg cs' --ex=si --ex='info reg cs'

vnc:
	vncviewer 127.0.0.1:5900

all: translate image_bake
