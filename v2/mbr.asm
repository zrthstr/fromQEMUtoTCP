[bits 16]  ; we start of in RealMode, apparently this implies 16 bit, but only kinda..
[org 0x7c00]

; When the BIOS loads and runs an MBR, it is loaded into memory at physical address 0x7c00.
; This is usually 0x0000:0x7c00 (CS = 0, offset address 0x7c00)

;we dont want to be interuped

  cli                     ; disable interups

;we then want to set up a stack so we can run code propperly.
;There is no guarantee what is in our registers.

; We want to null ax, ds, es, ss, sp

  xor ax, ax                  ; 0 AX
  mov ds, ax                  ; Set Data Segment to 0
  mov es, ax                  ; Set Extra Segment to 0
  mov ss, ax                  ; Set Stack Segment to 0
  mov sp, ax                  ; Set Stack pointer to 0

  ; nope, this is bad...
  ;mov cs, ax                  ; would we not also need to set this?


  mov si, hello
  jmp 0:PrintSt

;  jmp 0:Count

Count:
  add ax, 1
  add ax, 1
  add ax, 1
  add ax, 1
  add ax, 1
  jmp 0:Count


  ; 48 65 6c 6c 6f 20 57 6f 72 6c 64 21
  ;Copy:
  ;  mov bx, 0x600
  ;  mov WORD [bx], 0x48 ; 0x6548 ?

PrintSt:
  lodsb
  or al, al  ; logical or AL by itself
  jz Forever   ; if the result is zero, get out
  mov ah, 0x0E
  int 0x10      ; otherwise, print out the character!
  jmp 0:PrintSt

Forever:
  jmp 0:Forever

  hello db "some hello text", 0x0D, 0x0A, 0
