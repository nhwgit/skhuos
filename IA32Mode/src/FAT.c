#include "FAT.h"
#include "Disk.h"
#include "Print.h"
#include "String.h"

static FatInformation fatInfor = {0,};

static const char * mountFailMessage1 = "read disk fail.";
static const char * mountFailMessage2 = "do formatting first.";
static const char * notMountedMessage = "do mounting disk first.";
static const char * notFoundFileMessage = "File was not found";

static int bestIdx = 0; // 클러스터 인덱스
static int bestRootDirIdx = 0;
static bool isMounted = FALSE;
static BYTE buffer512[SECTOR_SIZE]= {0};
static BYTE buffer4096[CLUSTER_SIZE] = {0};

void initFAT(void) {
	isMounted = FALSE;
}

bool mount(void) {
	for(int i=0; i<SECTOR_SIZE; i++)
		buffer512[i] = 0;

	for(int i=0; i<CLUSTER_SIZE; i++)
		buffer4096[i] = 0;

	if(!readSector(TRUE, TRUE, 1, 0, buffer512)) {
		puts(mountFailMessage1);
		return FALSE;
	}
	BootSector * boot = (BootSector*)buffer512;
	if(boot->signature!=SIGNATURE) {
		puts(mountFailMessage2);
		return FALSE;
	}

	fatInfor.clusterLinkSectorAddress = boot->clusterLinkSectorAddress;
	fatInfor.clusterLinkSectorCount = boot->clusterLinkSectorCount;
	fatInfor.dataAddress = boot->dataAddress;
	fatInfor.freeClusterCount = boot->freeClusterCount;
	bestIdx = 0;
	bestRootDirIdx = 0;

	//0번 클러스터 링크 테이블 사용중인것으로 변경
	memsetZero(buffer512, sizeof(buffer512));
	((DWORD*)buffer512)[0] = 0xFFFFFFFF;
	writeSector(TRUE, TRUE, 1, 1, buffer512);
	isMounted = TRUE;
	return TRUE;
}

bool format(void) {
	BYTE buffer1[sizeof(DiskInformation)] = {0};
	DiskInformation * disk = (DiskInformation*)buffer1;
	disk = getDiskInformation();
	int sectorCount = disk->totalSector;
	int clusterCount = sectorCount / (CLUSTER_SIZE/SECTOR_SIZE);
	int clusterLinkSectorCount = (clusterCount+(SECTOR_SIZE/LINK_SIZE)-1)/(SECTOR_SIZE/LINK_SIZE);
	int dataSectorCount = sectorCount - 1 - clusterLinkSectorCount;
	clusterLinkSectorCount = (dataSectorCount+(SECTOR_SIZE/LINK_SIZE)-1)/(SECTOR_SIZE/LINK_SIZE);

	BootSector * boot = (BootSector*)buffer512;
	for(int i=0; i<sizeof(boot->bootCode); i++)
		boot->bootCode[i] = 0;
	boot->bootLoaderSignature = BOOTLOADER_SIGNATURE;
	boot->signature = SIGNATURE;
	boot->freeClusterCount = clusterCount;
	boot->clusterLinkSectorCount = clusterLinkSectorCount;
	boot->clusterLinkSectorAddress = 1; // MBR바로 뒤로 설정
	boot->dataAddress = 1+clusterLinkSectorCount; // FAT영역 바로 뒤로 설정

	if(!writeSector(TRUE, TRUE, 1, 0, buffer512))
		return FALSE;
	memsetZero(buffer512, sizeof(buffer512));
	for(int i=1; i<sectorCount; i++) {
		writeSector(TRUE, TRUE, 1, i, buffer512);
	}
	return TRUE;
}

