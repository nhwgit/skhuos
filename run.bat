@echo off
rem skhuOS 빌드 + QEMU 실행 (WSL Ubuntu-24.04 필요: make, nasm, gcc, qemu-system-x86)
cd /d "%~dp0"

wsl -d Ubuntu-24.04 -- bash -c "cd \"$(wslpath '%CD%')/build/default\" && make -f ../../Makefile all && (test -f HDD.img || qemu-img create -f raw HDD.img 20M) && qemu-system-x86_64 -m 64 -fda Disk.img -hda HDD.img -boot a"

if errorlevel 1 pause
