#include "FAT.h"
#include "Disk.h"
#include "Print.h"
#include "String.h"

static FatInformation fatInfor = {0,};

static const char * mountFailMessage1 = "read disk fail.";
static const char * mountFailMessage2 = "do formatting first.";
static const char * notMountedMessage = "do mounting disk first.";
static const char * notFoundFileMessage = "File was not found";
static const char * alreadyExistMessage = "file already exists";

static int bestIdx = 0; // 클러스터 인덱스
static int bestRootDirIdx = 0;
static bool isMounted = FALSE;
static BYTE buffer512[SECTOR_SIZE]= {0};
static BYTE buffer4096[CLUSTER_SIZE] = {0};

static void readCluster(int offset, BYTE* buffer);
static void writeCluster(int offset, BYTE* buffer);

void initFAT(void) {
	isMounted = FALSE;
}

bool mount(void) {
	memsetZero(buffer512, sizeof(buffer512));
	memsetZero(buffer4096, sizeof(buffer4096));

	if(!readSector(FS_PRIMARY, FS_MASTER, 1, 0, buffer512)) {
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
	isMounted = TRUE;
	return TRUE;
}

bool format(void) {
	DiskInformation * disk = getDiskInformation();
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

	if(!writeSector(FS_PRIMARY, FS_MASTER, 1, 0, buffer512))
		return FALSE;
	memsetZero(buffer512, sizeof(buffer512));
	for(int i=1; i<sectorCount; i++) {
		writeSector(FS_PRIMARY, FS_MASTER, 1, i, buffer512);
	}
	((DWORD*)buffer512)[0] = 0xFFFFFFFF; // 0번 클러스터(루트 디렉터리) 사용중 표시
	writeSector(FS_PRIMARY, FS_MASTER, 1, 1, buffer512);
	return TRUE;
}

static int findFreeClusterAndUpdate(void) {
	int loopCount = fatInfor.clusterLinkSectorCount;
	for(int i=0; i<loopCount; i++) {
		int offset = bestIdx / (SECTOR_SIZE/LINK_SIZE);
		readSector(FS_PRIMARY, FS_MASTER, 1, fatInfor.clusterLinkSectorAddress+offset, buffer512);
		for(int j=0; j<SECTOR_SIZE/4; j++) {
			if(((DWORD*)buffer512)[j]==0)
				return bestIdx++;
			bestIdx+=1;
		}
	}
	return -1;
}

static int findFreeDirectoryEntryAndUpdate(void) {
	readCluster(0, buffer4096);
	FileInformation * fileInfo = (FileInformation *)buffer4096;
	for(int i=0; i<MAX_FILECOUNT; i++) {
		if(fileInfo[bestRootDirIdx].clusterIdx == 0)
			return bestRootDirIdx;
		bestRootDirIdx = (bestRootDirIdx+1)%MAX_FILECOUNT;
	}
	return -1;
}
static void setClusterLinkTable(int idx, int data) {
	int offset = idx / (SECTOR_SIZE/LINK_SIZE);
	readSector(FS_PRIMARY, FS_MASTER, 1, fatInfor.clusterLinkSectorAddress+offset, buffer512);
	(((DWORD*)buffer512)[idx % (SECTOR_SIZE/LINK_SIZE)]) = data;
	writeSector(FS_PRIMARY, FS_MASTER, 1, fatInfor.clusterLinkSectorAddress+offset, buffer512);
}

static void readCluster(int offset, BYTE* buffer) {
	readSector(FS_PRIMARY, FS_MASTER, CLUSTER_SIZE/SECTOR_SIZE, fatInfor.dataAddress+(offset*(CLUSTER_SIZE/SECTOR_SIZE)), buffer);
}

static void writeCluster(int offset, BYTE* buffer) {
	writeSector(FS_PRIMARY, FS_MASTER, CLUSTER_SIZE/SECTOR_SIZE, fatInfor.dataAddress+(offset*(CLUSTER_SIZE/SECTOR_SIZE)), buffer);
}

void createFile(const char * fileName) {
	if(!isMounted) {
		puts(notMountedMessage);
		return ;
	}
	readCluster(0, buffer4096);
	FileInformation * dir = (FileInformation *)buffer4096;
	for(int i=0; i<MAX_FILECOUNT; i++) {
		if(dir[i].clusterIdx!=0 && strcmp(dir[i].fileName, fileName)==0) {
			puts(alreadyExistMessage);
			return;
		}
	}
	int dirEntry = findFreeDirectoryEntryAndUpdate();
	if(dirEntry == -1)
		return;
	int cluster = findFreeClusterAndUpdate();
	if(cluster == -1)
		return;
	setClusterLinkTable(cluster, 0xFFFFFFFF);

	FileInformation fileInfo = {0,};
	int nameLen = strlen(fileName);
	if(nameLen > MAX_FILENAME-1)
		nameLen = MAX_FILENAME-1;
	memcpy(fileInfo.fileName, fileName, nameLen);
	fileInfo.clusterIdx = cluster;
	fileInfo.fileSize = 0; // 빈 파일 생성

	readCluster(0, buffer4096);
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



int readFile(const char * fileName, BYTE * buffer) { // 성공하면 파일 크기 반환
	if(!isMounted) {
		puts(notMountedMessage);
		return -1;
	}
	readCluster(0, buffer4096);
	FileInformation * fileInfo = (FileInformation *)buffer4096;
	for(int i=0; i<MAX_FILECOUNT; i++) {
		if(fileInfo[i].clusterIdx!=0 && strcmp(fileInfo[i].fileName, fileName)==0) {
			readCluster(fileInfo[i].clusterIdx, buffer);
			return fileInfo[i].fileSize;
		}
	}
	puts(notFoundFileMessage);
	return -1;
}

bool writeFile(const char * fileName, const BYTE * buffer, int size) {
	if(!isMounted) {
		puts(notMountedMessage);
		return FALSE;
	}
	readCluster(0, buffer4096);
	FileInformation * fileInfo = (FileInformation *)buffer4096;
	for(int i=0; i<MAX_FILECOUNT; i++) {
		if(fileInfo[i].clusterIdx!=0 && strcmp(fileInfo[i].fileName, fileName)==0) {
			writeCluster(fileInfo[i].clusterIdx, (BYTE *)buffer);
			fileInfo[i].fileSize = size;
			writeCluster(0, buffer4096);
			return TRUE;
		}
	}
	puts(notFoundFileMessage);
	return FALSE;
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
