#ifndef __REG_CONTROL_H__
#define __REG_CONTROL_H__
#include "Type.h"

// arch 내부 전용 특권 레지스터 접근 — 상위 계층은 arch/CPU.h 사용 (Makefile layering-check로 강제)

void loadGDTR(QWORD address);
void loadTR(WORD address);
void loadIDTR(QWORD address);
QWORD getCR2(void);

#endif
