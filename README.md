# 💻 skhuOS — Intel x64 기반 범용 운영체제

![Multitasking Demo](docs/images/multitasking.png)

> `multiTasking` 명령어 실행 화면 — 24개 프로세스가 타이머 인터럽트 기반 선점형 스케줄링으로 동시에 실행됩니다.

## 📌 프로젝트 개요

- **프로젝트 내용**: Intel x64 확장 프로세서 기반의, 밑바닥부터 만드는 간단한 범용 운영체제
- **프로젝트 목적**: 운영체제 코어 및 컴퓨터 구조에 대한 깊은 이해도 향상
- **사용 기술**: `C`, `NASM Assembly` (외부 라이브러리 없이 약 4,300줄)
- **타깃 환경**: x86_64 (QEMU 가상머신에서 부팅)
- **제작 시기**: 2022년 학부 캡스톤 디자인

### 구현 범위 한눈에 보기

BIOS 부팅부터 셸 위에서 텍스트 에디터를 실행하기까지, 아래 전 과정을 직접 구현했습니다.

| 영역 | 구현 내용 |
| --- | --- |
| 부팅 | 512B 부트로더, 16 → 32 → 64비트 모드 전환 (GDT · A20 · CR0/CR3/CR4/EFER) |
| 메모리 | PML4 4단계 페이지 테이블 (4KB 페이지, 커널/응용 영역 분리) |
| 인터럽트 | IDT · PIC 초기화, TSS/IST 기반 컨텍스트 저장/복원 |
| 프로세스 | PCB · 라운드 로빈 선점형 스케줄러(10ms) · CAS 기반 재진입 뮤텍스 |
| 드라이버 | 키보드(8042) · 타이머(PIT 8253) · 디스크(ATA PIO, LBA) |
| 파일 시스템 | FAT 구조 커스텀 파일 시스템 (포맷 · 마운트 · 생성 · 삭제) |
| 응용 | 대화형 셸 + 텍스트 에디터 |

---

## 🗂 프로젝트 구조

```text
skhuos/
├── BootLoader/        # 16비트 부트섹터 (Real-Address Mode 진입)
│   ├── BootLoader.asm
│   └── Makefile
├── ProtectedMode/     # 32비트 커널 (Protected Mode 진입 + 페이징 설정)
│   ├── src/           # Entry.s, Main.c, Page.c, SwitchToIA32.asm 등
│   └── Makefile
├── IA32Mode/          # 64비트 커널 본체 (Shell, FS, Process, Driver 등)
│   ├── src/           # Shell, Process, FAT, Keyboard, PIT, Disk 등
│   └── makefile
├── util/
│   └── ImageMaker/    # 부트로더 + 커널 → Disk.img 생성 도구
├── build/             # 빌드 산출물
├── ImageMaker.exe     # 미리 빌드된 Image Maker
├── 실행파일.zip        # 미리 빌드된 OS 이미지 + QEMU 실행 스크립트
└── Makefile           # 통합 빌드 (BootLoader → ProtectedMode → IA32Mode → Disk.img)
```

각 모드(`BootLoader` → `ProtectedMode` → `IA32Mode`)는 부팅 단계와 1:1 대응되며, 단계별로 별도 바이너리로 빌드된 뒤 `ImageMaker`가 하나의 디스크 이미지로 합칩니다.

---

## 🚀 빠른 실행 (미리 빌드된 이미지 사용)

소스 빌드 없이 동작을 확인할 수 있는 가장 빠른 방법입니다.

1. `실행파일.zip`을 압축 해제합니다.
2. `실행파일/0실행기.bat`을 실행하면 QEMU 가상머신에서 OS가 부팅됩니다.
3. 부팅 후 셸 프롬프트에서 `help`를 입력하면 사용 가능한 명령어 목록이 출력됩니다.

### 파일 시스템 사용 전 필수 단계

`ls`, `touch`, `rm`, `editor` 등 파일 시스템 명령어를 사용하기 전에 반드시 다음을 먼저 실행해야 합니다.

| 순서 | 명령어 | 설명 |
| --- | --- | --- |
| 1 | `formatting` | 디스크를 FAT 구조로 포맷합니다. **수 분 소요될 수 있습니다.** |
| 2 | `mounting` | 포맷팅된 디스크를 OS에 마운트합니다. |

### `editor` 사용법

```text
editor <파일명>     # 에디터 진입
ESC → q → Enter    # 저장하지 않고 종료
ESC → w → Enter    # 저장 후 종료
```

