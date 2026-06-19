#include <stdio.h>
#include <stdlib.h>

#define SECTOR_SIZE 512
#define KERNEL_INFO_OFFSET 5 // 부트로더의 TOTALSECTORCOUNT 위치

int appendFile(FILE * image, const char * fileName);
void writeKernelInformation(const char * imageName, int totalSectorCount, int kernel32SectorCount);

int main(int argc, char * argv[]) {
	if(argc < 5) {
		fprintf(stderr, "usage: ImageMaker BootLoader.bin Kernel32.bin Kernel64.bin Disk.img\n");
		return -1;
	}

	FILE * image = fopen(argv[4], "wb");
	if(image == NULL) {
		fprintf(stderr, "%s open fail\n", argv[4]);
		return -1;
	}

	appendFile(image, argv[1]);
	int kernel32SectorCount = appendFile(image, argv[2]);
	int kernel64SectorCount = appendFile(image, argv[3]);
	fclose(image);

	writeKernelInformation(argv[4], kernel32SectorCount + kernel64SectorCount, kernel32SectorCount);
	printf("%s create complete\n", argv[4]);
	return 0;
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

void writeKernelInformation(const char * imageName, int totalSectorCount, int kernel32SectorCount) {
	FILE * image = fopen(imageName, "rb+");
	if(image == NULL) {
		fprintf(stderr, "%s open fail\n", imageName);
		exit(-1);
	}

	fseek(image, KERNEL_INFO_OFFSET, SEEK_SET);
	unsigned short data = (unsigned short)totalSectorCount;
	fwrite(&data, 2, 1, image);
	data = (unsigned short)kernel32SectorCount;
	fwrite(&data, 2, 1, image);
	fclose(image);
}
