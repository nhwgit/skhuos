#include "Keyboard.h"
#include "portControl.h"
#include "Print.h"
#include "Queue.h"
#include "Sync.h"

static BYTE keyMappingTable[89][2] =
{
	{KEY_NONE, KEY_NONE},
    {KEY_ESC, KEY_ESC},
    {'1', '!'},
    {'2', '@'},
    {'3', '#'},
	{'4', '$'},
    {'5', '%'},
    {'6', '^'},
    {'7', '&'},
    {'8', '*'},
    {'9', '('},
    {'0', ')'},
	{'-', '_'},
    {'=', '+'},
    {KEY_BACKSPACE, KEY_BACKSPACE},
    {KEY_TAB, KEY_TAB},
    {'q', 'Q'},
    {'w', 'W'},
    {'e', 'E'},
    {'r', 'R'},
    {'t', 'T'},
    {'y', 'Y'},
    {'u', 'U'},
    {'i', 'I'},
	{'o', 'O'},
    {'p', 'P'},
    {'[', '{'},
    {']', '}'},
    {'\n', '\n'},
    {KEY_CTRL, KEY_CTRL},
    {'a', 'A'},
    {'s', 'S'},
    {'d', 'D'},
    {'f', 'F'},
    {'g', 'G'},
    {'h', 'H'},
    {'j', 'J'},
    {'k', 'K'},
    {'l', 'L'},
    {';', ':'},
    {'\'', '\"'},
    {'`', '~'},
    {KEY_LSHIFT, KEY_LSHIFT},
    {'\\', '|'},
    {'z', 'Z'},
    {'x', 'X'},
    {'c', 'C'},
    {'v', 'V'},
    {'b', 'B'},
    {'n', 'N'},
    {'m', 'M'},
    {',', '<'},
    {'.', '>'},
    {'/', '?'},
    {KEY_RSHIFT, KEY_RSHIFT},
    {'*', '*'},
    {KEY_ALT, KEY_ALT},
    {' ', ' '},
    {KEY_CAPSLOCK, KEY_CAPSLOCK},
    {KEY_F1, KEY_F1},
    {KEY_F2, KEY_F2},
    {KEY_F3, KEY_F3},
    {KEY_F4, KEY_F4},
    {KEY_F5, KEY_F5},
    {KEY_F6, KEY_F6},
    {KEY_F7, KEY_F7},
    {KEY_F8, KEY_F8},
    {KEY_F9, KEY_F9},
    {KEY_F10, KEY_F10},
    {KEY_NUMLOCK, KEY_NUMLOCK},
    {KEY_SCROLLLOCK, KEY_SCROLLLOCK},

    {KEY_HOME, '7'},
    {KEY_UP, '8'},
    {KEY_PAGEUP, '9'},
    {'-', '-' },
    {KEY_LEFT, '4'},
    {KEY_CENTER, '5'},
    {KEY_RIGHT, '6'},
    {'+', '+'},
    {KEY_END, '1'},
    {KEY_DOWN, '2'},
    {KEY_PAGEDOWN, '3'},
    {KEY_INS, '0'},
    {KEY_DEL, '.'},
    {KEY_NONE, KEY_NONE},
    {KEY_NONE, KEY_NONE},
    {KEY_NONE, KEY_NONE},
    {KEY_F11, KEY_F11},
    {KEY_F12, KEY_F12}
};

static bool shiftOn;

static Queue keyQueue = {0};
static char queueBuffer[100];
static Mutex keyQueueMutex = {0};

void initKeyboard(void) {
	initQueue(&keyQueue, queueBuffer, QUEUE_COUNT, sizeof(char));
	initMutex(&keyQueueMutex);
	shiftOn = FALSE;
}

int isOutputBufferFull(void) {
	if(getPort(STATE_REG) & 0x01)
		return TRUE;
	return FALSE;
}

int isInputBufferFull(void) {
	if(getPort(STATE_REG) & 0x02)
		return TRUE;
	return FALSE;
}

int getScanCode(void) {
	while(!isOutputBufferFull());
	return getPort(BUFFER);
}

void inputQueue(BYTE scanCode) {
	BYTE ascii = scanToASCII(scanCode);
	if(ascii!=0) {
		bool preIf = setIf(FALSE);
		//acquireLock(&keyQueueMutex);
		enQueue(&keyQueue, &ascii);
		setIf(preIf);
		//releaseLock(&keyQueueMutex);
	}
}

bool getQueue(char * data) {
	bool preIf = setIf(FALSE);
	//acquireLock(&keyQueueMutex);
	bool success = deQueue(&keyQueue, data);
	setIf(preIf);
	//releaseLock(&keyQueueMutex);
	return success;
}

BYTE scanToASCII(BYTE scanCode) {
	if(scanCode == KEY_SHIFT_DOWN) {
		shiftOn = TRUE;
		return 0;
	}
	else if(scanCode == KEY_SHIFT_UP)
		shiftOn = FALSE;

	if(scanCode == 0xE1) {
		getScanCode();
		getScanCode();
		return KEY_PAUSE;
	}
	else if(scanCode == 0xE0) {
		getScanCode();
		return 0;
	}

	else if(scanCode >=0x80)
		return 0;
	return keyMappingTable[scanCode][shiftOn];
}



