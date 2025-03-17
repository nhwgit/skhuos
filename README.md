# skhuos
오직 하드웨어만 있는 상태에서 운영체제를 직접 구현했습니다. 부팅 과정, 인터럽트 처리, 표준 입출력, 디스크 입출력, 멀티프로세스 기능, 메모장 애플리케이션 등을 개발하며 운영체제의 핵심 원리를 익혔습니다. 이를 통해 시스템 프로그래밍의 기본 개념과 저수준 개발 역량을 크게 향상시켰습니다.

실행 방법

0. 실행파일.zip 압축 해제

1. 실행파일 폴더의 0실행기.bat을 누르면 가상머신(qemu)에서 OS가 실행된다.

2. help를 누르면 명령어 목록이 나온다.

3. ls, touch, rm, editor등 파일 시스템과 관련된 명령어를 입력하기 전에 아래의 작업을 먼저 해야한다.
- 디스크 포맷팅 (formatting 명령어) : 시간이 수분 소요될 수 있다.
- 포맷팅한 디스크를 마운팅 (mounting 명령어)

4. editor 사용법은 파일을 편집한 이후
esc를 누른 후 q를 누르고 엔터치면 저장하지 않고 메모장이 꺼지고, w를 누르고 엔터치면 저장한 후 메모장이 꺼진다.
