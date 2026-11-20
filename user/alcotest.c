#include "kernel/types.h"
#include "user/user.h"

#define PGSIZE 4096

int
main(void)
{
    void *a, *b, *c;
    void *tmp;

    printf("\n=== TEST 1: Ordinary allocation ===\n");

    a = malloc(100);

    if (a == 0) {
        printf("FAIL: malloc(100)\n");
        exit(1);
    }

    printf("Allocated: %p\n", a);

    free(a);

    tmp = malloc(100);

    if (tmp != a) {
        printf("FAIL: ordinary block was not reused\n");
        exit(1);
    }

    printf("PASS: ordinary block reused\n");
    free(tmp);


    printf("\n=== TEST 2: Large allocation ===\n");

    a = malloc(5000);

    if (a == 0) {
        printf("FAIL: malloc(5000)\n");
        exit(1);
    }

    // Записываем данные во всю область.
    for (int i = 0; i < 5000; i++)
        ((char *)a)[i] = 'A';

    for (int i = 0; i < 5000; i++) {
        if (((char *)a)[i] != 'A') {
            printf("FAIL: corrupted large allocation\n");
            exit(1);
        }
    }

    printf("PASS: large allocation read/write\n");
    

    free(a);

    
    printf("\nAfter freeing a:\n");
    
    b = malloc(6000);

    if (b != a) {
        printf("FAIL: large allocation was not reused\n");
        exit(1);
    }

    printf("PASS: large allocation reused\n");

   

    free(b);
    printf("\nAfter freeing b:\n");

    printf("\n=== TEST 3: Interior region ===\n");

    // A: большой регион.
    a = malloc(5000);

    // B: ещё один большой регион.
    b = malloc(5000);

    if (a == 0 || b == 0) {
        printf("FAIL: allocation\n");
        exit(1);
    }

    // Освобождаем A, но B всё ещё находится выше.
    free(a);

    // A не должна быть возвращена ядру.
    // Проверяем, что её можно повторно использовать.
    tmp = malloc(4000);

    if (tmp != a) {
        printf("FAIL: interior region was not reusable\n");
        exit(1);
    }

    printf("PASS: interior region retained\n");

    free(tmp);
    free(b);


    printf("\n=== TEST 4: Cascading release ===\n");

    // Получаем два больших региона.
    a = malloc(5000);
    b = malloc(5000);

    if (a == 0 || b == 0) {
        printf("FAIL: allocation\n");
        exit(1);
    }


    // Освобождаем нижний регион.
    free(a);

    // Верхний регион пока занят.
    // Освобождение b должно позволить вернуть оба региона.
    free(b);

    // Теперь выделяем два новых региона.
    c = malloc(5000);
    tmp = malloc(5000);

    if (c == 0 || tmp == 0) {
        printf("FAIL: allocation after cascading release\n");
        exit(1);
    }

    printf("PASS: allocation after cascading release\n");

    free(c);
    free(tmp);


    printf("\n=== ALL TESTS PASSED ===\n");

    exit(0);
}
