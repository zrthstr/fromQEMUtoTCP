gdt64:
        dq 0
.code: equ $ - gdt64
        dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.pointer:
        dw $ - gdt64 - 1 ; length of the gdt64 table
        dq gdt64         ; addess of the gdt64 table
