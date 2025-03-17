[ORG 0x00]
[BITS 16]

SEGMENT .text
jmp 0x07C0:Entry
TOTALSECTORCOUNT: dw 0x06
KERNEL32SECTORCOUNT: dw 0x02

Entry:
   mov ax, 0x07C0
   mov ds, ax
   mov ax, 0xB800
   mov es, ax
   mov si, 0x00
   mov di, 0x00
   mov ax, 0x0000
   mov ss, ax
   mov sp, 0xFFFE
   mov bp, 0xFFFE


.EraserFullScreen:
   mov byte [es: si], 0x00
   mov byte [es: si+1], 0x07
   add si, 2
   cmp si, [fullScreenSize]
   jl .EraserFullScreen

   mov si, 0
   mov di, 0
   mov word [currentLine], 0

   ; print message
   push bootingMessage
   push word [currentLine]
   push 0
   call Puts
   add sp, 6

ResetDisk:
   mov ah, 0
   mov dl, [disk]
   int 0x13
   jc DiskError

   mov [sectorNumber], byte 0x02
   mov [trackNumber],  byte 0x00
   mov [headNumber], byte 0x00

   ;print message
   push diskResetMessage
   push word [currentLine]
   push 0
   call Puts
   add sp, 6

   mov si, 0x1000
   mov es, si
   mov bx, 0x0000

   mov di, [TOTALSECTORCOUNT]

ReadDisk:
	cmp di, 0
	je ReadEnd

	sub di, 0x01

	mov ah, 0x02
	mov al, 0x1
	mov ch, byte [trackNumber]
	mov cl, byte [sectorNumber]
	mov dh, byte [headNumber]
	mov dl, byte [disk]
	int 0x13
	jc DiskError

	add si, 0x20
	mov es, si

	mov al, byte [sectorNumber]
	add al, 0x01
	mov byte [sectorNumber], al
	cmp al, 18
	jle ReadDisk

	xor byte [headNumber], 0x1
	mov byte [sectorNumber], 0x01

	cmp byte [ headNumber ], 0x1
	je ReadDisk

	add byte [ trackNumber ], 0x01
	jmp ReadDisk

ReadEnd:
   ;print message
	push loadingCompleteMessage
	push word [currentLine]
	push 0
	call Puts
	add sp, 6
	mov bx, word [currentLine];currentLine backup

   jmp 0x1000:0x0000

DiskError:
	push diskErrorMessage
	push word [currentLine]
	push 0
	call Puts
	add sp, 6

	jmp $

Puts:; (x, y, message)
   push bp
   mov bp, sp
   push es
   push si
   push di
   push ax
   push bx
   push cx
   push dx

   mov ax, 0xB800
   mov es, ax

   mov ax, word [bp+6]; y coordinate
   mov si, [oneLineSize]
   mul si
   mov di, ax

   mov ax, word [bp+4]; x coordinate
   mov si, 2
   mul si
   add di, ax

   mov si, word [bp+8]; message

   .PutsLoop:
      mov dx, 0
      mov cl, byte[si]
      cmp cl, 0
      je .PutsEnd
      cmp cl, 0x0a
      je .NewLine
      jmp .Write
      .NewLine:
         mov ax, di
         div word [oneLineSize]
         sub di, dx
         add di, [oneLineSize]
         inc word [currentLine]
         jmp .NextChar

      .Write:
         mov byte [es: di], cl
         add di, 2
         mov ax, di
         div word [oneLineSize]
         cmp dx, 0
         jne .NextChar
         inc word [currentLine]

      .NextChar:
         add si, 1
         jmp .PutsLoop

.PutsEnd:
   pop dx
   pop cx
   pop bx
   pop ax
   pop di
   pop si
   pop es
   pop bp
   ret

;message
bootingMessage: db 'Hello!', 0x0a, 'Welcome to skhuOS!', 0x0a, 0
diskErrorMessage: db 'Disk Error', 0x0a, 0
diskResetMessage: db 'Disk Reset', 0x0a, 0
loadingCompleteMessage: db 'Disk img loading complete', 0x0a, 0

;about Paints
fullScreenSize: dw 80*25*2
currentLine: dw 0x0000
oneLineSize: dw 80*2

;about disk
disk: db 0x00
hardDisk1: db 0x81
hardDisk2: db 0x82
trackNumber: db 0x00
sectorNumber: db 0x02
headNumber: db 0x00

times 510 - ($-$$) db 0x00
dw 0xAA55