---

## 🛠 소스에서 빌드

### 의존성

- `make`
- `gcc` (x86_64-elf 크로스 컴파일러 권장)
- `nasm` (NASM Assembler)
- `qemu-system-x86_64` (실행용)

### 빌드

루트 디렉터리에서:

```bash
make all      # BootLoader, ProtectedMode, IA32Mode를 차례로 빌드 후 Disk.img 생성
make clean    # 빌드 산출물 정리
```

빌드가 끝나면 `Disk.img`가 생성됩니다. QEMU로 실행:

```bash
qemu-system-x86_64 -m 64 -fda Disk.img
```

---

## 🏗 시스템 아키텍처 및 세부 기능

### 1. 부팅 (Booting)

64비트 모드로 진입하기 위해 다음의 4단계를 거칩니다.

> **전원 인가** ➡ **BIOS (부트로더 실행)** ➡ **Real-Address Mode (16비트)** ➡ **Protected Mode (32비트)** ➡ **IA-32e Mode (64비트)**

- **BIOS 및 부트로더 (0x7C00)**: 디스크에서 OS 이미지를 읽어 메모리에 적재 후 Real-Address Mode로 전환. VGA 비디오 메모리(0xB800)를 제어해 부팅 과정을 출력.
- **Real-Address Mode**:
  - 세그먼트 디스크립터(GDT)를 설정하여 물리 메모리 0~4GB 영역 매핑.
  - 시스템 컨트롤 포트(0x92)를 제어하여 **A20 게이트 활성화** (20번째 주소 비트 해제).
  - `CR0` 레지스터의 PE 필드를 1로 설정하여 Protected 모드로 전환.
- **Protected Mode**:
  - 4KB 단위의 페이지 테이블(PML4 구조) 구성. 커널 영역(32MB)과 응용 프로그램 영역을 분리.
  - `CR4`(PAE 활성화), `CR3`(PML4E 주소 지정), `IA32_EFER` 레지스터 설정 후 IA-32e(64비트) 모드로 진입.

<details>
<summary>GDT 및 Paging 설정 코드 보기</summary>

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
for (int i = 0; i < KERNEL_SIZE * 512; i++) {
    int physicalAddress = i * 0x1000;
    ptEntry[i].lower4Byte = PAGE_LOWER4B_FLAGS_P | PAGE_LOWER4B_FLAGS_RW | physicalAddress;
    ptEntry[i].upper4Byte = ((physicalAddress >> 28) & 0xFF);
}
```

</details>

### 2. 인터럽트 및 예외 처리 (Interrupts/Exceptions)

- **IDT (Interrupt Descriptor Table)**: Interrupt Gate를 사용하여 다중 인터럽트 방지 및 컨텍스트 스위칭 간소화.
- **PIC (Programmable Interrupt Controller)**: 마스터/슬레이브 PIC 초기화 및 IRQ 라우팅. 폴링 방식의 비효율성을 극복하고 이벤트 기반의 하드웨어 제어 구현.
- **Context Saving/Loading**: 인터럽트 발생 시 레지스터 상태를 TSS(Task State Segment) 기반 스택에 저장(`SAVEREG`)하고 복원(`LOADREG`).

<details>
<summary>SAVEREG 매크로 코드 보기 (Handler.asm)</summary>

```nasm
%macro SAVEREG 0
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax

    mov ax, ds
    push rax
    mov ax, es
    push rax
    push fs
    push gs

    mov ax, 0x08      ; 커널 데이터 세그먼트로 전환
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
%endmacro
```

</details>

### 3. 디바이스 드라이버 (Device Drivers)

1. **키보드 드라이버**: 8042 컨트롤러 ➡ PIC ➡ CPU 흐름으로 스캔 코드를 수신. 원형 큐를 구현하여 스캔 코드를 버퍼링하고 아스키 코드로 변환하여 화면에 출력.
2. **타이머 드라이버 (PIT 8253)**: Mode 2를 사용하여 일정 시간마다 주기적으로 인터럽트가 발생하도록 구성. 이를 기반으로 **Round Robin 스케줄링** 구현.
3. **디스크 드라이버**: HDD 컨트롤러와 통신. LBA(Logical Block Addressing) 방식을 사용하여 지정된 섹터/실린더/헤드에서 블록 단위의 I/O 읽기/쓰기 구현.

### 4. 프로세스 및 스케줄링 (Process Management)

- **PCB (Process Control Block) 기반 관리**: PID, 페이지 테이블 주소, 스택 정보, Context(레지스터) 정보를 담은 PCB 구조체 설계.

<details>
<summary>PCB 구조체 보기 (Process.h)</summary>

```c
#pragma pack(push, 1)
typedef struct processContext {
    QWORD reg[24];          // GS/FS/ES/DS, RAX~R15, RIP, CS, RFLAGS, RSP, SS
} ProcessContext;

