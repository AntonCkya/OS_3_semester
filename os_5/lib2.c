#include <math.h>
#include <stdlib.h>

float Derivative(float A, float deltaX){
    return (cos(A + deltaX) - cos(A - deltaX)) / (2 * deltaX);
}

int PrimeCount(int A, int B){
    int prime_count = 0;
    //int * arr = (int *) malloc(B * sizeof(int));
    int arr[10000];
    for (int i = 0; i <= B; i++){
        arr[i] = i;
    }
    arr[1] = 0;
    for (int i = 2; i <= B; i++){
        if (arr[i] != 0){
            for (int j = i*i; j <= B; j += i){
                arr[j] = 0;
            }
            if(i >= A)
                prime_count++;
        }
    }
    //free(arr);
    return prime_count;
}
