
[bits 16]  ; we start of in RealMode, apparently this implies 16 bit, but only kinda..
[org 0x0]

section .text
  cli                     ; disable interups

  xor ax, ax                  ; 0 AX
  mov ds, ax                  ; Set Data Segment to 0
  mov es, ax                  ; Set Extra Segment to 0
  mov ss, ax                  ; Set Stack Segment to 0
  mov sp, 0x7c00              ; set stack pointer to something unused
  mov bp, sp                  ; same with bp

section .data

NOTDAPACK:
  db      0x10            ; size of DAP (disk address package)
  db      0               ; unused, should be zero
notblkcnt:
  dw      16              ; int 13 resets this to # of blocks actually read/written
                          ; number of blocks to read
notdb_add:
  dw      0x0600          ; memory buffer destination address
  dw      0               ; in memory page zero
notd_lba:
  dd      2048            ; put the lba to read in this spot
                          ; in our case (all modern cases??) disk kb * 2
  dd      0               ; more storage bytes only for big lba's ( > 4 bytes )


[bits 32]

section .data

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

