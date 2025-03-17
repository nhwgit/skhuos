global switchContext

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
%endmacro

%macro LOADLEG 0
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

switchContext:
	push rbp
	mov rbp, rsp

	pushfq
	cmp rdi, 0
	je loadContext
	popfq

	push rax

	mov ax, ss
	mov qword[rdi+(8*23)], rax

	mov rax, rbp
	add rax, 16
	mov qword[rdi+(8*22)], rax

	pushfq
	pop rax
	mov qword[rdi+(8*21)], rax

	mov ax, cs
	mov qword[rdi+(8*20)], rax

	mov rax, qword[rbp+8]
	mov qword[rdi+(8*19)], rax


	pop rax
	pop rbp

	add rdi, 19*8
	mov rsp, rdi
	sub rdi, 19*8
	SAVEREG

loadContext:
	mov rsp, rsi
	LOADLEG
	iretq
