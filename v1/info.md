## startup

### high level plan
* MBR code prepares the env so a next stage can be loade
* load next stage from disk
* transfers controll to next stage
* next stage could possible allready be the final kernel..
* at some point we switch from RealMode ot ProtecedMode

### low level plan

    [bits 16]  ; we start of in RealMode, apparently this implies 16 bit, but only kinda..

When the BIOS loads and runs an MBR, it is loaded into memory at physical address 0x7c00.
This is usually 0x0000:0x7c00 (CS = 0, offset address 0x7c00)

we dont want to be interuped

    cli                     ; disable interups

we then want to set up a stack so we can run code propperly.
There is no guarantee what is in our registers.

We want to null ax, ds, es, ss, sp

  xor ax, ax                  ; 0 AX
  mov ds, ax                  ; Set Data Segment to 0
  mov es, ax                  ; Set Extra Segment to 0
  mov ss, ax                  ; Set Stack Segment to 0
  mov sp, ax                  ; Set Stack pointer to 0

Apparently we want to relocate our own position in ram, then jump there..


## memory layout
https://wiki.osdev.org/Memory_Map_(x86)

## detecting memory - how to ask the bios WTF is going on
https://wiki.osdev.org/Detecting_Memory_(x86)

## our disk layout

Offset  Size (bytes)    Description
----------------------------------------------------------
0x000   440  MBR Bootstrap (flat binary executable code)
0x1B8     4  Optional "Unique Disk ID / Signature"
0x1BC     2  Optional, reserved 0x0000
0x1BE    16  First partition table entry
0x1CE    16  Second partition table entry
0x1DE    16  Third partition table entry
0x1EE    16  Fourth partition table entry
0x1FE     2  (0x55, 0xAA) "Valid bootsector" signature bytes


## mbr
https://wiki.osdev.org/MBR_(x86)

// there are many kinds of MBRs, we will try to use a 'classical generic MBR'
https://en.wikipedia.org/wiki/Master_boot_record 


## bootloader
https://wiki.osdev.org/Rolling_Your_Own_Bootloader
https://wiki.osdev.org/Bootloader
https://wiki.osdev.org/Bootloaders


## GDB
https://wiki.osdev.org/Kernel_Debugging
https://wiki.osdev.org/GDB
