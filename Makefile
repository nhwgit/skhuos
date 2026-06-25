# skhuOS 통합 빌드 — Linux/WSL에서 실행 (make, nasm, gcc, gcc-multilib, binutils)
#
# 저장소 루트에서 `make` 한 번으로 boot → loader → kernel → Disk.img까지 빌드한다.
# 모든 산출물은 $(BUILD) 아래에만 생성되며 소스 트리는 변경되지 않는다.
#
#   boot/    16비트 부트섹터            → build/boot.bin
#   loader/  32비트 브릿지 (보호 모드)   → build/loader.bin
#   kernel/  64비트 커널 (IA-32e)       → build/kernel.bin
#   tools/   ImageMaker               → build/tools/imagemaker
#            최종 디스크 이미지          → build/Disk.img

BUILD ?= build

NASM    := nasm
CC      := gcc
LD      := ld
OBJCOPY := objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

WARN     := -Werror=implicit-function-declaration
CFLAGS32 := -m32 -ffreestanding -fno-pie -fno-stack-protector $(WARN) -MMD -MP -Icommon
# -mno-red-zone -mno-mmx -mno-sse -mno-sse2: freestanding 커널 필수 (빼면 #UD/링크 깨짐)
CFLAGS64 := -m64 -ffreestanding -fno-pie -fno-stack-protector \
            -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
            $(WARN) -MMD -MP -Icommon -Ikernel

COMMON_SRCS := $(wildcard common/*.c)

all: $(BUILD)/Disk.img

# ---------------- boot: 16비트 부트섹터 ----------------

$(BUILD)/boot.bin: boot/BootLoader.asm
	@mkdir -p $(dir $@)
	$(NASM) -o $@ $<

# ---------------- loader: 32비트 브릿지 ----------------
# Entry.s(플랫 바이너리)가 0x10200으로 점프하므로 Main.o가 링크 선두여야 한다.

LOADER_OBJS := $(BUILD)/obj32/loader/Main.o \
               $(patsubst %.c,$(BUILD)/obj32/%.o,$(filter-out loader/Main.c,$(wildcard loader/*.c)) $(COMMON_SRCS)) \
               $(patsubst %.asm,$(BUILD)/obj32/%.o,$(wildcard loader/*.asm))

$(BUILD)/obj32/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS32) -o $@ $<

$(BUILD)/obj32/%.o: %.asm
	@mkdir -p $(dir $@)
	$(NASM) -f elf32 -o $@ $<

$(BUILD)/loader_entry.bin: loader/Entry.s
	@mkdir -p $(dir $@)
	$(NASM) -o $@ $<

$(BUILD)/loader.elf: $(LOADER_OBJS) loader/linker.ld
	$(LD) -melf_i386 -T loader/linker.ld -nostdlib -e Main -Ttext 0x10200 -o $@ $(LOADER_OBJS)

$(BUILD)/loader.bin: $(BUILD)/loader_entry.bin $(BUILD)/loader.elf
	$(OBJCOPY) $(BUILD)/loader.elf $(BUILD)/loader.elf.bin
	cat $(BUILD)/loader_entry.bin $(BUILD)/loader.elf.bin > $@

# ---------------- kernel: 64비트 커널 ----------------
# loader가 0x600000으로 점프하므로 Entry.o가 링크 선두여야 한다.

KERNEL_ENTRY := $(BUILD)/obj64/kernel/arch/Entry.o
KERNEL_OBJS  := $(KERNEL_ENTRY) \
                $(patsubst %.c,$(BUILD)/obj64/%.o,$(wildcard kernel/*.c kernel/*/*.c) $(COMMON_SRCS)) \
                $(patsubst %.asm,$(BUILD)/obj64/%.o,$(wildcard kernel/*/*.asm))

$(BUILD)/obj64/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS64) -o $@ $<

$(BUILD)/obj64/%.o: %.asm
	@mkdir -p $(dir $@)
	$(NASM) -f elf64 -o $@ $<

$(KERNEL_ENTRY): kernel/arch/Entry.s
	@mkdir -p $(dir $@)
	$(NASM) -f elf64 -o $@ $<

$(BUILD)/kernel.elf: $(KERNEL_OBJS) kernel/linker.ld
	$(LD) -melf_x86_64 -T kernel/linker.ld -nostdlib -e Main -Ttext 0x600000 -o $@ $(KERNEL_OBJS)

$(BUILD)/kernel.bin: $(BUILD)/kernel.elf
	$(OBJCOPY) $< $@

# ---------------- tools & 디스크 이미지 ----------------

$(BUILD)/tools/imagemaker: tools/imagemaker/ImageMaker.c common/BootInfo.h
	@mkdir -p $(dir $@)
	$(CC) -o $@ tools/imagemaker/ImageMaker.c

$(BUILD)/Disk.img: $(BUILD)/boot.bin $(BUILD)/loader.bin $(BUILD)/kernel.bin $(BUILD)/tools/imagemaker
	$(BUILD)/tools/imagemaker $(BUILD)/boot.bin $(BUILD)/loader.bin $(BUILD)/kernel.bin $@

clean:
	rm -rf $(BUILD)

.PHONY: all clean

-include $(LOADER_OBJS:.o=.d) $(KERNEL_OBJS:.o=.d)
