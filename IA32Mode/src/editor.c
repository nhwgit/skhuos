#include "editor.h"
#include "FAT.h"
#include "Print.h"
#include "Keyboard.h"
#include "String.h"

static BYTE buffer1[4096] = {0};
static BYTE buffer2[4096] = {0};

void runEditor(const char * name) {
	readCluster(0, buffer1);
	FileInformation * fileInfo = (FileInformation *)buffer1;
	int clusterIdx = 0;
	for(clusterIdx=0; clusterIdx<MAX_FILECOUNT; clusterIdx++) {
		if(strcmp(fileInfo[clusterIdx].fileName, name)==0) {
			break;
		}
	}
	readCluster(fileInfo[clusterIdx].clusterIdx, buffer2);
	clear();

	int strIdx = strlen(buffer2);
	for(int i=0; i<MAX_LENGTH && i<strIdx; i++)
		viewCharacter(buffer2[i]);

	char data = 0;
	while(1) {
		if(getQueue(&data)) {
			if(data==ENTER) {
				int strIdxNew = (strIdx+80) - (strIdx-strIdx%80);
				while(strIdx <= strIdxNew) {
					buffer2[strIdx] = ' ';
				}
				continue;
			}
			else if(data==BACK_SPACE) {
				if(strIdx==0) continue;
				else buffer2[--strIdx] = '\0';
			}
			else if(data==ESC) {
				int stateIdx=0;
				char stateBuffer[80] = {0};
				while(1) {
					if(getQueue(&data)) {
						if(data==ESC) {
							for(int i=0; i<80; i++)
								viewCharacterXY('\0', 24, i);
							break;
						}
						else if(data==ENTER) {
							if(memcmp(stateBuffer, "w", 1)==0) {
								writeCluster(fileInfo[clusterIdx].clusterIdx, buffer2);
								fileInfo[clusterIdx].fileSize = strlen(buffer2);
								writeCluster(0, buffer1);
								return;
							}
							else if(memcmp(stateBuffer, "q", 1)==0)
								return;
						}
						else if(data==BACK_SPACE) {
							if(stateIdx==0) continue;
							else {
								stateBuffer[--stateIdx] = '\0';
								viewCharacterXY('\0', 24, stateIdx);
							}
						}
						else if(data!=0) {
							viewCharacterXY(data, 24, stateIdx);
							stateBuffer[stateIdx++] = data;
							stateBuffer[stateIdx] = '\0';
						}
					}
				}
			}
			else if(data!=0) {
				buffer2[strIdx++] = data;
				buffer2[strIdx] = '\0';
			}
			viewCharacter(data);
		}

	}
}
