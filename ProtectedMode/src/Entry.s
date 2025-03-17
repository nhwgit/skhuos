[ORG 0x00]
[BITS 16]

SEGMENT .text
Entry:;bx는 ProtectedMode에서 currentLine으로 백업됨
   mov ax, 0x1000
   mov ds, ax
   mov es, ax

   ;A20 activate
   in al, 0x92
   mov al, 0x02
   out 0x92, al

   cli
   lgdt [gdtr]

   mov eax, 0x4000D03B
   mov cr0, eax
   jmp dword 0x20: ProtectedMode-$$+0x10000

[BITS 32]
ProtectedMode:
   mov [currentLine], bx;currentLine load
   mov ax, 0x18
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   mov ss, ax
   mov esp, 0x9FFE
   mov ebp, 0x9FFE

   push (switchToProtectMode-$$+0x10000)
   push dword [currentLine-$$+0x10000]
   push 0
   call Puts
   add esp, 12

   jmp dword 0x20: 0x10200

Puts:; (x, y, message)
   push ebp
   mov ebp, esp
   push esi
   push edi
   push eax
   push ebx
   push ecx
   push edx

   mov eax, dword [ebp+12]; y coordinate
   mov esi, [oneLineSize-$$+0x10000]
   mul esi
   mov edi, eax

   mov eax, dword [ebp+8]; x coordinate
   mov esi, 2
   mul esi
   add edi, eax

   mov esi, dword [ebp+16]; message

   .PaintLoop:
      mov edx, 0
      mov cl, byte[esi]
      cmp cl, 0
      je .PaintEnd
      cmp cl, 0x0a
      je .NewLine
      jmp .Write
      .NewLine:
         mov eax, edi
         div dword [oneLineSize-$$+0x10000]
         sub edi, edx
         add edi, [oneLineSize-$$+0x10000]
         inc dword [currentLine-$$+0x10000]
         jmp .NextChar

      .Write:
         mov byte [edi+0xB8000], cl
         add edi, 2
         mov ax, di
         div word [oneLineSize-$$+0x10000]
         cmp dx, 0
         jne .NextChar
         inc dword [currentLine-$$+0x10000]

      .NextChar:
         add esi, 1
         jmp .PaintLoop

.PaintEnd:
   pop edx
   pop ecx
   pop ebx
   pop eax
   pop edi
   pop esi
   pop ebp
   ret


gdtr:
   dw gdtEnd - gdt - 1
   dd gdt+0x10000-$$

gdt:
   dw 0
   dw 0
   db 0
   db 0
   db 0
   db 0

DataDescriptor:
	dw 0xFFFF
	dw 0x0000
	db 0x00
	db 0x92
	db 0xAF
	db 0x00

CodeDescriptor:
	dw 0xFFFF
	dw 0x0000
	db 0x00
	db 0x9A
	db 0xAF
	db 0x00

DataDescriptor32bit:
	dw 0xFFFF
	dw 0x0000
	db 0x00
	db 0x92
	db 0xCF
	db 0x00

CodeDescriptor32bit:
   dw 0xFFFF
   dw 0x0000
   db 0x00
   db 0x9A
   db 0xCF
   db 0x00

gdtEnd:

;message
switchToProtectMode: db 'Switch to Protected Mode', 0x0a, 0
;about paint
fullScreenSize: dd 80*25*2
currentLine: dd 0x0000
oneLineSize: dd 80*2

;about gdt
descriptorCount: dw 0x02

times 512 - ($-$$) db 0x00
