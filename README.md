# From QEMU to TCP
... because nand2tetris was partially ungratifing

Building a simple OS for fun - trying to better understand things along the way - things mostly related to:
* OSdev
* compilers/linkers/parsing
* C/asm
* Kernels/OSes/Unix/POSIX
* Networks/TCP/IP
* cs in general

General reading:
* osdev-wiki
* https://wiki.osdev.org/Protected_Mode
* https://wiki.osdev.org/Rolling_Your_Own_Bootloader (!!)

TBD reading:
* https://akkadia.org/drepper/cpumemory.pdf
* https://manybutfinite.com/post/memory-translation-and-segmentation/
* https://manybutfinite.com/post/cpu-rings-privilege-and-protection/
* https://manybutfinite.com/post/getting-physical-with-memory/
* https://manybutfinite.com/post/how-the-kernel-manages-your-memory/
* https://manybutfinite.com/post/page-cache-the-affair-between-memory-and-files/
* https://manybutfinite.com/post/journey-to-the-stack/
* https://manybutfinite.com/post/system-calls/

## v1-hello-os
Reading:
* osdev-wiki
* https://manybutfinite.com/post/motherboard-chipsets-memory-map/
* https://manybutfinite.com/post/how-computers-boot-up/
* https://manybutfinite.com/post/kernel-boot-process/

Minimal bare bone 'OS', all inside MBR, running in x86 real mode, saying hello to the world

![QEMU with vnc](https://github.com/zrthstr/fromQEMUtoTCP/blob/main/v1-hello-os/doc/screen.png)

## v2-mbr-load-kernel-from-hdd
Reading:
* https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=42h:_Extended_Read_Sectors_From_Drive
* https://en.wikipedia.org/wiki/BIOS_interrupt_call (wip)
* https://en.wikipedia.org/wiki/INT_13H (wip)
* https://en.wikipedia.org/wiki/Logical_block_addressing (tbd)
* https://wiki.osdev.org/ATA_in_x86_RealMode_(BIOS) (tbd)

Expand on v1, try to raw read/read via bios interup a few kb from hdd partition0 into ram and jump to
