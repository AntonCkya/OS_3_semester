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

int main (int argc, char *argv[]){

    if (argc != 2){
		printf("The number of arguments is different than required\n");
		return 2;
	}

    int arg_len = strlen(argv[1]); 
    
    if (arg_len > 255){
        printf("The len of the file is higher than required\n");
		return 3;
    }

    fd = open("temp", O_RDWR | O_CREAT | O_TRUNC, 0777);
    /*
    O_RDWR - режим доступа на чтение и запись
    O_CREAT - файл будет создан если не существует
    O_TRUNC - при открытии файл отчистится
    0777 (a.k.a. S_IRWXU | S_IRWXG | S_IRWXO)
    */
    if (fd < 0){
		printf("Error while opening a file\n");
		return 4;
	}

    ftruncate(fd, 2 * sizeof(int)); // изменение размера файла с файловым дескриптором fd

    int *ptr;
    ptr = mmap(NULL, 2 * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0); // отражение на память
    /*
    mmap(void *start, size_t length, int prot , int flags, int fd, off_t offset);
    PROT_WRITE - данные можно читать
    PROT_READ - в данную область можно записывать
    MAP_SHARED - эту область могут использовать другие процессы
    */
    if (ptr == MAP_FAILED){
		printf("Error while memory map created\n");
		return 5;
	}
    ptr[0] = 0;
    /*
    ptr[0] = 0 - готово к записи
    ptr[0] = 1 - запись произошла, можно читать
    ptr[0] = 2 - запись окончена, a.k.a. данных больше не будет
    ptr[0] = 3 - в дочернем процессе произошло деление на 0
    ptr[0] = 4 - другие исключения в работе дочернего процесса
    */
    
    int id = fork();

    if (id > 0){
        int message;
        int reading;

        while (True){
            while (ptr[0] == 1){
                // активное ожидание
            }
            reading = scanf("%d", &message);

            printf("Readed in main.c: %d\n", message);
            
            if (reading > 0){
                ptr[1] = message;
                if (ptr[0] == 3){
                    break;
                }
                ptr[0] = 1;
            } else {
                if (ptr[0] == 3){
                    break;
                }
                ptr[0] = 2;
                break;
            }
            if (ptr[0] == 3){
                break;
            }
        }

        printf("Reading ended in main.c\n");

        if (ptr[0] == 3){
            printf("Division by zero\n");
		    return 0;
        }

        if (close(fd) == -1){
		    printf("Error while closing a file\n");
		    return 17;
        }

        munmap(ptr, 2*sizeof(int));

    } else if (id == 0){
        printf("Forking success in main.c\n");

        if (execl("c.out", "c.out", argv[1], (char *)NULL) == -1){
            printf("Error in executing child.c\n");
            return 18;
        }

    } else {
        printf("Fork raise error\n");
		return 19;
    }
    return 0;
}