typedef struct pcb {
    Node link;              // 스케줄러 LinkedList의 연결 노드 (id == pid)
    QWORD * pageTableAddress;
    QWORD stackSize;
    QWORD * stackAddress;
    ProcessContext context;
} PCB;

typedef struct processScheduler {
    PCB * runningProcess;
    LinkedList processList;
} ProcessScheduler;
#pragma pack(pop)
```

</details>

- **스케줄링 (Round Robin)**:
  - PIT 인터럽트를 통해 10ms 단위로 선점형 멀티태스킹 수행.
  - 큐 기반 스케줄러를 구성하여, 타이머 만료 시 또는 자발적 양보(Yield) 시 컨텍스트 스위칭(`switchContext`) 발생.

<details>
<summary>schedule() 코드 보기 (Process.c)</summary>

```c
void schedule(void) {
    bool preIf = setIf(FALSE);              // 인터럽트 잠시 차단

    if (scheduler.processList.count == 0) {
        setIf(preIf);
        return;
    }

    PCB *nextProcess = (PCB *)HeadRemove(&(scheduler.processList));

    if (nextProcess != NULL) {
        PCB *curProcess = scheduler.runningProcess;
        insertList(&(scheduler.processList), curProcess);  // 현재 프로세스를 큐 꼬리로
        scheduler.runningProcess = nextProcess;
        switchContext(&(curProcess->context), &(nextProcess->context));
    }
    setIf(preIf);
}
```

</details>

- **동기화 (Synchronization)**: 공유 자원 보호를 위해 원자적 연산(CAS; Compare-And-Swap)을 활용한 Mutex(뮤텍스) 락 구현 (재진입 가능 구조).

<details>
<summary>Mutex 구현 코드 보기 (Sync.c)</summary>

```c
void initMutex(Mutex * mutex) {
    mutex->avaliable = TRUE;
    mutex->count = 0;
    mutex->pid = -1;
}

void acquireLock(Mutex * mutex) {
    WORD currentPid = getRunningPid();

    // 재진입: 동일 프로세스가 이미 보유한 락이면 카운트만 증가
    if (mutex->avaliable == FALSE && mutex->pid == currentPid) {
        mutex->count++;
        return;
    }

    // CAS: avaliable이 TRUE일 때만 FALSE로 원자적 교환. 실패 시 yield
    while (__sync_bool_compare_and_swap(&(mutex->avaliable), TRUE, FALSE) == FALSE) {
        schedule();
    }
    mutex->pid = currentPid;
    mutex->count = 1;
}

