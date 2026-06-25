#ifndef __CONTEXT_SWITCHING__H___
#define __CONTEXT_SWITCHING__H___
#include "proc/Process.h"

void switchContext(ProcessContext * current, ProcessContext * next);
#endif
