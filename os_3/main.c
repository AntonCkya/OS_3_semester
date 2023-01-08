#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// gcc -o a.out main.c -lpthread

int array[1000000];

typedef struct {
    int left;
    int right;
} left_right;

typedef struct {
    int left;
    int mid;
    int right;
} left_mid_right;

void merge(int left, int mid, int right){
    //printf("merged: %d %d %d\n", left, mid, right);
    int it1 = left, it2 = mid + 1;
    int res[1000000];
    for (int i = left; i <= right; i++){
        res[i] = array[i];
    }
    for (int i = left; i <= right; i++){
        if (it1 > mid){
            array[i] = res[it2];
            it2++;
        } else if (it2 > right){
            array[i] = res[it1];
            it1++;
        } else if (res[it2] < res[it1]){
            array[i] = res[it2];
            it2++;
        } else {
            array[i] = res[it1];
            it1++;
        }
    }
}

void merge_sort(int left, int right){
    if (left >= right)
        return;
    int mid = (left + right) / 2;
    merge_sort(left, mid);
    merge_sort(mid + 1, right);
    merge(left, mid, right);
}

void * mt_merge_sort(void * args){
    left_right *lr = args;
    int left = lr->left;
    int right = lr->right;
    merge_sort(left, right);
    pthread_exit(NULL);
}

void * mt_merge(void * args){
    left_mid_right *lmr = args;
    int left = lmr->left;
    int right = lmr->right;
    int mid = lmr->mid;
    merge(left, mid, right);
    pthread_exit(NULL);
}

int main(int argc, char * argv[]){

    srand(time(NULL));
    //printf("randomized array:\n");

    long array_size = atol(argv[1]);
    int thread_count = atoi(argv[2]);

    for (int i = 0; i < array_size; i++){
        array[i] = 1 + rand() % 1000;
    }
    /*
    for (int i = 0; i < array_size; i++){
        printf("%d ", array[i]);
    }
    printf("\n\n"); */

    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));
    left_right *lr_threads = malloc(thread_count * sizeof(left_right));
    left_mid_right *lmr_threads = malloc(thread_count * sizeof(left_mid_right));

    for (int i = 0; i < thread_count; i++) {
        int left = i * (array_size / thread_count);
        int right = (i + 1) * (array_size / thread_count);
        if (i == thread_count - 1){
            right = array_size;
        }
        lr_threads[i].left = left;
        lr_threads[i].right = right - 1;
        pthread_create(&threads[i], NULL, mt_merge_sort, &(lr_threads[i]));
    }
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    /*
    for (int i = 0; i < thread_count; i++) {
        printf("%d %d\n", lr_threads[i].left, lr_threads[i].right);
    }
    printf("\nsemi-sorted array: \n");
    for (int i = 0; i < array_size; i++){
        printf("%d ", array[i]);
    }
    printf("\n");
    */

    while (thread_count != 1){
        if (thread_count == 2){
            break;
        }
        thread_count = thread_count / 2;
        for (int i = 0; i < thread_count; i++) {
            int left = lr_threads[2 * i].left;
            int right = lr_threads[2 * i + 1].right;
            int mid = lr_threads[2 * i].right;
            lr_threads[i].left = left;
            lr_threads[i].right = right;
            left_mid_right lmr;
            lmr_threads[i].left = left;
            lmr_threads[i].mid = mid;
            lmr_threads[i].right = right - 1;
            //printf("lmr: %d %d %d\n", lmr.left, lmr.mid, lmr.right);
            pthread_create(&threads[i], NULL, mt_merge, &(lmr_threads[i]));
        }
        for (int i = 0; i < thread_count; i++) {
            pthread_join(threads[i], NULL);
        }
    }

    merge(lr_threads[0].left, lr_threads[0].right, array_size - 1);

    free(threads);
    free(lr_threads);
    free(lmr_threads);
    
    /*
    printf("\nsorted array: \n");
    for (int i = 0; i < array_size; i++){
        if (array[i] != 0){
            printf("%d ", array[i]);
        }
    }
    printf("\n");
    */

    return 0;
}