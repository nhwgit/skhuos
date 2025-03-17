BASEDIRECTORY = ../..

all: BootLoader ProtectedMode IA32Mode Disk.img
	
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
	makc -C $(BASEDIRECTORY)/Util
	@echo Build Complete

Disk.img: $(BASEDIRECTORY)/BootLoader/BootLoader.bin $(BASEDIRECTORY)/ProtectedMode/ProtectedMode.bin $(BASEDIRECTORY)/IA32Mode/IA32Mode.bin
	@echo Disk Image Build Start
	$(BASEDIRECTORY)/util/ImageMaker/ImageMaker.exe $^
	@echo All Build Complete

clean:
	make -C $(BASEDIRECTORY)/BootLoader clean
	make -C $(BASEDIRECTORY)/ProtectedMode clean
	make -c $(BASEDIRECTORY)/util clean
	rm -f $(BASEDIRECTORY)/Disk.img