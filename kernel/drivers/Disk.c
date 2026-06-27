#include "drivers/Disk.h"
#include "drivers/PIC.h"
#include "drivers/PIT.h"
#include "proc/Sync.h"
#include "arch/portControl.h"
#include "arch/HandlerImp.h"
#include "Print.h"

static Mutex diskMutex = {0,};
static DiskInformation disk = {0,};
// ISR과 waitInterrupt 스핀 루프가 공유 — volatile이 아니면 최적화 시 루프에서 재로드가 생략될 수 있음
static volatile bool isPrimaryInterruptOccur = FALSE;
static volatile bool isSecondaryInterruptOccur = FALSE;
#define WAIT_TIMEOUT_MS 500 // 상태 폴링 최대 대기 시간

// [0]=SECONDARY, [1]=PRIMARY (bool 인덱스와 일치)
static WORD portAddress[2][11] = {{SECONDARY_DATA, SECONDARY_ERROR, SECONDARY_SECTOR_COUNT, SECONDARY_SECTOR_NUMBER, SECONDARY_CYLINDER_LSB,
		   SECONDARY_CYLINDER_MSB, SECONDARY_DRIVE_AND_HEADER, SECONDARY_STATUS, SECONDARY_COMMAND,
		   SECONDARY_DIGITAL_OUTPUT, SECONDARY_DRIVE_ADDRESS},
						{PRIMARY_DATA, PRIMARY_ERROR, PRIMARY_SECTOR_COUNT, PRIMARY_SECTOR_NUMBER, PRIMARY_CYLINDER_LSB,
			PRIMARY_CYLINDER_MSB, PRIMARY_DRIVE_AND_HEADER, PRIMARY_STATUS, PRIMARY_COMMAND,
			PRIMARY_DIGITAL_OUTPUT, PRIMARY_DRIVE_ADDRESS}};

static void primaryDiskInterruptHandler(int vectorNumber) {
	isPrimaryInterruptOccur = TRUE;
	sendEOI(vectorNumber - IRQ_START_OFFSET);
}

static void secondaryDiskInterruptHandler(int vectorNumber) {
	isSecondaryInterruptOccur = TRUE;
	sendEOI(vectorNumber - IRQ_START_OFFSET);
}

void initDisk(void) {
	initMutex(&diskMutex);
	isPrimaryInterruptOccur = FALSE;
	isSecondaryInterruptOccur = FALSE;
	registerInterruptHandler(IRQ_START_OFFSET+IRQ_DISK1, primaryDiskInterruptHandler);
	registerInterruptHandler(IRQ_START_OFFSET+IRQ_DISK2, secondaryDiskInterruptHandler);
	setPort(PRIMARY_DIGITAL_OUTPUT, 0);
	setPort(SECONDARY_DIGITAL_OUTPUT, 0);
	if(!readInformation(FS_PRIMARY, FS_MASTER))
		puts("Disk identify fail!");
}

// 명령 발행 직전에 자기 채널 플래그만 초기화 (상대 채널의 대기 상태를 건드리면 안 됨)
static void clearDiskInterruptFlag(bool isPrimary) {
	if(isPrimary)
		isPrimaryInterruptOccur = FALSE;
	else
		isSecondaryInterruptOccur = FALSE;
}

static bool waitInterrupt(bool isPrimary) {
	int limit = msToClock(WAIT_TIMEOUT_MS);
	PITStopwatch watch;
	startStopwatch(&watch);
	do {
		if(isPrimary && isPrimaryInterruptOccur)
			return TRUE;
		if(!isPrimary && isSecondaryInterruptOccur)
			return TRUE;
	} while(getElapsedClock(&watch) < limit);
	return FALSE;
}

static bool waitStatus(bool isPrimary, BYTE mask, BYTE value) {
	int limit = msToClock(WAIT_TIMEOUT_MS);
	PITStopwatch watch;
	startStopwatch(&watch);
	do {
		if((getPort(portAddress[isPrimary][STATUS])&mask)==value)
			return TRUE;
	} while(getElapsedClock(&watch) < limit);
	return FALSE;
}

static bool waitNoBusy(bool isPrimary) {
	return waitStatus(isPrimary, STATUS_BSY, 0);
}

static bool waitReady(bool isPrimary) {
	return waitStatus(isPrimary, STATUS_DRDY, STATUS_DRDY);
}

