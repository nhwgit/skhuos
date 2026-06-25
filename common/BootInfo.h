#ifndef __BOOTINFO_H___
#define __BOOTINFO_H___

// 부트섹터 안의 부팅 정보 블록. boot/BootLoader.asm의 BootInfo 블록과 레이아웃이 일치해야 한다.
// ImageMaker가 부트섹터에서 시그니처를 찾아 섹터 수를 기록하고,
// loader가 0x7C00에 적재된 부트섹터를 스캔해 읽는다 — 양쪽 모두 고정 오프셋에 의존하지 않는다.
#define BOOT_INFO_SIGNATURE        "SKHUBOOT"
#define BOOT_INFO_SIGNATURE_LENGTH 8
#define BOOT_SECTOR_LOADED_ADDRESS 0x7C00
#define BOOT_SECTOR_SIZE           512

#pragma pack(push, 1)
typedef struct bootInfo {
	char signature[BOOT_INFO_SIGNATURE_LENGTH];
	unsigned short totalSectorCount;  // 부트섹터 제외 (loader + kernel)
	unsigned short loaderSectorCount;
} BootInfo;
#pragma pack(pop)

#endif
