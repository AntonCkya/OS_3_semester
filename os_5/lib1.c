#include <math.h>

float Derivative(float A, float deltaX){
    return (cos(A + deltaX) - cos(A)) / deltaX;
} 

int PrimeCount(int A, int B){
    int prime_count = 0;
    for (int i = A; i <= B; i++){
        int is_prime = 1;
        for (int j = 2; j < i; j++){
            if (i % j == 0)
                is_prime = 0;
        }
        if (i < 2)
            is_prime = 0;
        prime_count += is_prime;
    }
    return prime_count;
}
