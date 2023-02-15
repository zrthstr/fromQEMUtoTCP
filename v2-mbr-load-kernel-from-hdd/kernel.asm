; gets loaded by mbr

[bits 16]
[org 0x0600]

Forever:
  add ax, 1
  jmp 0:Forever
