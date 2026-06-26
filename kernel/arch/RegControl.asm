[bits 64]

SEGMENT .text
global loadGDTR, loadTR, loadIDTR, enableInterrupt, disableInterrupt, getRFLAGs, getCR2, halt

loadGDTR:
	lgdt [rdi]
	ret

loadTR:
	ltr di
	ret

loadIDTR:
	lidt [rdi]
	ret

enableInterrupt:
	sti
	ret

disableInterrupt:
	cli
	ret

getRFLAGs:
	pushfq
	pop rax
	ret

getCR2:
	mov rax, cr2
	ret

halt:
	hlt
	ret
