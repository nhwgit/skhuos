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

void Main(void) {
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
