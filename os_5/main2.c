#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include "lib.h"

typedef float (*Derivative_function)(float A , float deltaX);
typedef int (*PrimeCount_function)(int A, int B);
const char lib1_path[] = "./libmylib1x.so";
const char lib2_path[] = "./libmylib2x.so";

void change_handle(void ** lib_handle, int current_lib){
    if (current_lib == 0){
        *lib_handle = dlopen(lib1_path, RTLD_NOW);
    } else {
        *lib_handle = dlopen(lib2_path, RTLD_NOW);
    }
}

void change_function(Derivative_function *Derivative, PrimeCount_function *PrimeCount, void* lib_handle){
    *Derivative = dlsym(lib_handle, "Derivative");
    *PrimeCount = dlsym(lib_handle, "PrimeCount");
}

void call_error(){
    printf("exception during command processing\n");
}

int main(){
    int command, current_lib = 0;
    void * lib_handle;
    Derivative_function Derivative;
    PrimeCount_function PrimeCount;
    change_handle(&lib_handle, current_lib);
    change_function(&Derivative, &PrimeCount, lib_handle);
    while(scanf("%d", &command) != EOF){
        switch(command){
            case 0:
                printf("Called library changing\n");
                dlclose(lib_handle);
                current_lib = (current_lib + 1) % 2;
                change_handle(&lib_handle, current_lib);
                change_function(&Derivative, &PrimeCount, lib_handle);
                break;
            case 1:
                printf("Called Derivative function\n");
                float fA , deltaX;
                scanf("%f %f", &fA, &deltaX);
                printf("Result: %f\n", Derivative(fA, deltaX));
                break;
            case 2:
                printf("Called PrimeCount function\n");
                int A, B;
                scanf("%d %d", &A, &B);
                printf("Result: %d\n", PrimeCount(A, B));
                break;
            default:
                printf("%d\n", command);
                call_error();
        }
    }
    return 0;
}