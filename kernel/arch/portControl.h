#ifndef __PORT_CONTROL_H__
#define __PORT_CONTROL_H__
#include "Type.h"

// arch·drivers 전용 포트 I/O — 상위 계층은 드라이버 API를 통해 접근 (Makefile layering-check로 강제)

BYTE getPort(WORD port);
void setPort(WORD port, BYTE data);
WORD getPortWord(WORD port);
void setPortWord(WORD port, WORD data);

#endif
