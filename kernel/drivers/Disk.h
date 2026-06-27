#ifndef __DISK_H__
#define __DISK_H__

#include "Type.h"

#define PRIMARY TRUE
#define SECONDARY FALSE
#define MASTER	TRUE
#define SLAVE	FALSE

// 파일시스템이 사용하는 디스크 (hda는 OS 이미지, hdb가 파일시스템)
#define FS_PRIMARY	PRIMARY
#define FS_MASTER	SLAVE

#define PRIMARY_DATA 0x1F0
#define PRIMARY_ERROR 0x1F1
#define PRIMARY_SECTOR_COUNT 0x1F2
#define PRIMARY_SECTOR_NUMBER 0x1F3
#define PRIMARY_CYLINDER_LSB 0x1F4
#define PRIMARY_CYLINDER_MSB 0x1F5
#define PRIMARY_DRIVE_AND_HEADER 0x1F6
#define PRIMARY_STATUS 0x1F7
#define PRIMARY_COMMAND 0x1F7
#define PRIMARY_DIGITAL_OUTPUT 0x3F6
#define PRIMARY_DRIVE_ADDRESS 0x3F7

#define SECONDARY_DATA 0x170
#define SECONDARY_ERROR 0x171
#define SECONDARY_SECTOR_COUNT 0x172
#define SECONDARY_SECTOR_NUMBER 0x173
#define SECONDARY_CYLINDER_LSB 0x174
#define SECONDARY_CYLINDER_MSB 0x175
#define SECONDARY_DRIVE_AND_HEADER 0x176
#define SECONDARY_STATUS 0x177
#define SECONDARY_COMMAND 0x177
#define SECONDARY_DIGITAL_OUTPUT 0x376
#define SECONDARY_DRIVE_ADDRESS 0x377

#define DATA 0
#define ERROR 1
#define SECTOR_COUNT 2
#define SECTOR_NUMBER 3
#define CYLINDER_LSB 4
#define CYLINDER_MSB 5
#define DRIVE_AND_HEADER 6
#define STATUS 7
#define COMMAND 8
#define DIGITAL_OUTPUT 9
#define DRIVE_ADDRESS 10

#define COMMAND_READ 0x20
#define COMMAND_WRITE 0x30
#define COMMAND_DRIVE_IDENTIFY 0xEC

#define STATUS_BSY 0x80
#define STATUS_DRDY 0x40
#define STATUS_DF 0x20
#define STATUS_DSC 0x10
#define STATUS_DRQ 0x08
#define STATUS_CORR 0x04
#define STATUS_IDX 0x02
#define STATUS_ERR 0x01

#define DRIVERHEAD_LBA 0xE0
#define DRIVERHEAD_SLAVE 0x10
#define DRIVERHEAD_MASTER 0x00

#define DIGITALOUTPUT_SRST	0x04
#define DIGITALOUTPUT_nIEN	0x02 // 0으로 설정해야 인터럽트 발생

typedef struct diskInformation {
	WORD config;
	WORD cylinderCount;
	WORD reserved1;
	WORD headCount;
	WORD unformattedBytesPerTrack;
	WORD unformattedBytesPerSector;
	WORD sectorPerCylinderCount;
	WORD interSectorGap;
	WORD bytesInPhaseLock;
	WORD vendorStatusWordCount;
	WORD serialNumber[10];
	WORD controllerType;
	WORD bufferSize;
	WORD ECCBytesCount;
	WORD firmwareRevision[4];
	WORD modelNumber[20];
	WORD reserved2[13];
	DWORD totalSector;
	WORD reserved3[196];
}DiskInformation;

void initDisk(void);
bool readInformation(bool primary, bool master);
int readSector(bool isPrimary, bool isMaster, char sectorCount, int LBA, char * buffer);
int writeSector(bool isPrimary, bool isMaster, char sectorCount, int LBA, char * buffer);
int getTotalSector(void);
DiskInformation * getDiskInformation(void);

#endif
