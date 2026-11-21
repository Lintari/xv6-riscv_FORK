#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

struct Node {
    int data;
    struct Node *next;
    char padding[20]; // Немного увеличим размер, чтобы попасть в разные корзины (bins)
};

int main(void) {
    printf("=== Starting Dynamic Allocation Test ===\n");
    print_allocator_state();

    struct Node *head = 0;
    
    // 1. Выделяем 5 узлов (проверка попадания в разные bins и выделения новых страниц)
    printf("\n--- Allocating 5 nodes ---\n");
    for (int i = 0; i < 5; i++) {
        struct Node *n = malloc(sizeof(struct Node));
        if (!n) {
            printf("Malloc failed!\n");
            exit(1);
        }
        n->data = i;
        n->next = head;
        head = n;
        printf("Allocated node with data: %d at %p\n", i, n);
    }

    // 2. Освобождаем 3 узла (проверка возврата блоков в free list и возможного shrink кучи)
    printf("\n--- Freeing 3 nodes ---\n");
    for (int i = 0; i < 3; i++) {
        struct Node *tmp = head;
        head = head->next;
        printf("Freeing node with data: %d\n", tmp->data);
        free(tmp);
    }

    // 3. Выделяем большой блок (проверка large_malloc)
    printf("\n--- Allocating large block (5000 bytes) ---\n");
    char *large_buf = malloc(5000);
    if (large_buf) {
        printf("Allocated large buffer at %p\n", large_buf);
    }

    // 4. Освобождаем всё (проверка полного возврата памяти ОС через sbrk)
    printf("\n--- Freeing remaining nodes and large block ---\n");
    free(large_buf);
    while (head) {
        struct Node *tmp = head;
        head = head->next;
        free(tmp);
    }

    printf("\n=== Test Completed ===\n");
    print_allocator_state();
    exit(0);
}
