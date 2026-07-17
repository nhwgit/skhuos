#ifndef __CPU_H__
#define __CPU_H__
#include "Type.h"

// arch가 상위 계층에 공개하는 CPU 제어 API (구현: RegControl.asm)
// 포트 I/O(portControl.h)·특권 레지스터(RegControl.h)는 arch·드라이버 내부 전용

void enableInterrupt(void);
void disableInterrupt(void);
QWORD getRFLAGs(void);
void halt(void);
void flushTLB(void); // 페이지 테이블 권한 변경 후 호출

#endif
