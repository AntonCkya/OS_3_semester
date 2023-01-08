#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#define True 1

int fdAB, fdAC;
/*  A ---string---> C
    A ---number---> B  */

int main(){

    printf("Programm A started\n");

    fdAB = open("tempAB", O_RDWR | O_CREAT | O_TRUNC, 0777);
    fdAC = open("tempAC", O_RDWR | O_CREAT | O_TRUNC, 0777);

    if (fdAB < 0 || fdAC < 0){
		printf("Error while opening a file\n");
		return 2;
	}

    ftruncate(fdAB, 2 * sizeof(int));
    ftruncate(fdAC, 1025 * sizeof(char));

    int *ptrAB;
    char *ptrAC;
    ptrAB = mmap(NULL, 2 * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fdAB, 0);
    ptrAC = mmap(NULL, 1025 * sizeof(char), PROT_WRITE | PROT_READ, MAP_SHARED, fdAC, 0);
    /*
    ptr[0] = 0 - готово к записи
    ptr[0] = 1 - запись произошла, можно читать
    ptr[0] = 2 - запись окончена, a.k.a. данных больше не будет
    */
    if (ptrAB == MAP_FAILED || ptrAC == MAP_FAILED){
        printf("Error while memory map created\n");
        return 3;
	}

    ptrAB[0] = 0;
    ptrAC[0] = 0;

    int id1 = fork();

    if (id1 < 0){
        printf("Fork raise error\n");
        return 4;
    } else if (id1 == 0){

        if (execl("b.out", "b.out", NULL, (char *)NULL) == -1){
            printf("Error in executing b.c\n");
            return 5;
        }

    } else {

        int id2 = fork();

        if (id2 < 0){
            printf("Fork raise error\n");
            return 4;
        } else if (id2 == 0){
            
            if (execl("c.out", "c.out", NULL, (char *)NULL) == -1){
                printf("Error in executing c.c\n");
                return 6;
            }

        } else {

            char c;
            int count = 0, reading;

            while (True){
                while (ptrAC[0] == 1){
                    // активное ожидание
                }
                while (ptrAB[0] == 1){
                    // активное ожидание
                }
                reading = scanf("%c", &c);
                if (reading > 0){
                    if (c != '\n'){
                        ptrAC[count + 1] = c;
                        count++;
                    } else {
                        ptrAC[count + 1] = c;
                        ptrAC[0] = 1;

                        ptrAB[1] = count;
                        ptrAB[0] = 1;

                        count = 0;
                    }
                } else {
                    ptrAB[0] = 2;
                    ptrAC[0] = 2;
                    break;
                }
            }

            if (close(fdAB) == -1 || close(fdAC) == -1){
		        printf("Error while closing a files\n");
		        return 17;
            }

            munmap(ptrAB, 2 * sizeof(int));
            munmap(ptrAC, 1025 * sizeof(char));

            printf("Programm stopped\n");

        }

    }

    return 0;

}