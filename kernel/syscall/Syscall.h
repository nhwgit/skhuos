#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "Type.h"

// RAX 기능 번호 — user/UserProgram.asm의 equ 값과 일치해야 함
#define SYSCALL_WRITEXY	1
#define SYSCALL_SLEEP	2
#define SYSCALL_GETPID	3
#define SYSCALL_EXIT	4

#define SYSCALL_FAIL	((QWORD)-1)

QWORD dispatchSyscall(QWORD number, QWORD arg1, QWORD arg2, QWORD arg3);

#endif
