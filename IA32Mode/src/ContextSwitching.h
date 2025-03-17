#ifndef __CONTEXT_SWITCHING__H___
#define __CONTEXT_SWITCHING__H___
#include "Process.h"

void switchContext(ProcessContext * current, ProcessContext * next);
#endif
