#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void println(int argn, ...) {
    va_list va;
    int flag;
    int nextInt;
    char *nextStr;
    va_start(va, argn);

    for (int i = 0; i < argn; i += 2) {z
        flag = va_arg(va, int);
        switch (flag) {
            case 0:
                nextInt = va_arg(va, int);
                printf("%d", nextInt);
                break;
            case 1:
                nextStr = va_arg(va, char *);
                printf("%s", nextStr);
                break;
        }
        printf(" ");
    }
    printf("\n");
    va_end(va);
}

void readln(int n, ...) {
    if (n == 0){
        return;
    }
    va_list va;
    va_start(va, n);
    char fmt[n * 3];
    // construct format string for vscanf,
    // each argument takes 3 bytes i.e. "%d" followed by either a space or null for the last argument
    for (int i = 0; i < n; i++) {
        strncpy(&fmt[i * 3], "%d ", 3);
    }
    fmt[n * 3 - 1] = '\0';
    // An alternative of scanf that can take a va_list argument
    // ref: scanf manpage(3)
    vscanf(fmt, va);
    va_end(va);
}

#ifdef TEST
int main(int argc, char argv[]) {
    int a, b, c;
    readln(3, &a, &b, &c);
    printf("%d %d %d\n", a, b, c);
}
#endif
