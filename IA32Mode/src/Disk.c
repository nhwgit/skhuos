#include "Disk.h"
#include "Sync.h"
#include "portControl.h"
#include "Print.h"

static Mutex diskMutex = {0,};
static DiskInformation disk = {0,};
static bool isPrimaryInterruptOccur = FALSE;
static bool isSecondaryInterruptOccur = FALSE;
#define WAIT_LIMIT 2100000000 // 상태 폴링 최대 반복 횟수

// [0]=SECONDARY, [1]=PRIMARY (bool 인덱스와 일치)
static WORD portAddress[2][11] = {{SECONDARY_DATA, SECONDARY_ERROR, SECONDARY_SECTOR_COUNT, SECONDARY_SECTOR_NUMBER, SECONDARY_CYLINDER_LSB,
		   SECONDARY_CYLINDER_MSB, SECONDARY_DRIVE_AND_HEADER, SECONDARY_STATUS, SECONDARY_COMMAND,
		   SECONDARY_DIGITAL_OUTPUT, SECONDARY_DRIVE_ADDRESS},
						{PRIMARY_DATA, PRIMARY_ERROR, PRIMARY_SECTOR_COUNT, PRIMARY_SECTOR_NUMBER, PRIMARY_CYLINDER_LSB,
			PRIMARY_CYLINDER_MSB, PRIMARY_DRIVE_AND_HEADER, PRIMARY_STATUS, PRIMARY_COMMAND,
			PRIMARY_DIGITAL_OUTPUT, PRIMARY_DRIVE_ADDRESS}};

void initDisk(void) {
	initMutex(&diskMutex);
	isPrimaryInterruptOccur = FALSE;
	isSecondaryInterruptOccur = FALSE;
	setPort(PRIMARY_DIGITAL_OUTPUT, 0);
	setPort(SECONDARY_DIGITAL_OUTPUT, 0);
	readInformation(FS_PRIMARY, FS_MASTER);
}

void setDiskInterruptFlag(bool isPrimary, bool isSecondary) {
	isPrimaryInterruptOccur = isPrimary;
	isSecondaryInterruptOccur = isSecondary;
}

bool waitInterrupt(bool isPrimary) {
	if(isPrimary) {
		for(int i=0; i<WAIT_LIMIT; i++) {
			if(isPrimaryInterruptOccur)
				return TRUE;
		}
	}
	else {
		for(int i=0; i<WAIT_LIMIT; i++) {
			if(isSecondaryInterruptOccur)
				return TRUE;
		}
	}
	return FALSE;
}

bool waitNoBusy(bool isPrimary) {
	for(int i=0; i<WAIT_LIMIT; i++) {
		if((getPort(portAddress[isPrimary][STATUS])&STATUS_BSY)!=STATUS_BSY)
			return TRUE;
	}
	return FALSE;
}

bool waitReady(bool isPrimary) {
	for(int i=0; i<WAIT_LIMIT; i++) {
		if((getPort(portAddress[isPrimary][STATUS])&STATUS_DRDY)==STATUS_DRDY)
			return TRUE;
	}
	return FALSE;
}

static BYTE driveFlag(bool isMaster) {
	if(isMaster)
		return DRIVERHEAD_LBA | DRIVERHEAD_MASTER;
	return DRIVERHEAD_LBA | DRIVERHEAD_SLAVE;
}

static void sendDiskCommand(bool isPrimary, bool isMaster, char sectorCount, int LBA, BYTE command) {
	while(!waitNoBusy(isPrimary)); // 이전 명령이 끝나기 전(BUSY)에 보낸 명령은 무시됨
	setPort(portAddress[isPrimary][SECTOR_COUNT], (int)sectorCount);
	setPort(portAddress[isPrimary][SECTOR_NUMBER], LBA);
	setPort(portAddress[isPrimary][CYLINDER_LSB], LBA >> 8);
	setPort(portAddress[isPrimary][CYLINDER_MSB], LBA >> 16);
	setPort(portAddress[isPrimary][DRIVE_AND_HEADER], (driveFlag(isMaster) & 0xF0) | (LBA >> 24)&0x0F);

	while(!waitReady(isPrimary));
	setDiskInterruptFlag(isPrimary, !isPrimary);
	setPort(portAddress[isPrimary][COMMAND], command);
}

bool readInformation(bool isPrimary, bool isMaster) {
	acquireLock(&diskMutex);
	while(!waitNoBusy(isPrimary));

	setPort(portAddress[isPrimary][DRIVE_AND_HEADER], driveFlag(isMaster));

	while(!waitReady(isPrimary));

	setDiskInterruptFlag(isPrimary, !isPrimary);
	setPort(portAddress[isPrimary][COMMAND], COMMAND_DRIVE_IDENTIFY);


	if(!waitInterrupt(isPrimary)) {
		if((getPort(portAddress[isPrimary][STATUS])&STATUS_ERR)==STATUS_ERR) {
			releaseLock(&diskMutex);
			return FALSE;
		}
	}

	DiskInformation * diskPointer = &disk;

	for(int sector=0; sector<256; sector++)
		((WORD *)diskPointer)[sector] = getPortWord(portAddress[isPrimary][DATA]);
	releaseLock(&diskMutex);
	return TRUE;
}

int readSector(bool isPrimary, bool isMaster, char sectorCount, int LBA, char * buffer) {
	acquireLock(&diskMutex);
	sendDiskCommand(isPrimary, isMaster, sectorCount, LBA, COMMAND_READ);

	for(int i=0; i<sectorCount; i++) {
		char status = 0;
		while(1) { // 섹터 준비(DRQ) 대기
			status = getPort(portAddress[isPrimary][STATUS]);
			if((status&STATUS_ERR)==STATUS_ERR) {
				releaseLock(&diskMutex);
				return i;
			}
			if((status&STATUS_DRQ)==STATUS_DRQ)
				break;
		}
		for(int j = 0; j<256; j++) {
			((WORD*)buffer)[i*256+j] = getPortWord(portAddress[isPrimary][DATA]);
		}
	}
	releaseLock(&diskMutex);
	return sectorCount;
}

int writeSector(bool isPrimary, bool isMaster, char sectorCount, int LBA, char * buffer) {
	acquireLock(&diskMutex);
	sendDiskCommand(isPrimary, isMaster, sectorCount, LBA, COMMAND_WRITE);

	for(int i=0; i<sectorCount; i++) {
		char status = 0;
		while(1) { // 섹터마다 DRQ를 기다려야 함 (이전 섹터 플러시 중에 보낸 데이터는 유실됨)
			status = getPort(portAddress[isPrimary][STATUS]);
			if((status&STATUS_ERR)==STATUS_ERR) {
				releaseLock(&diskMutex);
				return i;
			}
			if((status&STATUS_DRQ)==STATUS_DRQ)
				break;
		}
		setDiskInterruptFlag(isPrimary, !isPrimary);
		for(int j=0; j<256; j++) {
			setPortWord(portAddress[isPrimary][DATA], ((WORD *)buffer)[i*256+j]);
		}
	}
	releaseLock(&diskMutex);
	return sectorCount;
}

int getTotalSector(void) {
	return disk.totalSector;
}

DiskInformation * getDiskInformation(void) {
	return &disk;
}
