#include "lib.h"
#include <stdio.h>

void call_Derivative(){
    float A, deltaX;
    scanf("%f %f", &A, &deltaX);
    printf("%f\n", Derivative(A, deltaX));
}

void call_PrimeCount(){
    int A, B;
    scanf("%d %d", &A, &B);
    printf("%d\n", PrimeCount(A, B));
}

void call_error(){
    printf("exception during command processing\n");
}

int main(){
    int command;
    while(scanf("%d", &command) != EOF){
        switch(command){
            case 1:
                call_Derivative();
                fflush(stdin);
                break;
            case 2:
                call_PrimeCount();
                fflush(stdin);
                break;
            default:
                call_error();
                fflush(stdin);
        }
    }
    return 0;
}
