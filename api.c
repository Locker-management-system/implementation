#include <termio.h>
#include <stdio.h>

/**
 * 입력한 값을 화면에 표시하지 않음
 * @return 누른 키 값
 */
int getkey(void) {
    int ch;
    struct termios old;
    struct termios current;

    /* 현재 설정된 terminal i/o 값을 backup함 */
    tcgetattr(0, &old);

    /* 현재의 설정된 terminal i/o에 일부 속성만 변경하기 위해 복사함 */
    current = old;

    /* buffer i/o를 중단함 */
    current.c_lflag &= ~ICANON;

    current.c_lflag &= ~ECHO;

    /* 변경된 설정값으로 설정합니다.*/
    tcsetattr(0, TCSANOW, &current);
    ch = getchar();
    tcsetattr(0, TCSANOW, &old);

    return ch;
}