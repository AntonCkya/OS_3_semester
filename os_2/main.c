#include <stdio.h>
#include <unistd.h>
#include <string.h>

int fd1[2], fd2[2];
int pipe1, pipe2;

int main (int argc, char *argv[]){

    int arg_len = strlen(argv[1]); 

    if (argc != 2){
		printf("The number of arguments is different than required\n");
		return 2;
	}
    if (arg_len > 255){
        printf("The len of the file is higher than required\n");
		return 3;
    }

    pipe1 = pipe(fd1);
    if (pipe1 == -1){
        printf("Pipe1 opening error\n");
		return 4;
    }
    pipe2 = pipe(fd2);
    if (pipe2 == -1){
        printf("Pipe2 opening error\n");
		return 5;
    }

    int id = fork();

    if (id > 0){
        
        if (close(fd1[0]) == -1){
            printf("Pipe1 (read) closing error\n");
		    return 6;
        }
        if (close(fd2[1]) == -1){
            printf("Pipe2 (write) closing error\n");
		    return 7;
        }

        int message;
        while (scanf("%d", &message) == 1){
			if (write(fd1[1], &message, sizeof(message)) == -1){
                printf("Error while writing to Pipe1\n");
		        return 8;
            }
        }
        write(fd1[1], "\n", sizeof(char));
        int res;
        if (read(fd2[0], &res, sizeof(res)) == -1){
            printf("Error while reading from Pipe2\n");
		    return 9;
        }

        if (res == 2){
            printf("Division by zero exception\n");
		    return 10;
        } else if (res != 0){
            printf("Something wrong in child process\n");
		    return 11;
        }
        printf("i'm fine\n");

        if (close(fd1[1]) == -1){
            printf("Pipe1 (write) closing error\n");
		    return 12;
        }
        if (close(fd2[0]) == -1){
            printf("Pipe2 (read) closing error\n");
		    return 13;
        }

    } else if (id == 0){      

        if (dup2(fd1[0], 0) == -1){
            printf("Error in dub2 Pipe1 (read)");
            return 14;
        }

        if (dup2(fd2[1], 1) == -1){
            printf("Error in dub2 Pipe2 (write)");
            return 15;
        }

        if (close(fd1[1]) == -1){
            printf("Pipe1 (write) closing error\n");
		    return 16;
        }

        if (close(fd2[0]) == -1){
            printf("Pipe2 (read) closing error\n");
		    return 17;
        }

        // execl execlp execv execvp
        if (execl("b.out", "b.out", argv[1], (char *)NULL) == -1){
            printf("Error in executing child.c\n");
            return 18;
        }


    } else {
        printf("Fork raise error\n");
		return 19;
    }
    return 0;
}
