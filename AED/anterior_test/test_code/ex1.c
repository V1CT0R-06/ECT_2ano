

void SelectionSort(int* a, unsigned int n) {
    unsigned int i, j, min;
    int tmp;

    for (i = 0; i + 1 < n; i++) {
        min = i;

        for (j = i + 1; j < n; j++) {
            if (a[j] < a[min]) {
                min = j;
            }
        }

        if (min != i) {
            tmp = a[i];
            a[i] = a[min];
            a[min] = tmp;
        }
    }
}

#include <stdio.h>

int main() {
    int a[] = {6, 5, 4, 3, 2, 1};
    unsigned int n = 6;

    SelectionSort(a, n);

    for (unsigned int i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");

    return 0;
}
