#include "syscall/Syscall.h"
#include "Print.h"
#include "proc/Process.h"

// 유저 포인터 검증: 유저 영역 안에서 NUL로 끝나는 문자열인지 확인
// (커널 주소를 넘겨 커널 메모리를 읽거나 출력하게 만드는 것을 차단)
static bool isUserString(const char * str) {
	QWORD addr = (QWORD)str;
	if(addr < USER_MEMORY_BASE || addr >= USER_MEMORY_END)
		return FALSE;
	for(; addr < USER_MEMORY_END; addr++) {
		if(*(const char *)addr == '\0')
			return TRUE;
	}
	return FALSE;
}

static QWORD writeXY(QWORD line, QWORD column, const char * str) {
	const QWORD lineWidth = ONE_LINE_SIZE/ONE_CHAR_SIZE;
	if(line >= LINE_COUNT || column >= lineWidth || !isUserString(str))
		return SYSCALL_FAIL;
	for(int i=0; str[i]!='\0' && column+i < lineWidth; i++)
		viewCharacterXY(str[i], line, column+i);
	return 0;
}

QWORD dispatchSyscall(QWORD number, QWORD arg1, QWORD arg2, QWORD arg3) {
	switch(number) {
	case SYSCALL_WRITEXY:
		return writeXY(arg1, arg2, (const char *)arg3);
	case SYSCALL_SLEEP:
		sleepProcess(arg1);
		return 0;
	case SYSCALL_GETPID:
		return getRunningPid();
	case SYSCALL_EXIT:
		exitProcess(); // 복귀 없음
	}
	return SYSCALL_FAIL;
}
