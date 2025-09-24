#include <stdio.h>

void printArray(const char *s, int a[], int n) {
    printf("%s:\n", s);
    for (int i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

void cumSum(int a[], int b[], int n) {
    int c = 0;
    for (int i = 0; i < n; i++) {
        c += a[i];
        b[i] = c;
    }
}

int main() {
    int a[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int b[12];

    printArray("a", a, 12);

    cumSum(a, b, 12);

    printArray("b", b, 12);

    return 0;
}
