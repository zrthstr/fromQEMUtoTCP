[bits 16]  ; we start of in RealMode, apparently this implies 16 bit, but only kinda..
[org 0x7c00]
  cli                     ; disable interups

  xor ax, ax                  ; 0 AX
  mov ds, ax                  ; Set Data Segment to 0
  mov es, ax                  ; Set Extra Segment to 0
  mov ss, ax                  ; Set Stack Segment to 0
  ;mov sp, ax                 ; Set Stack pointer to 0 ; bad
  mov sp, 0x7c00              ; set stack pointer to something unused

  mov si, hello
  call PrintSt

  call CopyPart0

  mov si, done
  call PrintSt

  jmp 0xc00                   ; ze kernel we read from disk
  ;jmp 0x600                   ; ze kernel we read from disk

  call Forever


PrintSt:
  lodsb
  or al, al                   ; logical or AL by itself
  jz retPrint                  ; if the result is zero, get out
  ;ret
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

Forever:
  jmp 0:Forever

  hello db "Loading kernel from partition 0", 0x0D, 0x0A, 0
  done db "done copying" , 0x0D, 0x0A, 0
