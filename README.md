실행 방법

0. 실행파일.zip 압축 해제

1. 실행파일 폴더의 0실행기.bat을 누르면 가상머신(qemu)에서 OS가 실행된다.

2. help를 누르면 명령어 목록이 나온다.

3. ls, touch, rm, editor등 파일 시스템과 관련된 명령어를 입력하기 전에 아래의 작업을 먼저 해야한다.
- 디스크 포맷팅 (formatting 명령어) : 시간이 수분 소요될 수 있다.
- 포맷팅한 디스크를 마운팅 (mounting 명령어)

4. editor 사용법은 파일을 편집한 이후
esc를 누른 후 q를 누르고 엔터치면 저장하지 않고 메모장이 꺼지고, w를 누르고 엔터치면 저장한 후 메모장이 꺼진다.


# 💻 skhuOS - Intel x64 기반 범용 운영체제

![OS Booting](https://private-user-images.githubusercontent.com/141965854/580476898-830e1cd0-7236-4b90-bedb-b9947f29ec32.png?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NzY2MjY2MjMsIm5iZiI6MTc3NjYyNjMyMywicGF0aCI6Ii8xNDE5NjU4NTQvNTgwNDc2ODk4LTgzMGUxY2QwLTcyMzYtNGI5MC1iZWRiLWI5OTQ3ZjI5ZWMzMi5wbmc_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjYwNDE5JTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI2MDQxOVQxOTE4NDNaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT1jZjQ5Y2NmNjhkMjE5YWNiY2UyYmUxYjc5MjkwZjQ5ZmQyZWE4MDZiZmY4ZTIxYjVmZTc3NGZkOTY3NDBlOWUxJlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCZyZXNwb25zZS1jb250ZW50LXR5cGU9aW1hZ2UlMkZwbmcifQ.CVAdJK67pXNK7nAfEvcZfDF-ZWoM8y20EnG2KGbia-k)

## 📌 프로젝트 개요
- **프로젝트 내용**: Intel x64 확장 프로세서 기반의 밑바닥부터 만드는 간단한 범용 운영체제
- **프로젝트 목적**: 운영체제 코어 및 컴퓨터 구조에 대한 깊은 이해도 향상
- **사용 기술**: `C`, `NASM Assembly`
- **제작 시기**: 2022년 학부 캡스톤 디자인

<br>

## 💡 회고 및 극복한 점 (Challenges)

운영체제를 처음부터 개발하며 다음과 같은 한계점들을 마주하고 해결했습니다.

1. **하드웨어 제어 및 메모리 직접 적재의 어려움**
   - 하드웨어 레지스터에 단 1비트만 잘못 입력되어도 인터럽트가 발생하거나 재부팅되는 등 오동작이 발생했습니다.
   - 코드와 변수를 물리 메모리에 직접 적재할 때, 엉뚱한 곳에 데이터가 적재되는 현상을 겪었습니다. 로직 문제인지 메모리 맵핑 문제인지 원인을 규명하고 해결하는 과정에서 컴퓨터 구조에 대한 이해도를 크게 높일 수 있었습니다.
2. **Bare-metal 환경의 디버깅 한계**
   - 윈도우나 리눅스 환경과 달리, 밑바닥부터 만드는 환경에서는 전용 디버거가 존재하지 않아 오직 메모리 덤프와 화면 출력에 의존하여 디버깅을 수행해야 했습니다.
3. **표준 라이브러리(C Standard Library) 부재**
   - `stdio.h`, `stdlib.h` 등 기존 OS에 종속된 C 표준 라이브러리를 사용할 수 없어, 화면 출력부터 메모리 관리까지 직접 구현해야 했습니다.

---

## 🛠 시스템 아키텍처 및 세부 기능

### 1. 부팅 (Booting)
64비트 모드로 진입하기 위해 다음의 4단계를 거칩니다.
> **전원 인가** ➡ **BIOS (부트로더 실행)** ➡ **Real-Address Mode (16비트)** ➡ **Protected Mode (32비트)** ➡ **IA-32e Mode (64비트)**

- **BIOS 및 부트로더 (0x7C00):** 디스크에서 OS 이미지를 읽어 메모리에 적재 후 Real-Address Mode로 전환. VGA 비디오 메모리(0xB800)를 제어해 부팅 과정을 출력.
- **Real-Address Mode:** 
  - 세그먼트 디스크립터(GDT)를 설정하여 물리 메모리 0~4GB 영역 매핑.
  - 시스템 컨트롤 포트(0x92)를 제어하여 **A20 게이트 활성화** (20번째 주소 비트 해제).
  - `CR0` 레지스터의 PE 필드를 1로 설정하여 Protected 모드 전환.
- **Protected Mode:**
  - 4KB 단위의 페이지 테이블(PML4 구조) 구성. 커널 영역(32MB)과 응용 프로그램 영역을 분리.
  - `CR4` 레지스터(PAE 활성화), `CR3` 레지스터(PML4E 주소 지정), `IA32_EFER` 레지스터 설정 후 IA-32e(64비트) 모드로 진입.

<details>
<summary><b>GDT 및 Paging 설정 코드 보기</b></summary>

```nasm
; GDT (Global Descriptor Table) 설정 예시
gdtr:
    dw gdtEnd - gdt - 1
    dd gdt + 0x10000 - $$
gdt:
    dq 0x0000000000000000 ; Null Descriptor
    ; Data / Code Descriptors 생략...
gdtEnd:
```
```c
// Page Table 설정 (4KB 단위)
PtEntry * ptEntry = (PtEntry *)PTABLE_BASE_ADDRESS;

// 커널 영역 (Valid)
for(int i = 0; i < KERNEL_SIZE * 512; i++) {
    int physicalAddress = i * 0x1000;
    ptEntry[i].lower4Byte = PAGE_LOWER4B_FLAGS_P | PAGE_LOWER4B_FLAGS_RW | physicalAddress;
    ptEntry[i].upper4Byte = ((physicalAddress >> 28) & 0xFF);
}
```
</details>

<br>

### 2. 인터럽트 및 예외 처리 (Interrupts/Exceptions)
![x86 OS 에서의 인터럽트와 예외](https://private-user-images.githubusercontent.com/141965854/580476894-6a9592cc-5019-469c-97bb-e3602425b64f.jpeg?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NzY2MjY4NzgsIm5iZiI6MTc3NjYyNjU3OCwicGF0aCI6Ii8xNDE5NjU4NTQvNTgwNDc2ODk0LTZhOTU5MmNjLTUwMTktNDY5Yy05N2JiLWUzNjAyNDI1YjY0Zi5qcGVnP1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI2MDQxOSUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNjA0MTlUMTkyMjU4WiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9NGViOTc3NzA3ZWEyNzQ5YWQyMzI0M2YxMDQyZGY5OWUxODY0Y2MxMzMzY2RlZWI4NGNhZTFmYTBlM2I3ZTFhNyZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QmcmVzcG9uc2UtY29udGVudC10eXBlPWltYWdlJTJGanBlZyJ9.rhTTqV44p6C8yqwOoFXG5sXA2aRFjGb5tzVoDgAKytw)
- **IDT (Interrupt Descriptor Table):** Interrupt Gate를 사용하여 다중 인터럽트 방지 및 컨텍스트 스위칭 간소화.
- **PIC (Programmable Interrupt Controller):** 마스터/슬레이브 PIC 초기화 및 IRQ 라우팅. 폴링 방식의 비효율성을 극복하고 이벤트 기반의 하드웨어 제어 구현.
- **Context Saving/Loading:** 인터럽트 발생 시 레지스터 상태를 TSS(Task State Segment) 기반 스택에 저장(`SAVEREG`)하고 복원(`LOADREG`).

<br>

### 3. 디바이스 드라이버 (Device Drivers)
1. **키보드 드라이버:** 8042 컨트롤러 ➡ PIC ➡ CPU 흐름으로 스캔 코드를 수신. 원형 큐를 구현하여 스캔 코드를 버퍼링하고 아스키코드로 변환하여 화면에 출력.
2. **타이머 드라이버 (PIT 8253):** Mode 2를 사용하여 일정 시간마다 주기적으로 인터럽트가 발생하도록 구성. 이를 기반으로 **Round Robin 스케줄링**을 구현.
3. **디스크 드라이버:** HDD 컨트롤러와 통신. LBA(Logical Block Addressing) 방식을 사용하여 지정된 섹터/실린더/헤드에서 블록 단위의 I/O 읽기/쓰기 구현.

<br>

### 4. 프로세스 및 스케줄링 (Process Management)
- **PCB (Process Control Block) 기반 관리:** PID, 페이지 테이블 주소, 스택 정보, Context(레지스터) 정보를 담은 PCB 구조체 설계.
- **스케줄링 (Round Robin):** 
  - PIT 인터럽트를 통해 10ms 단위로 선점형 멀티태스킹 수행.
  - 큐 기반의 스케줄러를 구성하여, 타이머 만료 시 또는 자발적 양보(Yield) 시 컨텍스트 스위칭(`switchContext`) 발생.
- **동기화 (Synchronization):** 공유 자원 보호를 위해 원자적 연산(CAS; Compare-And-Swap)을 활용한 **Mutex(뮤텍스)** 락 구현. (재진입 가능 구조)

<br>

### 5. 파일 시스템 (File System)
![FAT Structure](https://private-user-images.githubusercontent.com/141965854/580476904-d3db573c-0141-468c-a8e3-078b9ba747a1.png?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NzY2MjY4NzgsIm5iZiI6MTc3NjYyNjU3OCwicGF0aCI6Ii8xNDE5NjU4NTQvNTgwNDc2OTA0LWQzZGI1NzNjLTAxNDEtNDY4Yy1hOGUzLTA3OGI5YmE3NDdhMS5wbmc_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjYwNDE5JTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI2MDQxOVQxOTIyNThaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT04M2E0NDU3YTJlZjUzMWRlODA2YWZkZGZjZmVlNmJjYmFhNGFhZDZjYmQxNWUxNzBmZTUyZDI5OWRlYjAzMDRhJlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCZyZXNwb25zZS1jb250ZW50LXR5cGU9aW1hZ2UlMkZwbmcifQ.nIPeEoyE2gYXwgfKwS7WjcM0UW4qsUTUi_3zQC84bEw)

Windows의 FAT 파일 시스템을 모방한 커스텀 파일 시스템을 구축했습니다.
1. **BR 영역:** 파일 시스템의 메타 정보 (FAT 시작 위치, Data 주소 등).
2. **FAT 영역:** 클러스터 링크 테이블 저장 (파일의 단편화 추적).
3. **Root Directory:** 파일 메타데이터 (이름, 용량, 시작 클러스터) 저장.

<br>

### 6. 터미널 셸 명령어 (Shell Commands)

OS 상호작용을 위해 기본적인 셸과 명령어 체계를 구축했습니다.

| 명령어 | 기능 설명 |
|---|---|
| `help` / `clear` / `reboot` | 명령어 목록 출력 / 화면 지우기 / 시스템 재부팅 |
| `diskCapacity` | HDD 컨트롤러에서 읽어온 물리 디스크 용량 출력 |
| `formatting` / `mount` | 디스크를 FAT 구조로 포맷 / OS에 마운트 |
| `ls` / `touch` / `rm` | 디렉터리 목록 조회 / 빈 파일 생성 / 파일 삭제 |
| `multiTasking` | 멀티태스킹 데모 시각화 (24개 프로세스 동시 실행) |
| `editor [name]` | 간단한 텍스트 에디터 실행 및 저장 |

---

## 🖥 데모 (Demonstration)

### 1. 선점형 멀티태스킹 (`multiTasking`)
![Multitasking](https://private-user-images.githubusercontent.com/141965854/580476901-0c22e001-5d52-4ce6-b361-51ba39e30bc8.png?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NzY2MjY4NzgsIm5iZiI6MTc3NjYyNjU3OCwicGF0aCI6Ii8xNDE5NjU4NTQvNTgwNDc2OTAxLTBjMjJlMDAxLTVkNTItNGNlNi1iMzYxLTUxYmEzOWUzMGJjOC5wbmc_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjYwNDE5JTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI2MDQxOVQxOTIyNThaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT0zMDA5MGNmNjRhMmM4MzFhODY2YjAwM2ViZjg4ZjRjOGQxNzZkODZlODE4ZDkzODU0ODZjMjE3NjFmMmQ1YzJlJlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCZyZXNwb25zZS1jb250ZW50LXR5cGU9aW1hZ2UlMkZwbmcifQ.nRTBLlE6kHcPxVsUEKcVuLI9AJqdL7R2-1hgbFVzHyo)
- 총 24개의 프로세스가 동시에 생성되며, 각 줄마다 하나의 프로세스가 할당됩니다.
- 타이머 인터럽트를 통해 컨텍스트 스위칭이 일어나며 좌측부터 우측으로 순차적으로 문자가 출력됩니다.

### 2. 파일 생성 및 텍스트 에디터 (`touch`, `editor`)
![Editor 1](https://private-user-images.githubusercontent.com/141965854/580476899-26b22e5c-6d93-4c59-9466-e9945fce6951.png?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NzY2MjY4NzgsIm5iZiI6MTc3NjYyNjU3OCwicGF0aCI6Ii8xNDE5NjU4NTQvNTgwNDc2ODk5LTI2YjIyZTVjLTZkOTMtNGM1OS05NDY2LWU5OTQ1ZmNlNjk1MS5wbmc_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjYwNDE5JTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI2MDQxOVQxOTIyNThaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT05NDllZDUwODMyYmUzYzY2NDgxMWNhNTZhN2VkY2Y4M2ZiOTg1Yjg2OGI3MWI2NWYyYmY2MmUyNmNiMTQ2ZWZmJlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCZyZXNwb25zZS1jb250ZW50LXR5cGU9aW1hZ2UlMkZwbmcifQ.i2xcM18s7ObPJ6i8nmhQFGfXkYlVM517x8UVsuYKnVA)
- `touch aaa` 명령어로 0 Byte 파일을 생성하고 `ls`로 확인합니다.

![Editor 2](https://private-user-images.githubusercontent.com/141965854/580476896-b53f396c-16a8-488b-8f96-dbb5935fefd2.png?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NzY2MjY4NzgsIm5iZiI6MTc3NjYyNjU3OCwicGF0aCI6Ii8xNDE5NjU4NTQvNTgwNDc2ODk2LWI1M2YzOTZjLTE2YTgtNDg4Yi04Zjk2LWRiYjU5MzVmZWZkMi5wbmc_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjYwNDE5JTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI2MDQxOVQxOTIyNThaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT1hZTFjYWVkZDYxMDZlZjk2MmI2NjA2YWRlNjk3ZmM1YmM3M2M1MzI2ODhjYzcwMjBiNWY0YTIzNDA2NTc2ZWFlJlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCZyZXNwb25zZS1jb250ZW50LXR5cGU9aW1hZ2UlMkZwbmcifQ.VUZNFfMnUNQKIOLcOJKgHestO7yBJFhiF705pKPRQkE)
- `editor aaa`를 통해 에디터 화면으로 전환 후 `www.skhu.ac.kr`를 입력 및 저장(ESC -> w)합니다.

![Editor 3](https://private-user-images.githubusercontent.com/141965854/580476892-f24fff83-c68c-4def-8ad7-1ca0ee0caf13.png?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NzY2MjY4NzgsIm5iZiI6MTc3NjYyNjU3OCwicGF0aCI6Ii8xNDE5NjU4NTQvNTgwNDc2ODkyLWYyNGZmZjgzLWM2OGMtNGRlZi04YWQ3LTFjYTBlZTBjYWYxMy5wbmc_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjYwNDE5JTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI2MDQxOVQxOTIyNThaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT1mMGU5NDQyYjhmNDNiMGE4ODc2MTQ5YTA5MzY5NWUzMDBhMzMxN2M2YzFkMGYwODJkM2IzNDY1MmM5NzA1NmY5JlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCZyZXNwb25zZS1jb250ZW50LXR5cGU9aW1hZ2UlMkZwbmcifQ.Csu794fafe2ESnphS4dLiRwHcrXqyKVgQ5c9-4n_3ws)
- 다시 `ls`를 입력하면 `aaa` 파일의 크기가 디스크에 쓰여 정상적으로 14 Byte로 증가한 것을 확인할 수 있습니다.
