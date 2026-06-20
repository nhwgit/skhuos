[BITS 64]
SEGMENT .text

extern testHandler, keyboardHandler, interruptHandler, timerHandler, disk1Handler, disk2Handler

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

; 에러 코드가 있는 벡터 (벡터 번호, 핸들러 함수)
%macro ISR_ERRCODE 2
global IV%1
IV%1:
    SAVEREG
    mov rdi, %1
    mov rsi, qword [ rbp + 8 ]
    call %2
    LOADREG
    add rsp, 8
    iretq
%endmacro

; 예외 핸들러
ISR         00, testHandler ; divide error
ISR         01, testHandler ; debug error
ISR         02, testHandler ; RNMI
ISR         03, testHandler ; BreakPoint
ISR         04, testHandler ; Overflow
ISR         05, testHandler ; BoundRangeExceeded
ISR         06, testHandler ; InvalidOpcode
ISR         07, testHandler ; DeviceNotAvailable
ISR_ERRCODE 08, testHandler ; DoubleFault
ISR         09, testHandler ; CoprocessorSegmentOverrun
ISR_ERRCODE 10, testHandler ; InvalidTSS
ISR_ERRCODE 11, testHandler ; SegmentNotPresent
ISR_ERRCODE 12, testHandler ; StackSeg
ISR_ERRCODE 13, testHandler ; GeneralProtection
ISR_ERRCODE 14, testHandler ; PageFault
ISR         15, testHandler
ISR         16, testHandler ; FPUError
ISR_ERRCODE 17, testHandler ; AlignmentCheck
ISR         18, testHandler ; MachineCheck
ISR         19, testHandler ; SIMDError
ISR         20, testHandler ; RETCException

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
