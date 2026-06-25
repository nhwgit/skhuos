@echo off
rem skhuOS 빌드 + QEMU 실행 (WSL Ubuntu-24.04 필요: make, nasm, gcc, qemu-system-x86)
cd /d "%~dp0"

wsl -d Ubuntu-24.04 -- bash -c "cd \"$(wslpath '%CD%')\" && make && (test -f build/HDD.img || qemu-img create -f raw build/HDD.img 20M) && qemu-system-x86_64 -m 64 -drive file=build/Disk.img,format=raw,index=0,media=disk -drive file=build/HDD.img,format=raw,index=1,media=disk -boot c"

if errorlevel 1 pause
