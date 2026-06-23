BASEDIRECTORY = ../..

.PHONY: all BootLoader ProtectedMode IA32Mode Util clean

all: BootLoader ProtectedMode IA32Mode Util Disk.img
	
BootLoader:
	@echo Build Boot Loader
	make -C $(BASEDIRECTORY)/BootLoader
	@echo Build Complete

ProtectedMode:
	@echo Build ProtectedMode Kernel
	make -C $(BASEDIRECTORY)/ProtectedMode
	@echo Build Complete

IA32Mode:
	@echo Build IA32Mode Kernel
	make -C $(BASEDIRECTORY)/IA32Mode
	@echo Build Complete

Util:
	@echo Build Util
	make -C $(BASEDIRECTORY)/util
	@echo Build Complete

Disk.img: $(BASEDIRECTORY)/BootLoader/BootLoader.bin $(BASEDIRECTORY)/ProtectedMode/ProtectedMode.bin $(BASEDIRECTORY)/IA32Mode/IA32Mode.bin | Util
	@echo Disk Image Build Start
	$(BASEDIRECTORY)/util/ImageMaker/ImageMaker.exe $^ Disk.img
	truncate -s 1440K Disk.img # 1.44MB 플로피 규격으로 패딩 (QEMU가 지오메트리를 인식 못 하면 BIOS 디스크 읽기가 멈춤)
	@echo All Build Complete

clean:
	make -C $(BASEDIRECTORY)/BootLoader clean
	make -C $(BASEDIRECTORY)/ProtectedMode clean
	make -C $(BASEDIRECTORY)/IA32Mode clean
	make -C $(BASEDIRECTORY)/util clean
	rm -f Disk.img