void releaseLock(Mutex * mutex) {
    WORD currentPid = getRunningPid();

    if (mutex->pid != currentPid || mutex->avaliable == TRUE) {
        return;
    }

    mutex->count--;

    if (mutex->count == 0) {
        mutex->pid = -1;
        __sync_synchronize();        // 메모리 배리어: pid 클리어 후 release 보장
        mutex->avaliable = TRUE;
    }
}
```

</details>

### 5. 파일 시스템 (File System)

Windows의 FAT 파일 시스템을 모방한 커스텀 파일 시스템을 구축했습니다. 디스크를 세 영역으로 나눕니다.

1. **BR 영역**: 파일 시스템의 메타 정보 (FAT 시작 위치, Data 주소 등).
2. **FAT 영역**: 클러스터 링크 테이블 저장 (파일의 단편화 추적).
3. **Root Directory**: 파일 메타데이터 (이름, 용량, 시작 클러스터) 저장.

### 6. 터미널 셸 명령어 (Shell Commands)

OS 상호작용을 위해 기본적인 셸과 명령어 체계를 구축했습니다.

| 명령어 | 기능 설명 |
| --- | --- |
| `help` / `clear` / `reboot` | 명령어 목록 출력 / 화면 지우기 / 시스템 재부팅 |
| `diskCapacity` | HDD 컨트롤러에서 읽어온 물리 디스크 용량 출력 |
| `formatting` / `mounting` | 디스크를 FAT 구조로 포맷 / OS에 마운트 |
| `ls` / `touch` / `rm` | 디렉터리 목록 조회 / 빈 파일 생성 / 파일 삭제 |
| `multiTasking` | 멀티태스킹 데모 시각화 (24개 프로세스 동시 실행) |
| `editor [name]` | 간단한 텍스트 에디터 실행 및 저장 |

---

## 🖥 데모 (Demonstration)

### 1. 선점형 멀티태스킹 (`multiTasking`)

문서 상단의 대표 이미지가 이 명령어의 실행 화면입니다.

- 총 24개의 프로세스가 동시에 생성되며, 각 줄마다 하나의 프로세스가 할당됩니다.
- 타이머 인터럽트를 통해 컨텍스트 스위칭이 일어나며 좌측부터 우측으로 순차적으로 문자가 출력됩니다.

### 2. 파일 생성 및 텍스트 에디터 (`touch`, `editor`)

![Editor 1 - touch](docs/images/editor1.png)

- `touch aaa` 명령어로 0 Byte 파일을 생성하고 `ls`로 확인합니다.

![Editor 2 - edit](docs/images/editor2.png)

- `editor aaa`를 통해 에디터 화면으로 전환 후 `www.skhu.ac.kr`를 입력 및 저장(ESC → w)합니다.

![Editor 3 - result](docs/images/editor3.png)

- 다시 `ls`를 입력하면 `aaa` 파일의 크기가 디스크에 쓰여 14 Byte로 정상 증가한 것을 확인할 수 있습니다.

---

## 🚧 한계와 향후 과제

학습용 OS로서 의도적으로 단순화한 부분과, 그 확장 방향을 명확히 인지하고 있습니다.

| 한계 | 현재 상태 | 확장 방향 |
| --- | --- | --- |
| 특권 분리 없음 | 모든 코드가 Ring 0에서 동작, 특권 전환을 동반하는 시스템 콜 부재 | DPL3 세그먼트 + 페이지 U/S 비트 + `syscall` 진입점 |
| 단일 주소 공간 | identity 매핑, 컨텍스트 스위칭 시 CR3 미교체 | 프로세스별 페이지 테이블 + demand paging |
| Blocked 상태 부재 | 디스크 대기·`sleep`이 busy-wait으로 CPU 소모 | READY/BLOCKED 상태 모델 + 타이머 만료 큐 |
| 단일 코어 | PIC + 인터럽트 차단 기반 동기화 | APIC 전환, 코어별 런큐 |
| 파일 시스템 | 계층 디렉터리·버퍼 캐시·저널링 없음 | 캐시 → 계층 디렉터리 → 저널링 순 도입 |

---

## 💡 회고 및 극복한 점 (Challenges)

1. **메모리 적재 주소 불일치 버그**
   - 코드와 변수를 물리 메모리에 직접 적재하는 과정에서 데이터가 엉뚱한 주소에 쓰여, 쓰레기 값이 출력되거나 예고 없이 재부팅(트리플 폴트)되는 문제를 겪었습니다.
   - VGA 출력으로 단계별 체크포인트를 찍어 죽는 지점을 이분 탐색하고, QEMU 메모리 덤프로 기대 주소와 실제 적재 내용을 대조하여 원인을 추적했습니다.
   - 원인은 링커 스크립트가 가정하는 적재 주소와 부트로더가 실제 복사하는 목적지의 불일치였고, 메모리 맵을 문서로 정리해 일치시킨 뒤 모든 고정 주소를 헤더 상수로 일원화했습니다.
2. **Bare-metal 환경의 디버깅 한계**
   - 전용 디버거가 없는 환경에서 메모리 덤프와 화면 출력에 의존해 디버깅했습니다. 하드웨어 레지스터에 단 1비트만 잘못 써도 "논리 오류"가 아닌 "재부팅"으로 나타나기 때문에, 재현 가능한 최소 단계와 체크포인트 출력을 먼저 갖추는 것이 디버깅 속도를 결정한다는 것을 배웠습니다.
3. **표준 라이브러리(C Standard Library) 부재**
   - `printf`, `malloc` 등은 모두 OS의 시스템 콜 위에 구현된 것이라 bare-metal에서는 쓸 수 없습니다. VGA 텍스트 메모리(0xB8000) 직접 제어를 통한 화면 출력부터 문자열/메모리 조작 함수까지 직접 구현했습니다.
