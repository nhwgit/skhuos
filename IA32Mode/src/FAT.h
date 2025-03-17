#ifndef __FAT_H__
#define __FAT_H__

#include "Type.h"

#define CLUSTER_SIZE	4096 // BYTE
#define SECTOR_SIZE		512 // BYTE
#define LINK_SIZE	4 // BYTE

#define SIGNATURE	0x19990125
#define BOOTLOADER_SIGNATURE 0x55AA

#define MAX_FILECOUNT	128
#define MAX_FILENAME	((CLUSTER_SIZE / MAX_FILECOUNT) - 8)

#pragma pack(push, 1)
typedef struct bootSector {
	BYTE bootCode[420];
	DWORD signature;
	DWORD freeClusterCount;
	DWORD clusterLinkSectorCount;
	DWORD clusterLinkSectorAddress;
	DWORD dataAddress;
	WORD bootLoaderSignature;
}BootSector;

typedef struct fatInformation {
	DWORD freeClusterCount;
	DWORD clusterLinkSectorCount;
	DWORD clusterLinkSectorAddress;
	DWORD dataAddress;
}FatInformation;

typedef struct fileInformation {
	char fileName[MAX_FILENAME];
	DWORD fileSize;
	DWORD clusterIdx;
}FileInformation;
#pragma pack(pop)

void initFAT(void);
bool mount(void);
bool format(void);
int findFreeClusterAndUpdate(void);
int findFreeDirectoreyEntryAndUpdate(void);
void setClusterLinkTable(int idx, int data);
void readCluster(int offset, const BYTE* buffer);
void writeCluster(int offset, BYTE* buffer);
void createFile(const char * fileName);
void deleteFile(const char * fileName);
void showDir(void);

#endif
