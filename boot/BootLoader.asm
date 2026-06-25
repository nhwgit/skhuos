[ORG 0x00]
[BITS 16]

SEGMENT .text
jmp 0x07C0:Entry

BootInfo:; 레이아웃은 common/BootInfo.h와 일치. 섹터 수는 빌드 시 ImageMaker가 시그니처를 찾아 기록
	db 'SKHUBOOT'
	TOTALSECTORCOUNT: dw 0x0000
	LOADERSECTORCOUNT: dw 0x0000

Entry:
   mov ax, 0x07C0
   mov ds, ax
   mov byte [disk], dl; BIOS가 DL에 부팅 드라이브 번호를 넘겨줌
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

   ;print message
   push diskResetMessage
   push word [currentLine]
   push 0
   call Puts
   add sp, 6

   mov di, [TOTALSECTORCOUNT]

ReadDisk:
	cmp di, 0
	je ReadEnd

	sub di, 0x01

	mov si, DAP
	mov ah, 0x42; INT 13h 확장 읽기 (LBA)
	mov dl, byte [disk]
	int 0x13
	jc DiskError

	add word [DAP+6], 0x20; 다음 512바이트 (세그먼트 단위)
	add word [DAP+8], 0x01; 다음 LBA
	adc word [DAP+10], 0x00
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
disk: db 0x80

DAP:; INT 13h 확장 읽기용 Disk Address Packet
	db 0x10; 패킷 크기
	db 0x00
	dw 0x0001; 읽을 섹터 수
	dw 0x0000; 대상 오프셋
	dw 0x1000; 대상 세그먼트
	dd 0x00000001; 시작 LBA (부트로더 다음 섹터)
	dd 0x00000000

times 510 - ($-$$) db 0x00
dw 0xAA55
