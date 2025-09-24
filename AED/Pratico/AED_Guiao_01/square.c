#include <stdio.h>
#include <math.h>

int main() {
    int n, i;

    printf("Quantas linhas tem a tabela? ");
    scanf("%d", &n);

    printf("%-10s %-10s %-10s\n", "Numero", "Quadrado", "Raiz");
    printf("----------------------------------------\n");

    for (i = 1; i <= n; i++) {
        printf("%-10d %-10d %-10.4f\n", i, i*i, sqrt(i));
    }

    return 0;
}
