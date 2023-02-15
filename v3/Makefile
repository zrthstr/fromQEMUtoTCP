DISK=disk.hdd
KERNEL=kernel.elf

#CC = gcc
CC = i686-elf-gcc
CFLAG = -ffreestanding -mno-red-zone -O0

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
	cat kernel.bin | xxd 

image_write_mbr:
	dd if=mbr.bin bs=1M count=64 of=$(DISK) conv=notrunc seek=0

image_write_kernel:
	#dd if=kernel.bin of=$(DISK) bs=1MB count=1 seek=1 conv=notrunc
	dd if=kernel.bin of=$(DISK) bs=1M count=1 seek=1 conv=notrunc

image_disk_pattern:
	python -c 'import string;print(string.ascii_letters*20)' > $(DISK_PATTERN)

##################
### asm to bin
#################

MBRASM = mbr.asm

translate_mbr:
	nasm mbr.asm -o mbr.bin
	objdump -D -b binary -mi386 -Maddr16,data16 mbr.bin

translate_kernel:
	#nasm kernel.asm -o kernel.bin
	#objdump -D -b binary -mi386 -Maddr16,data16 kernel.bin
	#$(CC) -S  kernel.c -o kernel.s
	$(CCC) kernel.c
	nasm kernel.s -o kernel.bin

translate: translate_mbr translate_kernel

##############################
### run QEMU in debug mode ###
##############################

## see https://wiki.osdev.org/QEMU

Q_RAM=-m 2G

# -s   startes GBD stub (port)
# -S   mean wait for GDB to run. Usefull for debugging startup
Q_DEBUG=-S -s


### qemu -drive
# Instead of -hda, -hdb, -hdc, -hdd, you can use:
#
# qemu-system-x86_64 -drive file=file,index=0,media=disk
# qemu-system-x86_64 -drive file=file,index=1,media=disk
# qemu-system-x86_64 -drive file=file,index=2,media=disk
# qemu-system-x86_64 -drive file=file,index=3,media=disk

#Q_DRIVE = -hda
Q_DRIVE = -drive file=$(DISK),media=disk,format=raw

run:
	qemu-system-x86_64 -M q35 $(Q_RAM) $(Q_DEBUG) $(Q_DRIVE)


###########
### GDB ###
###########

## > file <my-kernel-binary>
## > remote target :1234
## 
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