// 데이터 전송 준비(DRQ) 대기. ERR나 타임아웃이면 FALSE
static bool waitDataRequest(bool isPrimary) {
	int limit = msToClock(WAIT_TIMEOUT_MS);
	PITStopwatch watch;
	startStopwatch(&watch);
	do {
		BYTE status = getPort(portAddress[isPrimary][STATUS]);
		if((status&STATUS_BSY)!=STATUS_BSY) { // BSY 중에는 다른 상태 비트가 유효하지 않음
			if((status&STATUS_ERR)==STATUS_ERR)
				return FALSE;
			if((status&STATUS_DRQ)==STATUS_DRQ)
				return TRUE;
		}
	} while(getElapsedClock(&watch) < limit);
	return FALSE;
}

static BYTE driveFlag(bool isMaster) {
	if(isMaster)
		return DRIVERHEAD_LBA | DRIVERHEAD_MASTER;
	return DRIVERHEAD_LBA | DRIVERHEAD_SLAVE;
}

static bool sendDiskCommand(bool isPrimary, bool isMaster, char sectorCount, int LBA, BYTE command) {
	if(!waitNoBusy(isPrimary)) // 이전 명령이 끝나기 전(BUSY)에 보낸 명령은 무시됨
		return FALSE;
	setPort(portAddress[isPrimary][SECTOR_COUNT], (int)sectorCount);
	setPort(portAddress[isPrimary][SECTOR_NUMBER], LBA);
	setPort(portAddress[isPrimary][CYLINDER_LSB], LBA >> 8);
	setPort(portAddress[isPrimary][CYLINDER_MSB], LBA >> 16);
	setPort(portAddress[isPrimary][DRIVE_AND_HEADER], (driveFlag(isMaster) & 0xF0) | (LBA >> 24)&0x0F);

	if(!waitReady(isPrimary))
		return FALSE;
	clearDiskInterruptFlag(isPrimary);
	setPort(portAddress[isPrimary][COMMAND], command);
	return TRUE;
}

bool readInformation(bool isPrimary, bool isMaster) {
	acquireLock(&diskMutex);
	if(!waitNoBusy(isPrimary)) {
		releaseLock(&diskMutex);
		return FALSE;
	}

	setPort(portAddress[isPrimary][DRIVE_AND_HEADER], driveFlag(isMaster));

	if(!waitReady(isPrimary)) {
		releaseLock(&diskMutex);
		return FALSE;
	}

	clearDiskInterruptFlag(isPrimary);
	setPort(portAddress[isPrimary][COMMAND], COMMAND_DRIVE_IDENTIFY);

	// 부팅 초기(인터럽트 비활성)에도 호출되므로 waitInterrupt 대신 DRQ 폴링
	if(!waitDataRequest(isPrimary)) {
		releaseLock(&diskMutex);
		return FALSE;
	}

	DiskInformation * diskPointer = &disk;

	for(int sector=0; sector<256; sector++)
		((WORD *)diskPointer)[sector] = getPortWord(portAddress[isPrimary][DATA]);
	releaseLock(&diskMutex);
	return TRUE;
}

int readSector(bool isPrimary, bool isMaster, char sectorCount, int LBA, char * buffer) {
	acquireLock(&diskMutex);
	if(!sendDiskCommand(isPrimary, isMaster, sectorCount, LBA, COMMAND_READ)) {
		releaseLock(&diskMutex);
		return 0;
	}

	for(int i=0; i<sectorCount; i++) {
		if(!waitInterrupt(isPrimary)) {
			releaseLock(&diskMutex);
			return i;
		}
		clearDiskInterruptFlag(isPrimary);
		// 상태 읽기가 INTRQ를 해제해야 다음 섹터 인터럽트의 엣지가 만들어짐
		if(getPort(portAddress[isPrimary][STATUS])&STATUS_ERR) {
			releaseLock(&diskMutex);
			return i;
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
	if(!sendDiskCommand(isPrimary, isMaster, sectorCount, LBA, COMMAND_WRITE)) {
		releaseLock(&diskMutex);
		return 0;
	}

	// 첫 블록은 인터럽트 없이 DRQ만 세워짐 (ATA PIO data-out 프로토콜)
	if(!waitDataRequest(isPrimary)) {
		releaseLock(&diskMutex);
		return 0;
	}

	for(int i=0; i<sectorCount; i++) {
		clearDiskInterruptFlag(isPrimary);
		for(int j=0; j<256; j++) {
			setPortWord(portAddress[isPrimary][DATA], ((WORD *)buffer)[i*256+j]);
		}
		// 섹터 플러시 완료(다음 블록 준비 또는 명령 종료) 인터럽트 대기
		if(!waitInterrupt(isPrimary)) {
			releaseLock(&diskMutex);
			return i;
		}
		if(getPort(portAddress[isPrimary][STATUS])&STATUS_ERR) {
			releaseLock(&diskMutex);
			return i;
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
