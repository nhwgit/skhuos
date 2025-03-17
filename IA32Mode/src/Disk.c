#include "Disk.h"
#include "Sync.h"
#include "portControl.h"
#include "Print.h"

static Mutex diskMutex = {0,};
static DiskInformation disk = {0,};
static bool isPrimaryInterruptOccur = FALSE;
static bool isSecondaryInterruptOccur = FALSE;
static WORD portAddress[2][11] = {{SECONDARY_DATA, SECONDARY_ERROR, SECONDARY_SECTOR_COUNT, SECONDARY_SECTOR_NUMBER, SECONDARY_SYNLINDER_LSB,
		   SECONDARY_SYNLINDER_MSB, SECONDARY_DRIVE_AND_HEADER, SECONDARY_STATUS, SECONDARY_COMMAND,
		   SECONDARY_DIGITAL_OUTPUT, SECONDARY_DRIVE_ADDRESS},
						{PRIMARY_DATA, PRIMARY_ERROR, PRIMARY_SECTOR_COUNT, PRIMARY_SECTOR_NUMBER, PRIMARY_SYNLINDER_LSB,
			PRIMARY_SYNLINDER_MSB, PRIMARY_DRIVE_AND_HEADER, PRIMARY_STATUS, PRIMARY_COMMAND,
			PRIMARY_DIGITAL_OUTPUT, PRIMARY_DRIVE_ADDRESS}};

void initDisk(void) {
	initMutex(&diskMutex);
	isPrimaryInterruptOccur = FALSE;
	isSecondaryInterruptOccur = FALSE;
	setPort(PRIMARY_DIGITAL_OUTPUT, 0);
	setPort(SECONDARY_DIGITAL_OUTPUT, 0);
	readInformation(PRIMARY, MASTER);
}

void setDiskInterruptFlag(bool isPrimary, bool isSecondary) {
	isPrimaryInterruptOccur = isPrimary;
	isSecondaryInterruptOccur = isSecondary;
}

bool waitInterrupt(bool isPrimary) {
	if(isPrimary) {
		for(int i=0; i<2100000000; i++) {
			if(isPrimaryInterruptOccur)
				return TRUE;
		}
	}
	else {
		for(int i=0; i<2100000000; i++) {
			if(isSecondaryInterruptOccur)
				return TRUE;
		}
	}
	return FALSE;
}

bool waitNoBusy(bool isPrimary) {
	for(int i=0; i<2100000000; i++) {
		if((getPort(portAddress[isPrimary][STATUS])&STATUS_BSY)!=STATUS_BSY)
			return TRUE;
	}
	return FALSE;
}

bool waitReady(bool isPrimary) {
	for(int i=0; i<2100000000; i++) {
		if((getPort(portAddress[isPrimary][STATUS])&STATUS_DRDY)==STATUS_DRDY)
			return TRUE;
	}
	return FALSE;
}

bool readInformation(bool isPrimary, bool isMaster) {
	acquireLock(&diskMutex);
	while(!waitNoBusy(isPrimary));
	BYTE flag = 0;
	if(isMaster)
		flag = DRIVERHEAD_LBA | DRIVERHEAD_MASTER;
	else
		flag = DRIVERHEAD_LBA | DRIVERHEAD_SLAVE;

	setPort(portAddress[isPrimary][DRIVE_AND_HEADER], flag);

	while(!waitReady(isPrimary));

	setDiskInterruptFlag(isPrimary, !isPrimary);
	setPort(portAddress[isPrimary][COMMAND], COMMAND_DRIVE_IDENTIFY);


	if(!waitInterrupt(isPrimary)) {
		if(getPort(portAddress[isPrimary][STATUS]&STATUS_ERR)==STATUS_ERR) {
			releaseLock(&diskMutex);
			return FALSE;
		}
	}

	DiskInformation * diskPointer = &disk;

	for(int sector=0; sector<256; sector++)
		((WORD *)diskPointer)[sector] = getPortWord(portAddress[isPrimary][DATA]);
	releaseLock(&diskMutex);
}

int readSector(bool isPrimary, bool isMaster, char sectorCount, int LBA, char * buffer) {
	acquireLock(&diskMutex);
	BYTE flag = 0;
	if(isMaster)
		flag = DRIVERHEAD_LBA | DRIVERHEAD_MASTER;
	else
		flag = DRIVERHEAD_LBA | DRIVERHEAD_SLAVE;

	setPort(portAddress[isPrimary][SECTOR_COUNT], (int)sectorCount);
	setPort(portAddress[isPrimary][SECTOR_NUMBER], LBA);
	setPort(portAddress[isPrimary][SYNLINDER_LSB], LBA >> 8);
	setPort(portAddress[isPrimary][SYNLINDER_MSB], LBA >> 16);
	setPort(portAddress[isPrimary][DRIVE_AND_HEADER], (flag & 0xF0) | (LBA >> 24)&0x0F);

	while(!waitReady(isPrimary));
	setDiskInterruptFlag(isPrimary, !isPrimary);
	setPort(portAddress[isPrimary][COMMAND], COMMAND_READ);

	for(int i=0; i<sectorCount; i++) {
		char status = 0;
		status = getPort(portAddress[isPrimary][STATUS]);
		if((status&STATUS_ERR)==STATUS_ERR) {
			releaseLock(&diskMutex);
			return i;
		}
		if((status&STATUS_DRQ)==STATUS_DRQ) {
			for(int j = 0; j<256; j++) {
				((WORD*)buffer)[i*256+j] = getPortWord(portAddress[isPrimary][DATA]);
			}
		}
	}
	releaseLock(&diskMutex);
	return sectorCount;
}

int writeSector(bool isPrimary, bool isMaster, char sectorCount, int LBA, char * buffer) {
	acquireLock(&diskMutex);
	BYTE flag = 0;
	if(isMaster)
		flag = DRIVERHEAD_LBA | DRIVERHEAD_MASTER;
	else
		flag = DRIVERHEAD_LBA | DRIVERHEAD_SLAVE;

	setPort(portAddress[isPrimary][SECTOR_COUNT], (int)sectorCount);
	setPort(portAddress[isPrimary][SECTOR_NUMBER], LBA);
	setPort(portAddress[isPrimary][SYNLINDER_LSB], LBA >> 8);
	setPort(portAddress[isPrimary][SYNLINDER_MSB], LBA >> 16);
	setPort(portAddress[isPrimary][DRIVE_AND_HEADER], (flag & 0xF0) | (LBA >> 24)&0x0F);

	while(!waitReady(isPrimary));
	setDiskInterruptFlag(isPrimary, !isPrimary);
	setPort(portAddress[isPrimary][COMMAND], COMMAND_WRITE);

	char status = 0;
	while(1) {
		status = getPort(portAddress[isPrimary][STATUS]);
		if((status&STATUS_ERR)==STATUS_ERR) {
			releaseLock(&diskMutex);
			return 0;
		}
		if((status&STATUS_DRQ)==STATUS_DRQ) {
			break;
		}
	}
	for(int i=0; i<sectorCount; i++) {
		setDiskInterruptFlag(isPrimary, !isPrimary);
		for(int j=0; j<256; j++) {
			setPortWord(portAddress[isPrimary][DATA], ((WORD *)buffer)[i*256+j]);
		}
		status = getPort(portAddress[isPrimary][STATUS]);
		if((status&STATUS_ERR)==STATUS_ERR) {
			releaseLock(&diskMutex);
			return 0;
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
