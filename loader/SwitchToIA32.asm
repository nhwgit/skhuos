[BITS 32]

global SwitchIA32Mode
SECTION .text

SwitchIA32Mode:
	mov eax, cr4
	or eax, 0x20
	mov cr4, eax

	mov ecx, 0xC0000080
	rdmsr
	or eax, 0x0100
	wrmsr

	mov eax, [PML4BaseAddress]
	mov cr3, eax

	mov eax, cr0
	or eax, 0x80000000
	xor eax, 0x60000000
	mov cr0, eax

	jmp 0x10:0x600000



PML4BaseAddress: dd 0x100000
