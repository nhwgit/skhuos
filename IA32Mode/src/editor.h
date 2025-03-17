#ifndef __EDITOR_H__
#define __EDITOR_H__
#include "Type.h"

#define MAX_LENGTH 24*80
#define ENTER		0x0A
#define BACK_SPACE	0x08
#define ESC			0x1B

void runEditor(const char * name);
#endif
