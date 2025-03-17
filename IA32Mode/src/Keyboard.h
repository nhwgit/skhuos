#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "Type.h"

#define STATE_REG 0x64
#define BUFFER 0x60

#define ACK 0xFA

#define LED_STATE_COMMAND 0xED

#define QUEUE_SIZE	30

//Change LED
#define NUMLOCK 0x00
#define CAPSLOCK 0x01
#define SCROLLLOCK 0x02

//key
#define KEY_NONE        0x00
#define KEY_ENTER       '\n'
#define KEY_TAB         '\t'
#define KEY_ESC         0x1B
#define KEY_BACKSPACE   0x08

#define KEY_SHIFT_DOWN	0x2A
#define KEY_SHIFT_UP	0xAA

#define KEY_F1          0x61
#define KEY_F2          0x62
#define KEY_F3          0x63
#define KEY_F4          0x64
#define KEY_F5          0x65
#define KEY_F6          0x66
#define KEY_F7          0x67
#define KEY_F8          0x68
#define KEY_F9          0x69
#define KEY_F10         0x6A
#define KEY_F11         0x6B
#define KEY_F12         0x6C
#define KEY_CTRL        0x6D
#define KEY_ALT         0x6E
#define KEY_LSHIFT      0x6F
#define KEY_RSHIFT      0x70
#define KEY_PRINTSCREEN 0x71
#define KEY_CAPSLOCK    0x72
#define KEY_NUMLOCK     0x73
#define KEY_SCROLLLOCK  0x74
#define KEY_INS         0x75
#define KEY_HOME        0x76
#define KEY_DEL         0x77
#define KEY_END         0x78
#define KEY_PAGEUP      0x79
#define KEY_PAGEDOWN    0x7A
#define KEY_UP          0x7B
#define KEY_LEFT        0x7C
#define KEY_CENTER      0x7D
#define KEY_RIGHT       0x7E
#define KEY_DOWN        0x7F
#define KEY_PAUSE       0x80

#define QUEUE_COUNT		50

typedef struct key {
	BYTE code;
	BYTE bombinedCode;
} KEY;
int isOutputBufferFull(void);
int isInputBufferFull(void);
//bool changeLED(bool capsLock, bool numLock, bool scrollLock);
int getScanCode(void);
BYTE scanToASCII(BYTE scanCode);
void initKeyboard(void);
void inputQueue(BYTE scanCode);
bool getQueue(char * data);

#endif
