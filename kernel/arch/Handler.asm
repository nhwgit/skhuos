[BITS 64]
SEGMENT .text

extern dispatchInterrupt, exceptionHandler, dispatchSyscall

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

; 외부 인터럽트 스텁 (벡터 번호) — C 디스패처가 등록된 핸들러로 분기
%macro ISR 1
IV%1:
    SAVEREG
    mov rdi, %1
    call dispatchInterrupt
    LOADREG
    iretq
%endmacro

SAVEREG_SIZE equ 19*8 ; SAVEREG가 쌓는 크기 (GPR 15개 + 세그먼트 4개) — CPU가 쌓은 프레임 접근 오프셋

; 에러 코드가 없는 예외 (벡터 번호) — exceptionHandler가 halt 또는 프로세스 종료, 복귀 없음
%macro EXC 1
IV%1:
    SAVEREG
    mov rdi, %1
    xor rsi, rsi
    mov rdx, qword [rsp + SAVEREG_SIZE]     ; RIP
    mov rcx, qword [rsp + SAVEREG_SIZE + 8] ; CS — 링3에서 온 예외 판별용
    call exceptionHandler
%endmacro

; 에러 코드가 있는 예외 (벡터 번호)
%macro EXC_ERRCODE 1
IV%1:
    SAVEREG
    mov rdi, %1
    mov rsi, qword [rsp + SAVEREG_SIZE]      ; 에러 코드
    mov rdx, qword [rsp + SAVEREG_SIZE + 8]  ; RIP
    mov rcx, qword [rsp + SAVEREG_SIZE + 16] ; CS
    call exceptionHandler
%endmacro

; 예외 스텁 (0~20)
EXC         0  ; divide error
EXC         1  ; debug error
EXC         2  ; NMI
EXC         3  ; BreakPoint
EXC         4  ; Overflow
EXC         5  ; BoundRangeExceeded
EXC         6  ; InvalidOpcode
EXC         7  ; DeviceNotAvailable
EXC_ERRCODE 8  ; DoubleFault
EXC         9  ; CoprocessorSegmentOverrun
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
EXC         20 ; 21 이상 예약 벡터도 IV20으로 수렴

; 외부 인터럽트 스텁 (32~47): 벡터별 처리는 registerInterruptHandler 등록으로 결정
%assign vec 32
%rep 16
ISR %[vec]
%assign vec vec+1
%endrep

; 시스템 콜 게이트(0x81, DPL3) 진입점 — IST가 아닌 TSS RSP0(프로세스별 커널 스택)에서 실행
; RAX=기능 번호, RDI/RSI/RDX=인자, 반환값은 RAX (번호·인자 위치는 SAVEREG가 쌓는 순서 기준)
global syscallEntry
syscallEntry:
    SAVEREG
    mov rdi, qword [rsp + 4*8] ; RAX = 기능 번호
    mov rsi, qword [rsp + 9*8] ; RDI = 인자1
    mov rdx, qword [rsp + 8*8] ; RSI = 인자2
    mov rcx, qword [rsp + 7*8] ; RDX = 인자3
    call dispatchSyscall
    mov qword [rsp + 4*8], rax ; 반환값을 복원될 RAX 자리에 기록
    LOADREG
    iretq

; IDT 등록용 스텁 주소 테이블 — 크기는 C의 INTERRUPT_VECTOR_COUNT(arch/HandlerImp.h)와 일치해야 함
SEGMENT .data
global vectorStubTable
vectorStubTable:
%assign vec 0
%rep 48
  %if vec > 20 && vec < 32
    dq IV20 ; 예약 벡터
  %else
    dq IV%[vec]
  %endif
%assign vec vec+1
%endrep
