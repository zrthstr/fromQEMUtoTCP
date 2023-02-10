DISK=disk.hdd
KERNEL=kernel.elf

######################
### bake the image ###
######################

image_rm:
	rm -f $(DISK)

image_bake: image_rm
	dd if=/dev/zero bs=1M count=64 of=$(DISK)
	parted -s $(DISK) mklabel gpt
	parted -s $(DISK) mkpart ESP fat32 2048s 100%
	parted -s $(DISK) set 1 esp on

image_info:
	fdisk -l $(DISK)

image: image_bake image_info

image_read_mbr:
	dd if=$(DISK) bs=1 count=446 | xxd

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
	qemu-system-x86_64 -M q35 $(Q_RAM) $(Q_DEBUG) $(DRIVE)


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


#GDB_EC = --eval-command='target remote :1234; file $(KERNEL)'
GDB_EC = --eval-command='target remote :1234'
gdb:
	gdb $(GDB_EC)



