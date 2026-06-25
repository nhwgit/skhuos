#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// -Icommon을 쓰면 대소문자 무시 FS에서 <string.h>가 common/String.h로 잘못 해석되므로 상대 경로로 include
#include "../../common/BootInfo.h"

#define SECTOR_SIZE 512

int isSameFile(const char * a, const char * b);
int appendFile(FILE * image, const char * fileName);
void writeBootInfo(const char * imageName, int totalSectorCount, int loaderSectorCount);

int main(int argc, char * argv[]) {
	if(argc < 5) {
		fprintf(stderr, "usage: imagemaker boot.bin loader.bin kernel.bin Disk.img\n");
		return -1;
	}

	// 출력 이미지가 입력에 섞이면 쓰는 동시에 읽히면서 EOF에 도달하지 못하고 무한 증식함
	for(int i=1; i<4; i++) {
		if(isSameFile(argv[i], argv[4])) {
			fprintf(stderr, "%s: output image must not be used as an input\n", argv[i]);
			return -1;
		}
	}

	FILE * image = fopen(argv[4], "wb");
	if(image == NULL) {
		fprintf(stderr, "%s open fail\n", argv[4]);
		return -1;
	}

	appendFile(image, argv[1]);
	int loaderSectorCount = appendFile(image, argv[2]);
	int kernelSectorCount = appendFile(image, argv[3]);
	fclose(image);

	writeBootInfo(argv[4], loaderSectorCount + kernelSectorCount, loaderSectorCount);
	printf("%s create complete\n", argv[4]);
	return 0;
}

int isSameFile(const char * a, const char * b) { // 경로 표기가 달라도 같은 파일이면 참 (출력이 아직 없으면 경로 문자열로 비교)
	struct stat statA, statB;
	if(stat(a, &statA) != 0 || stat(b, &statB) != 0)
		return strcmp(a, b) == 0;
	return statA.st_dev == statB.st_dev && statA.st_ino == statB.st_ino;
}

int appendFile(FILE * image, const char * fileName) { // 512바이트 배수로 패딩 후 섹터 수 반환
	FILE * src = fopen(fileName, "rb");
	if(src == NULL) {
		fprintf(stderr, "%s open fail\n", fileName);
		exit(-1);
	}

	char buffer[SECTOR_SIZE];
	long size = 0;
	while(1) {
		int readCount = fread(buffer, 1, sizeof(buffer), src);
		if(readCount <= 0)
			break;
		if(fwrite(buffer, 1, readCount, image) != readCount) {
			fprintf(stderr, "%s write fail\n", fileName);
			exit(-1);
		}
		size += readCount;
	}
	fclose(src);

	int padding = (SECTOR_SIZE - (size % SECTOR_SIZE)) % SECTOR_SIZE;
	for(int i=0; i<padding; i++)
		fputc(0x00, image);

	int sectorCount = (size + padding) / SECTOR_SIZE;
	printf("%s: %ld byte, %d sector\n", fileName, size, sectorCount);
	return sectorCount;
}

// 부트섹터에서 BootInfo 시그니처를 찾아 그 자리에 섹터 수를 기록 (고정 오프셋 없음)
void writeBootInfo(const char * imageName, int totalSectorCount, int loaderSectorCount) {
	FILE * image = fopen(imageName, "rb+");
	if(image == NULL) {
		fprintf(stderr, "%s open fail\n", imageName);
		exit(-1);
	}

	char bootSector[BOOT_SECTOR_SIZE];
	if(fread(bootSector, 1, sizeof(bootSector), image) != sizeof(bootSector)) {
		fprintf(stderr, "%s: boot sector read fail\n", imageName);
		exit(-1);
	}

	long offset = -1;
	for(long i=0; i + (long)sizeof(BootInfo) <= BOOT_SECTOR_SIZE; i++) {
		if(memcmp(bootSector + i, BOOT_INFO_SIGNATURE, BOOT_INFO_SIGNATURE_LENGTH) != 0)
			continue;
		if(offset != -1) {
			fprintf(stderr, "%s: duplicate BootInfo signature\n", imageName);
			exit(-1);
		}
		offset = i;
	}
	if(offset == -1) {
		fprintf(stderr, "%s: BootInfo signature not found in boot sector\n", imageName);
		exit(-1);
	}

	fseek(image, offset + BOOT_INFO_SIGNATURE_LENGTH, SEEK_SET);
	unsigned short data = (unsigned short)totalSectorCount;
	fwrite(&data, 2, 1, image);
	data = (unsigned short)loaderSectorCount;
	fwrite(&data, 2, 1, image);
	fclose(image);
}
