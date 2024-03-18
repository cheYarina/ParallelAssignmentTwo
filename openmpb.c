#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h> 

#define NARRAY 150000
#define NBUCKET 10    
#define INTERVAL 10   
#define MAX_VALUE 100 

struct Node {
    int data;
    struct Node *next;
};

void BucketSort(int arr[]);
struct Node *InsertionSort(struct Node *list);
void print(int arr[]);
void printBuckets(struct Node *list);
int getBucketIndex(int value);


void BucketSort(int arr[]) {
    int i, j;
    struct Node **buckets;

    
    buckets = (struct Node **)malloc(sizeof(struct Node *) * NBUCKET);

    
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

 
    for (i = 0; i < NBUCKET; i++) {
        printf("Bucket[%d]: ", i);
        printBuckets(buckets[i]);
        printf("\n");
    }

 
    #pragma omp parallel for private(i)
    for (i = 0; i < NBUCKET; ++i) {
        buckets[i] = InsertionSort(buckets[i]);
    }


    for (j = 0, i = 0; i < NBUCKET; ++i) {
        struct Node *node;
        node = buckets[i];
        while (node) {
            arr[j++] = node->data;
            node = node->next;
        }
    }


    for (i = 0; i < NBUCKET; ++i) {
        struct Node* current = buckets[i];
        while (current != NULL) {
            struct Node* next = current->next;
            free(current);
            current = next;
        }
    }
    free(buckets);
}


struct Node *InsertionSort(struct Node *list) {
    struct Node *k, *nodeList;
    if (list == 0 || list->next == 0) {
        return list;
    }

    nodeList = list;
    k = list->next;
    nodeList->next = 0;
    while (k != 0) {
        struct Node *ptr;
      
        if (nodeList->data > k->data) {
            struct Node *tmp = k;
            k = k->next;
            tmp->next = nodeList;
            nodeList = tmp;
            continue;
        }

        for (ptr = nodeList; ptr->next != 0; ptr = ptr->next) {
            if (ptr->next->data > k->data)
                break;
        }

        struct Node *tmp = k;
        k = k->next;
        tmp->next = ptr->next;
        ptr->next = tmp;
    }
    return nodeList;
}

int getBucketIndex(int value) {
    return value / INTERVAL;
}

void print(int arr[]) {
    int i;
    for (i = 0; i < NARRAY; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}


void printBuckets(struct Node *list) {
    struct Node *cur = list;
    while (cur) {
        printf("%d ", cur->data);
        cur = cur->next;
    }
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
    clock_t end = clock(); =

    printf("-------------\n");
    printf("Sorted array: ");
    print(array);

    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC; 
    printf("Execution time: %f seconds\n", time_taken);

    return 0;
}
