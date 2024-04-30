.section .text
.global _start
_start:
    call kmain
    hlt
