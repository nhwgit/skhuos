#include "Type.h"
#include "Print.h"
#include "arch/Descriptor.h"
#include "arch/CPU.h"
#include "mm/Memory.h"
#include "drivers/Keyboard.h"
#include "lib/Queue.h"
#include "proc/Process.h"
#include "drivers/Disk.h"
#include "drivers/PIC.h"
#include "drivers/PIT.h"
#include "fs/FAT.h"
#include "shell/Shell.h"

extern QWORD __bss_start, _end; // 링커 스크립트 심볼

// kernel.bin에는 .bss가 실리지 않는데, 로더의 페이지 테이블 초기화(0x106000~0x906000)가
// 커널 영역까지 PTE로 덮으므로 .bss는 부팅 직후 쓰레기 값 — 반드시 0으로 밀어야 함
static void clearBss(void) {
	for(QWORD * p = &__bss_start; p < &_end; p++)
		*p = 0;
}

void Main(void) {
	clearBss(); // 다른 어떤 초기화보다 먼저
	cursorLine(7); // 부팅 메시지 다음 줄부터 출력
	puts("Switch to IA-32e mode");
	puts("Success Booting");

	initializeDescriptor();
	puts("Initialize Descriptor");

	initializePIC();
	setIMR(0);
	puts("PIC initialize");
	initKeyboard();
	initMemoryBitmap();
	puts("Memory Pool Initialize");
	initScheduler();
	initializePIT(msToClock(SLOT_TIME), 1);
	initDisk();
	puts("Disk Initialize");
	initFAT();
	enableInterrupt(); // 각 init의 ISR 등록이 끝난 뒤 활성화 (등록 전 IRQ가 미등록 벡터로 빠지는 것 방지)
	startShell();
	while(1);
}
