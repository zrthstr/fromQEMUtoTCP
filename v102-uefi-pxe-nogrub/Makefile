NET = -net nic -net tap,ifname=tap0,script=no,downscript=no
run:
	qemu-system-x86_64 -L OVMF_dir/ -pflash OVMF.fd $(NET)

vnc:
	vncviewer 127.0.0.1:5900

config_network:
	sudo ip tuntap add dev tap0 mode tap user $(whoami)
	sudo ip link set tap0 up
	sudo ip addr add 192.168.200.1/24 dev tap0

dhcp:
	dnsmasq --conf-file=dnsmasq.conf --no-daemon

# --keep-in-foreground  for less debug noises

compile:
	gcc -Ignu-efi/inc -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -c main.c -o main.o

link:
	ld -shared -Bsymbolic -Lgnu-efi/x86_64/lib -Lgnu-efi/x86_64/gnuefi -Tgnu-efi/gnuefi/elf_x86_64_efi.lds gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o main.o -o main.so -lgnuefi -lefi

convert:
	objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 main.so main.efi


deploy:
	cp main.efi /tmp/tftp/BOOTX64.EFI

sync-tftp:
	#rm -rf /tmp/tftp
	#cp  ./tftp
	#cp -r tftp /tmp/tftpBOOTX64.EFI
