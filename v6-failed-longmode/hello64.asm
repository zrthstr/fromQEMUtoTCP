;;;
;;;  nasm hello64.asm -o hello64.bin
;;;

section .text
bits 64
longstart:
    mov word [0xb8000], 0x0e4f ; 'O', yellow on black
    mov word [0xb8002], 0x0e4b ; 'K', yellow on black

    hlt


align 4096
;p4_table:
;    resb 4096
;p3_table:
;    resb 4096
;p2_table:
;    resb 4096
;stack_bottom:
;       resb 64
;stack_top:
;

section .rodata
gdt64:
        dq 0
.code: equ $ - gdt64
        dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.pointer:
        dw $ - gdt64 - 1 ; length of the gdt64 table
        dq gdt64         ; addess of the gdt64 table

section .text
