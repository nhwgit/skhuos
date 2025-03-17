#ifndef __REG_CONTROL_H__
#define __REG_CONTROL_H__
#include "Type.h"

void loadGDTR(QWORD address);
void loadTR(WORD address);
void loadIDTR(QWORD address);
void enableInterrupt(void);
void disableInterrupt(void);
QWORD getRFLAGs(void);

#endif
