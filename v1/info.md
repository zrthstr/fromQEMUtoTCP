
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


