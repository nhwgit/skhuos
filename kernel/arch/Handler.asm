[BITS 64]
SEGMENT .text

extern testHandler, keyboardHandler, timerHandler, disk1Handler, disk2Handler, exceptionHandler

%macro SAVEREG 0
    push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rbp
	push rdi
	push rsi
	push rdx
	push rcx
	push rbx
	push rax

	mov ax, ds
	push rax
	mov ax, es
	push rax
	push fs
	push gs

    mov ax, 0x08
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
%endmacro

%macro LOADREG 0
    pop gs
	pop fs
	pop rax
	mov es, ax
	pop rax
	mov ds, ax

	pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rsi
	pop rdi
	pop rbp
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
%endmacro

; 에러 코드가 없는 벡터 (벡터 번호, 핸들러 함수)
%macro ISR 2
global IV%1
IV%1:
    SAVEREG
    mov rdi, %1
    call %2
    LOADREG
    iretq
%endmacro

SAVEREG_SIZE equ 19*8 ; SAVEREG가 쌓는 크기 (GPR 15개 + 세그먼트 4개) — CPU가 쌓은 프레임 접근 오프셋

; 에러 코드가 없는 예외 (벡터 번호) — exceptionHandler가 halt하므로 복귀 없음
%macro EXC 1
global IV%1
IV%1:
    SAVEREG
    mov rdi, %1
    xor rsi, rsi
    mov rdx, qword [rsp + SAVEREG_SIZE] ; RIP
    call exceptionHandler
%endmacro

; 에러 코드가 있는 예외 (벡터 번호)
%macro EXC_ERRCODE 1
global IV%1
IV%1:
    SAVEREG
    mov rdi, %1
    mov rsi, qword [rsp + SAVEREG_SIZE]     ; 에러 코드
    mov rdx, qword [rsp + SAVEREG_SIZE + 8] ; RIP
    call exceptionHandler
%endmacro

; 예외 핸들러
EXC         00 ; divide error
EXC         01 ; debug error
EXC         02 ; NMI
EXC         03 ; BreakPoint
EXC         04 ; Overflow
EXC         05 ; BoundRangeExceeded
EXC         06 ; InvalidOpcode
EXC         07 ; DeviceNotAvailable
EXC_ERRCODE 08 ; DoubleFault
EXC         09 ; CoprocessorSegmentOverrun
EXC_ERRCODE 10 ; InvalidTSS
EXC_ERRCODE 11 ; SegmentNotPresent
EXC_ERRCODE 12 ; StackSeg
EXC_ERRCODE 13 ; GeneralProtection
EXC_ERRCODE 14 ; PageFault
EXC         15
EXC         16 ; FPUError
EXC_ERRCODE 17 ; AlignmentCheck
EXC         18 ; MachineCheck
EXC         19 ; SIMDError
EXC         20 ; 21 이상 예약 벡터도 IDT에서 IV20으로 수렴

; 인터럽트 핸들러
ISR 32, timerHandler    ; Timer
ISR 33, keyboardHandler ; Keyboard
ISR 34, testHandler     ; SlavePIC
ISR 35, testHandler     ; Serial2
ISR 36, testHandler     ; Serial1
ISR 37, testHandler     ; Parallel2
ISR 38, testHandler     ; Floppy
ISR 39, testHandler     ; Parallel1
ISR 40, testHandler     ; RTC
ISR 41, testHandler
ISR 42, testHandler
ISR 43, testHandler
ISR 44, testHandler     ; Mouse
ISR 45, testHandler     ; Coprocessor
ISR 46, disk1Handler    ; HDD1
ISR 47, disk2Handler    ; HDD2
ISR 48, testHandler     ; ETCInterrupt
