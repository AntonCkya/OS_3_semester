#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#define True 1

int fd;

int main(int argc, char *argv[]){

    printf("Forking success in child.c\n");

    fd = open("temp", O_RDWR);
    if (fd < 0){
		printf("Error while opening a file in child process\n");
		return 4;
	}

    int *ptr;
    ptr = mmap(NULL, 2 * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED){
		printf("Error while memory map created in child process\n");
		return 5;
	}

    int message, count = 1;
    double result;

    while(True){
        while(ptr[0] == 0){
            // активное ожидание
        }
        if (ptr[0] == 1){
            ptr[0] = 0;
            message = ptr[1];
            printf("Readed in child.c: %d, result = %f\n", message, result);

            if (count > 1 && message == 0){
                ptr[0] = 3;
                printf("Division by zero in child.c\n");
                break;
            }

            if (count == 1){
                result = message;
                count++;
                continue;
            }

            result = result / message;
            count++;
        } else if (ptr[0] == 2){
            FILE *fout = fopen(argv[1], "w");
            printf("Writing to file in child.c, file name = %s\n", argv[1]);
            fprintf(fout, "%f\n", result);
            fclose(fout);
            break;
        }

    }

    printf("Reading ended in child.c, result = %f\n", result);

    if (close(fd) == -1){
		printf("Error while closing a file in child process\n");
		return 17;
    }
    munmap(ptr, 2*sizeof(int));

    return 0;
}