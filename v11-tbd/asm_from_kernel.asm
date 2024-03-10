DEFAULT REL


bits 64

SECTION .TEXT
bits 64
    GLOBAL asm_from_kernel

asm_from_kernel:
    ; Set up VGA text mode
   ; mov ax, 03h
   ; int 10h

    ; Print "Hello from GPT" to the screen
    mov edi, 0xb8000   ; VGA text memory address
    mov eax, 0x47616c6f6d6f6820 ; "GPT from aloH" in little-endian
    mov ebx, 0x6572206f6c6c6548 ; "Hello er"
    mov [edi], eax
    mov [edi+4], ebx

        ;; now for a change, lets do sth usefull
    ;;lidt [idtr]
; https://www.reddit.com/r/asm/comments/fsh25n/relocation_r_x86_64_32s_against_test_can_not_be/
; https://stackoverflow.com/questions/43367427/32-bit-absolute-addresses-no-longer-allowed-in-x86-64-linux
; https://stackoverflow.com/questions/71936758/relocation-against-text-can-not-be-used-when-making-a-pie-object-when-perform


; https://www.osdev.org/howtos/1/
    lidt [idt]
bits 64
    ret

section .data:
bits 64
; not sure about the alignment. To be sure, I will page align the idt for now.
align 4096

; the number of available interrupts, i.e. the number of entries in the idt
; ref section 8.2 p. 216 in AMD's programmers manual
IDT_NUM_ENTRIES equ 256

; the size of a single entry in the idt, in _long mode_, is 16 bytes.
; ref section 4.6.5 p. 79 in AMD's programmers manual
IDT_ENTRY_SIZE equ 16

; total size in bytes required for the idt
IDT_TOTAL_SIZE equ IDT_NUM_ENTRIES*IDT_ENTRY_SIZE

idt:
    resb IDT_TOTAL_SIZE

; file: idt.asm
;;;;global idtr

idtr:
    dw $ - idt - 1  ; two bytes (word), declaring the size of the IDT in bytes
    dq idt          ; 64-bit (double quad word) base address of the idt

;Now we have to set the IDTR.

; file: boot.asm
;;;;extern idtr

