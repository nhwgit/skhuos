#ifndef __HANDLER_H__
#define __HANDLER_H__
#include "Type.h"
#include "arch/HandlerImp.h"

extern QWORD vectorStubTable[INTERRUPT_VECTOR_COUNT]; // Handler.asm이 생성하는 IDT용 스텁 주소 테이블

#endif
