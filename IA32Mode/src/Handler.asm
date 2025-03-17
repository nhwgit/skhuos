[BITS 64]
SEGMENT .text

extern testHandler, keyboardHandler, interruptHandler, timerHandler, disk1Handler, disk2Handler

global IV00, IV01, IV02, IV03, IV04, IV05, IV06, IV07, IV08, IV09
global IV10, IV11, IV12, IV13, IV14, IV15, IV16, IV17, IV18, IV19, IV20
global IV32, IV33, IV34, IV35, IV36, IV37, IV38, IV39
global IV40, IV41, IV42, IV43, IV44, IV45, IV46, IV47, IV48

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



%macro LOAGREG 0
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

IV00: ;divide error
    SAVEREG
    mov rdi, 0
    call testHandler;

    LOAGREG
    iretq

IV01: ; debug error
    SAVEREG

    mov rdi, 1
    call testHandler

    LOAGREG
    iretq

IV02: ; RNMI
    SAVEREG

    mov rdi, 2
    call testHandler

    LOAGREG
    iretq

IV03: ; BreakPoint
    SAVEREG

    mov rdi, 3
    call testHandler

    LOAGREG
    iretq

IV04: ; Overflow
    SAVEREG

    mov rdi, 4
    call testHandler

    LOAGREG
    iretq

IV05: ; BoundRangeExceeded
    SAVEREG

    mov rdi, 5
    call testHandler

    LOAGREG
    iretq

IV06: ; InvalidOpcode
    SAVEREG

    mov rdi, 6
    call testHandler

    LOAGREG
    iretq

IV07: ; DeviceNotAvailable
    SAVEREG


    mov rdi, 7
    call testHandler

    LOAGREG
    iretq

IV08: ; DoubleFault
    SAVEREG

    mov rdi, 8
    mov rsi, qword [ rbp + 8 ]
    call testHandler

    LOAGREG
    add rsp, 8
    iretq

IV09: ; CoprocessorSegmentOverrun
    SAVEREG

    mov rdi, 9
    call testHandler

    LOAGREG
    iretq

IV10: ; InvalidTSS
    SAVEREG

    mov rdi, 10
    mov rsi, qword [ rbp + 8 ]
    call testHandler

    LOAGREG
    add rsp, 8
    iretq

IV11: ; SegmentNotPresent
    SAVEREG

    mov rdi, 11
    mov rsi, qword [ rbp + 8 ]
    call testHandler

    LOAGREG
    add rsp, 8
    iretq

IV12: ; StackSeg
    SAVEREG

    mov rdi, 12
    mov rsi, qword [ rbp + 8 ]
    call testHandler

    LOAGREG
    add rsp, 8
    iretq

IV13: ; GeneralProtection
	jmp $
    SAVEREG

    mov rdi, 13
    mov rsi, qword [ rbp + 8 ]
    call testHandler

    LOAGREG
    add rsp, 8
    iretq

IV14: ; PageFault
    SAVEREG

    mov rdi, 14
    mov rsi, qword [ rbp + 8 ]
    call testHandler

    LOAGREG
    add rsp, 8
    iretq

IV15:
    SAVEREG

    mov rdi, 15
    call testHandler

    LOAGREG
    iretq

IV16: ; FPUError
    SAVEREG

    mov rdi, 16
    call testHandler

    LOAGREG
    iretq

IV17: ; AlignmentCheck
    SAVEREG

    mov rdi, 17
    mov rsi, qword [ rbp + 8 ]
    call testHandler

    LOAGREG
    add rsp, 8
    iretq

IV18: ; MachineCheck
    SAVEREG

    mov rdi, 18
    call testHandler

    LOAGREG
    iretq

IV19: ; SIMDError
    SAVEREG

    mov rdi, 19
    call testHandler

    LOAGREG
    iretq

IV20: ; RETCException
    SAVEREG

    mov rdi, 20
    call testHandler

    LOAGREG
	iretq

;   인터럽트 핸들러

IV32: ; Timer
    SAVEREG

    mov rdi, 32
    call timerHandler

    LOAGREG
    iretq

IV33: ; Keyboard
    SAVEREG
    mov rdi, 33
    call keyboardHandler

    LOAGREG
    iretq

IV34: ; SlavePIC
    SAVEREG

    mov rdi, 34
    call testHandler

    LOAGREG
    iretq

IV35: ; Serial2
    SAVEREG

    mov rdi, 35
    call testHandler

    LOAGREG
    iretq

IV36: ; Serial1
    SAVEREG

    mov rdi, 36
    call testHandler

    LOAGREG
    iretq

IV37: ; Paraller2
    SAVEREG

    mov rdi, 37
    call testHandler

    LOAGREG
    iretq

IV38: ; Floppy
    SAVEREG

    mov rdi, 38
    call testHandler

    LOAGREG
    iretq

IV39: ; Parallel1
    SAVEREG

    mov rdi, 39
    call testHandler

    LOAGREG
    iretq

IV40: ; RTC
    SAVEREG

    mov rdi, 40
    call testHandler

    LOAGREG
    iretq

IV41:
    SAVEREG

    mov rdi, 41
    call testHandler

    LOAGREG
    iretq

IV42:
    SAVEREG

    mov rdi, 42
    call testHandler

    LOAGREG
    iretq

IV43:
    SAVEREG

    mov rdi, 43
    call testHandler

    LOAGREG
    iretq

IV44: ; Mouse
    SAVEREG

    mov rdi, 44
    call testHandler

    LOAGREG
    iretq

IV45: ; Coprocessor
    SAVEREG

    mov rdi, 45
    call testHandler

    LOAGREG
    iretq

IV46: ; HDD1
    SAVEREG

    mov rdi, 46
    call disk1Handler

    LOAGREG
    iretq

IV47: ; HDD2
    SAVEREG

    mov rdi, 47
    call disk2Handler

    LOAGREG
    iretq

IV48: ; ETCInterrupt
    SAVEREG

    mov rdi, 48
    call testHandler

    LOAGREG
    iretq
