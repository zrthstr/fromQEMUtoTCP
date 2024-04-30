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

build-grub:
	 grub/usr/bin/grub-mkstandalone -o ./bootx64.efi -d grub/usr/lib/grub/x86_64-efi -O x86_64-efi --modules="part_gpt part_msdos" --fonts="unicode" "boot/grub/grub.cfg=./grub.cfg"

fetch-grub-shit:
	sudo pacman -Sw grub
	mkdir grub; cp /var/cache/pacman/pkg/grub-2:2.12-2-x86_64.pkg.tar.zst grub/


kernel.bin: boot.o kernel.o
	ld -o kernel.bin -Ttext 0x1000 $^ --oformat binary

%.o: %.c
	gcc -ffreestanding -c $< -o $@

%.o: %.s
	as $< -o $@

clean:
	rm -rf *.bin *.o


## also: cp grub.cfg /tmp/tfpt/

sync-tftp:
	rm -rf /tmp/tftp
	cp kernel.bin ./tftp
	cp bootx64.efi ./tftp
	cp -r tftp /tmp/tftp
