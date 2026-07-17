[BITS 64]

; 링3 데모 프로그램 — 커널이 USER_CODE_ADDRESS로 복사한 뒤 유저 모드로 실행한다.
; 복사 후에도 유효하도록 데이터 참조는 전부 RIP 상대([rel])이며 커널 심볼 참조 금지(자기 완결).
; 시스템 콜 번호는 syscall/Syscall.h, 벡터는 arch/Descriptor.h와 일치해야 함

SYSCALL_WRITEXY equ 1
SYSCALL_SLEEP   equ 2
SYSCALL_GETPID  equ 3
SYSCALL_EXIT    equ 4
SYSCALL_VECTOR  equ 0x81

SEGMENT .data
global userProgramStart, userProgramEnd

userProgramStart:
	; rdi: 0=정상 종료 데모, 1=커널 메모리 접근 데모(보호 증명)
	mov r13, rdi
	mov r12, 10                ; 표시 줄 (공격 모드는 12)
	test r13, r13
	jz .formatCS
	mov r12, 12

.formatCS:
	; CS 하위 바이트를 메시지에 16진수로 기록 — CPL=3 증거
	mov ax, cs
	mov bl, al
	shr bl, 4
	and bl, 0x0F
	cmp bl, 9
	jbe .highDigit
	add bl, 7                  ; 'A'-'0'-10
.highDigit:
	add bl, '0'
	mov [rel csHigh], bl
	mov bl, al
	and bl, 0x0F
	cmp bl, 9
	jbe .lowDigit
	add bl, 7
.lowDigit:
	add bl, '0'
	mov [rel csLow], bl
	and al, 0x03
	add al, '0'
	mov [rel cplChar], al

	mov rax, SYSCALL_WRITEXY
	mov rdi, r12
	mov rsi, 2
	lea rdx, [rel helloMsg]
	int SYSCALL_VECTOR

	xor r14, r14
.tickLoop:
	mov rax, SYSCALL_SLEEP
	mov rdi, 400
	int SYSCALL_VECTOR
	mov rax, SYSCALL_WRITEXY
	mov rdi, r12
	lea rsi, [r14 + 44]
	lea rdx, [rel tickMsg]
	int SYSCALL_VECTOR
	inc r14
	cmp r14, 5
	jb .tickLoop

	test r13, r13
	jnz .attack

	mov rax, SYSCALL_WRITEXY
	mov rdi, r12
	mov rsi, 52
	lea rdx, [rel exitMsg]
	int SYSCALL_VECTOR
	mov rax, SYSCALL_EXIT
	int SYSCALL_VECTOR
.hang:
	jmp .hang                  ; 도달하지 않음

.attack:
	mov rax, SYSCALL_WRITEXY
	mov rdi, r12
	mov rsi, 52
	lea rdx, [rel attackMsg]
	int SYSCALL_VECTOR
	mov rax, [0x600000]        ; 커널 영역 읽기 시도 → #PF → 커널이 이 프로세스만 종료
	jmp .hang

helloMsg:  db "Hello from Ring 3! CS=0x"
csHigh:    db "?"
csLow:     db "?"
           db " CPL="
cplChar:   db "?", 0
tickMsg:   db ".", 0
exitMsg:   db "exit syscall", 0
attackMsg: db "read kernel->", 0
userProgramEnd:
