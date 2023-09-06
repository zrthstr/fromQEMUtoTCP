[bits 16]  ; we start of in RealMode, apparently this implies 16 bit, but only kinda..
[org 0x7c00]
  cli                     ; disable interups

  xor ax, ax                  ; 0 AX
  mov ds, ax                  ; Set Data Segment to 0
  mov es, ax                  ; Set Extra Segment to 0
  mov ss, ax                  ; Set Stack Segment to 0
  mov sp, 0x7c00              ; set stack pointer to something unused
  mov bp, sp                  ; same with bp

  mov si, hello
  call PrintSt

  call CopyPart0

  mov si, done_cp
  call PrintSt

  mov si, stpm
  call PrintSt
  call SwitchToPMode


PrintSt:
  lodsb
  or al, al                   ; logical or AL by itself
  jz retPrint                 ; if the result is zero, get out
  mov ah, 0x0E
  int 0x10                    ; otherwise, print out the character!
  jmp 0:PrintSt
retPrint:
  ret

CopyPart0:
  ; lets copy N words from Partititon 0 to ram
  mov si, DAPACK          ; address of "disk address packet"
  mov ah, 0x42            ; AL is unused
  mov dl, 0x80            ; drive number 0 (OR the drive # with 0x80)
  int 0x13

  ;jc short .error
  mov si, done
  ret

SwitchToPMode:
  cli ; bouble is better

  lgdt [ gdt_descriptor ]
  mov eax, cr0 ; To make the switch to protected mode , we set
  or eax, 0x1  ; the first bit of CR0 , a control register
  mov cr0, eax ; Update the control register

  jmp CODE_SEG:init_pm

[bits 32]
init_pm:
  mov ax, DATA_SEG ; Now in PM , our old segments are meaningless ,
  mov ds, ax ; so we point our segment registers to the
  mov ss, ax ; data selector we defined in our GDT
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ebp, 0x90000 ; Update our stack position so it is right
  mov esp, ebp ; at the top of the free space.

  mov ebx, MSG_PROT_MODE
  mov ecx, 2 * 21 * 80
  call print_string_pm


  call foooo

  ;jmp 0xc00                   ; ze kernel we read from disk
  jmp 0x600                   ; ze kernel we read from disk

  call Forever32 ; Finally , call some well - known label


Forever32:
  jmp Forever32

%include "pm_print.asm"

DAPACK:
  db      0x10            ; size of DAP (disk address package)
  db      0               ; unused, should be zero
blkcnt:
  dw      16              ; int 13 resets this to # of blocks actually read/written
                          ; number of blocks to read
db_add:
  dw      0x0600          ; memory buffer destination address
  dw      0               ; in memory page zero
d_lba:
  dd      2048            ; put the lba to read in this spot
                          ; in our case (all modern cases??) disk kb * 2
  dd      0               ; more storage bytes only for big lba's ( > 4 bytes )

%include "gdt.asm"

; String
  hello db "Loading kernel from partition 0", 0x0D, 0x0A, 0
  done_cp db "done copying" , 0x0D, 0x0A, 0
  stpm db "switching to pmode", 0x0D, 0x0A, 0
  done db "done.." , 0x0D, 0x0A, 0
  jtc db "jumping to kernel", 0x0D, 0x0A, 0
  MSG_PROT_MODE db "CPU is now in protected mode", 0

bits 32

PTE_PRESENT equ 1 << 7

; Flags for _large_ p2 aka. PDE page table entries
PDE_PRESENT  equ 1 << 0
PDE_WRITABLE equ 1 << 1
PDE_LARGE    equ 1 << 7


foooo:
    ; Switching to long mode
    ;
    ; Step 1: Disable paging
    ;
    ; to disable paging set `CR0.PG` to `0`.
    mov eax, cr0
    and eax, ~(1 << 31)
    mov cr0, eax

    ; Step 2: Enable Physical Address Extension
    mov eax, cr4
    or eax, (1 << 5)
    mov cr4, eax

    ; Step 3: Set `cr3` register
    mov eax, p4_table
    mov cr3, eax

    ; Step 4: Set the p2[1] entry to point to the _second_ 2 MiB frame
        mov eax, (0x20_0000 | PDE_PRESENT | PDE_WRITABLE | PDE_LARGE)
        mov [p2_table + 8], eax

        ; point the 0th entry to the first frame
        ; TODO: explain
        mov eax, (0x00_0000 | PDE_PRESENT | PDE_WRITABLE | PDE_LARGE)
        mov [p2_table], eax

        ; Step 5: Set the 0th entry of p3 to point to our p2 table
        mov eax, p2_table ; load the address of the p2 table
        or eax, (PDE_PRESENT | PDE_WRITABLE)
        mov [p3_table], eax

        ; Step 6: Set the 0th entry of p4 to point to our p3 table
        mov eax, p3_table
        or eax, (PDE_PRESENT | PDE_WRITABLE)
        mov [p4_table], eax

        ; Step 7: Set EFER.LME to 1 to enable the long mode
        mov ecx, 0xC0000080
        rdmsr
        or  eax, 1 << 8
        wrmsr

        ; Step 8: enable paging
        mov eax, cr0
        or eax, 1 << 31
        mov cr0, eax

    lgdt [gdt64.pointer]
    jmp gdt64.code:longstart


section .text
bits 64
longstart:
    mov word [0xb8000], 0x0e4f ; 'O', yellow on black
    mov word [0xb8002], 0x0e4b ; 'K', yellow on black

    jmp 0x600
    hlt

section .bss
; must be page aligned
align 4096
p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096
stack_bottom:
        resb 64
stack_top:

section .rodata
gdt64:
        dq 0
.code: equ $ - gdt64
        dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.pointer:
        dw $ - gdt64 - 1 ; length of the gdt64 table
        dq gdt64         ; addess of the gdt64 table

section .text
