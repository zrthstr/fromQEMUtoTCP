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
* https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf
* https://wiki.osdev.org/Rolling_Your_Own_Bootloader
* https://wiki.osdev.org/Exceptions#General_Protection_Fault
* https://github.com/thomasloven/mittos64/tree/master/doc
* https://github.com/tuhdo/os01/blob/master/Operating_Systems_From_0_to_1.pdf
* https://github.com/0xAX/linux-insides/blob/master/SUMMARY.md

Why does the MBR go to `x07c00`? -> https://www.glamenv-septzen.net/en/view/6

partially TBD - reading:
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
* https://en.wikipedia.org/wiki/BIOS_interrupt_call
* https://en.wikipedia.org/wiki/INT_13H
* https://en.wikipedia.org/wiki/Logical_block_addressing
* https://wiki.osdev.org/ATA_in_x86_RealMode_(BIOS)

Expand on v1, try to raw read/read via bios interup a few kb from hdd partition0 into ram and jump to

## v3-getting-to-c
Trying to get from asm to c.
This did but was to early as we were still in 16bit real mode..

reading:
* https://wiki.osdev.org/Creating_a_64-bit_kernel
* https://wiki.osdev.org/GCC_Cross-Compiler

## v4-getting-into-proteced-mode
set up gtd, then enable proteced mode
switched to bochs for now for simpler debugging of real mode..
reading:
* https://wiki.osdev.org/Protected_Mode
* https://wiki.osdev.org/Global_Descriptor_Table

## v5-propperly-to-c
make propper linker script
load 32 bit kernel as elf
print msg via VGA from c
reading:
* https://wiki.osdev.org/Printing_To_Screen
* https://wiki.osdev.org/Detecting_Colour_and_Monochrome_Monitors

## v6-tbd
* learn to read from a FS  ?
* switch to long mode / 64 bit?

* lets craft a IDT ?
* and trigger a fault ?
* and marvel at out blue screen ?
reading:
* https://wiki.osdev.org/Setting_Up_Long_Mode
* https://wiki.osdev.org/Interrupt_Descriptor_Table

## TBD
* paging
* 64bit longmode
* read from fs?
* IDT
* write a rudimentary kernel
* get to ring 3
* userland
* probe hardware
* init network card
* write network stack
