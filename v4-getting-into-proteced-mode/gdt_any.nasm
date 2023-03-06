gdt:
    ; Null descriptor
    dq 0x0
    dq 0x0
    
    ; Code descriptor with read and execute privileges for the entire memory
    dw 0xffff ; limit low
    dw 0x0    ; base low
    db 0x0    ; base middle
    db 0b10011010 ; access byte (present, ring 0, code segment, execute/read)
    db 0b11001111 ; granularity (4KB, 32-bit protected mode)
    db 0x0    ; base high

gdt_size:
    dw $-gdt-1 ; size of the GDT
gdt_start:
    dd gdt    ; address of the GDT

