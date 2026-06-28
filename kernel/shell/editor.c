#include "shell/editor.h"
#include "fs/FAT.h"
#include "Print.h"
#include "drivers/Keyboard.h"
#include "String.h"

static BYTE buffer[4096] = {0};

void runEditor(const char * name) {
	if(readFile(name, buffer)==-1)
		return;
	clear();

	int strIdx = strlen(buffer);
	for(int i=0; i<MAX_LENGTH && i<strIdx; i++)
		viewCharacter(buffer[i]);

	char data = 0;
	while(1) {
		data = getKey();
		if(data==KEY_ENTER) {
			int strIdxNew = strIdx - strIdx%80 + 80; // 다음 80배수까지 공백 채움
			if(strIdxNew > sizeof(buffer)-1)
				continue;
			while(strIdx < strIdxNew)
				buffer[strIdx++] = ' ';
			buffer[strIdx] = '\0';
			moveToNextLine();
			continue;
		}
		else if(data==KEY_BACKSPACE) {
			if(strIdx==0) continue;
			else buffer[--strIdx] = '\0';
		}
		else if(data==KEY_ESC) {
			int stateIdx=0;
			char stateBuffer[80] = {0};
			while(1) {
				data = getKey();
				if(data==KEY_ESC) {
					for(int i=0; i<80; i++)
						viewCharacterXY('\0', 24, i);
					break;
				}
				else if(data==KEY_ENTER) {
					if(memcmp(stateBuffer, "w", 1)==0) {
						writeFile(name, buffer, strlen(buffer));
						return;
					}
					else if(memcmp(stateBuffer, "q", 1)==0)
						return;
				}
				else if(data==KEY_BACKSPACE) {
					if(stateIdx==0) continue;
					else {
						stateBuffer[--stateIdx] = '\0';
						viewCharacterXY('\0', 24, stateIdx);
					}
				}
				else if(data!=0 && stateIdx < sizeof(stateBuffer)-1) {
					viewCharacterXY(data, 24, stateIdx);
					stateBuffer[stateIdx++] = data;
					stateBuffer[stateIdx] = '\0';
				}
			}
		}
		else if(data!=0) {
			if(strIdx >= sizeof(buffer)-1)
				continue;
			buffer[strIdx++] = data;
			buffer[strIdx] = '\0';
		}
		viewCharacter(data);
	}
}
