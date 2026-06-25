[bits 64]

SEGMENT .text
global getPort, setPort, getPortWord, setPortWord

getPort:
	push rdx

	mov rdx, rdi
	mov rax, 0
	in al, dx

	pop rdx
	ret

setPort:
	push rdx
	push rax

	mov rdx, rdi
	mov rax, rsi
	out dx, al

	pop rax
	pop rdx
	ret

getPortWord:
	push rdx
	mov rdx, rdi
	mov rax, 0
	in ax, dx

	pop rdx
	ret

setPortWord:
	push rdx
	push rax

	mov rdx, rdi
	mov rax, rsi
	out dx, ax

	pop rax
	pop rdx
	ret
