#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NARRAY 100  
#define NBUCKET 10     
#define INTERVAL 10    
#define MAX_VALUE 100  

struct Node {
    int data;
    struct Node *next;
};

typedef struct {
    struct Node *list;
} ThreadArg;

void BucketSort(int arr[]);
struct Node *InsertionSort(struct Node *list);
void print(int arr[]);
void printBuckets(struct Node *list);
int getBucketIndex(int value);
void *sort_bucket(void *arg);


void BucketSort(int arr[]) {
    int i, j;
    struct Node **buckets;
    ThreadArg *args;

    buckets = (struct Node **)malloc(sizeof(struct Node *) * NBUCKET);
    args = (ThreadArg *)malloc(sizeof(ThreadArg) * NBUCKET);

    for (i = 0; i < NBUCKET; ++i) {
        buckets[i] = NULL;
    }

    for (i = 0; i < NARRAY; ++i) {
        struct Node *current;
        int pos = getBucketIndex(arr[i]);
        current = (struct Node *)malloc(sizeof(struct Node));
        current->data = arr[i];
        current->next = buckets[pos];
        buckets[pos] = current;
    }

    pthread_t threads[NBUCKET];
    for (i = 0; i < NBUCKET; ++i) {
        args[i].list = buckets[i];
        if (pthread_create(&threads[i], NULL, sort_bucket, (void *)&args[i])) {
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }

    for (i = 0; i < NBUCKET; ++i) {
        if (pthread_join(threads[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            exit(1);
        }
    }

    for (j = 0, i = 0; i < NBUCKET; ++i) {
        struct Node *node = args[i].list;
        while (node) {
            arr[j++] = node->data;
            struct Node *temp = node;
            node = node->next;
            free(temp);
        }
    }

    free(buckets);
    free(args);
}

struct Node *InsertionSort(struct Node *list) {
    if (list == NULL || list->next == NULL) {
        return list;
    }

    struct Node *sorted = NULL;
    struct Node *current = list;
    while (current != NULL) {
        struct Node *next = current->next;
        struct Node **indirect = &sorted;

        while (*indirect != NULL && (*indirect)->data < current->data) {
            indirect = &(*indirect)->next;
        }

        current->next = *indirect;
        *indirect = current;
        current = next;
    }
    return sorted;
}

void *sort_bucket(void *arg) {
    ThreadArg *targ = (ThreadArg *)arg;
    targ->list = InsertionSort(targ->list);
    return NULL;
}

int getBucketIndex(int value) {
    return value / INTERVAL;
}

void print(int arr[]) {
    for (int i = 0; i < NARRAY; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(void) {
    int array[NARRAY];
    srand(time(NULL)); 

    for (int i = 0; i < NARRAY; i++) {
        array[i] = rand() % MAX_VALUE;
    }

    printf("Initial array: ");
    print(array);
    printf("-------------\n");

    clock_t start = clock(); 
    BucketSort(array);
    clock_t end = clock(); 

    printf("-------------\n");
    printf("Sorted array: ");
    print(array);

    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC; 
    printf("Execution time: %f seconds\n", time_taken);

    return 0;
}
