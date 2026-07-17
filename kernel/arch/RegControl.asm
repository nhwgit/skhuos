[bits 64]

SEGMENT .text
global loadGDTR, loadTR, loadIDTR, enableInterrupt, disableInterrupt, getRFLAGs, getCR2, halt, flushTLB

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

flushTLB: ; CR3 재적재로 TLB 전체 무효화 (페이지 권한 변경 후 필수)
	mov rax, cr3
	mov cr3, rax
	ret