int findFreeClusterAndUpdate(void) {
	int loopCount = fatInfor.clusterLinkSectorCount;
	for(int i=0; i<loopCount; i++) {
		int offset = bestIdx / (SECTOR_SIZE/LINK_SIZE);
		readSector(TRUE, TRUE, 1, fatInfor.clusterLinkSectorAddress+offset, buffer512);
		for(int j=0; j<SECTOR_SIZE/4; j++) {
			if(((DWORD*)buffer512)[j]==0)
				return bestIdx++;
			bestIdx+=1;
		}
	}
	return -1;
}

int findFreeDirectoreyEntryAndUpdate(void) {
	readCluster(0, buffer4096);
	FileInformation * fileInfo = (FileInformation *)buffer4096;
	for(int i=0; i<MAX_FILECOUNT; i++) {
		if(fileInfo[bestRootDirIdx].clusterIdx == 0)
			return bestRootDirIdx;
		bestRootDirIdx = (bestRootDirIdx+1)%MAX_FILECOUNT;
	}
	return -1;
}
void setClusterLinkTable(int idx, int data) {
	int offset = idx / (SECTOR_SIZE/LINK_SIZE);
	readSector(TRUE, TRUE, 1, fatInfor.clusterLinkSectorAddress+offset, buffer512);
	(((DWORD*)buffer512)[idx % (SECTOR_SIZE/LINK_SIZE)]) = data;
	writeSector(TRUE, TRUE, 1, fatInfor.clusterLinkSectorAddress+offset, buffer512);
}

void readCluster(int offset, const BYTE* buffer) {
	readSector(TRUE, TRUE, CLUSTER_SIZE/SECTOR_SIZE, fatInfor.dataAddress+(offset*CLUSTER_SIZE), buffer);
}

void writeCluster(int offset, BYTE* buffer) {
	writeSector(TRUE, TRUE, CLUSTER_SIZE/SECTOR_SIZE, fatInfor.dataAddress+(offset*CLUSTER_SIZE), buffer);
}

void createFile(const char * fileName) {
	if(!isMounted) {
		puts(notMountedMessage);
		return ;
	}
	FileInformation fileInfo = {0,};
	int cluster = findFreeClusterAndUpdate();
	if(cluster == -1)
		return;
	setClusterLinkTable(cluster, 0xFFFFFFFF);
	int dirEntry = findFreeDirectoreyEntryAndUpdate();
	if(dirEntry == -1)
		return;
	memcpy(fileInfo.fileName, fileName, MAX_FILENAME);
	fileInfo.clusterIdx = cluster;
	fileInfo.fileSize = 0; // 빈 파일 생성

	readCluster(0, buffer4096);
	FileInformation * tmp = (FileInformation*)buffer4096;
	((FileInformation*)buffer4096)[dirEntry] = fileInfo;
	writeCluster(0, buffer4096);
}

void deleteFile(const char * fileName) {
	if(!isMounted) {
		puts(notMountedMessage);
		return ;
	}
	readCluster(0, buffer4096);
	FileInformation * fileInfo = (FileInformation *)buffer4096;
	for(int i=0; i<MAX_FILECOUNT; i++) {
		if(strcmp(fileInfo[i].fileName, fileName)==0) {
			setClusterLinkTable(fileInfo[i].clusterIdx, 0x00000000);
			fileInfo[i].clusterIdx = 0;
			fileInfo[i].fileName[0] = 0xE5;
			writeCluster(0, buffer4096);
			return;
		}
	}
	puts(notFoundFileMessage);
}



void showDir(void) {
	if(!isMounted) {
		puts(notMountedMessage);
		return ;
	}
	readCluster(0, buffer4096);
	FileInformation * fileInfo = (FileInformation *)buffer4096;
	puts(".");
	for(int i=0; i<MAX_FILECOUNT; i++) {
		if(fileInfo[i].clusterIdx!=0) {
			printString(fileInfo[i].fileName);
			printString(" ");
			printInt(fileInfo[i].fileSize, 10);
			printString("Byte");
			puts("");
		}
	}
}
