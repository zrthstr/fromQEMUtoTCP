new iteration. In order to dexomplexify we are now building on top of UEFI.
This means we start in long mode.
So no:
* real mode code
* switching to protected mode
* dealing with 32 bit paging?
* switching to long mode...

Apparenlty we "JUST" get to born in long mode.


bochs does not suport UEFI, so just QEMU form now on.


## uefi
find / -iname "OVMF.fd" 2>/dev/null
/usr/share/edk2/x64/OVMF.fd
/usr/share/edk2/ia32/OVMF.fd

## boot
lets boot from the network, via PXE
lets use dnsmasq for dhcp and tftp
load a imple grub via PXE/TFTP
points to grub config, with our kernel..
