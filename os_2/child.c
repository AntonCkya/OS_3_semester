#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

    int message, return_code = 0, count = 1;
    double result;
    while (read(0, &message, sizeof(message)) == sizeof(message)){

		if (count > 1 && message == 0){
            return_code = 2;
            write(1, &return_code, sizeof(return_code));
        }

        if (count == 1){
            result = message;
            count++;
            continue;
        }

        result = result / message;
        count++;
	}
    if (return_code == 2){
        write(1, &return_code, sizeof(return_code));
        return 0;
    }
    write(1, &return_code, sizeof(return_code));

    FILE *fout = fopen(argv[1], "w");
    fprintf(fout, "%lf\n", result);
    fclose(fout);

    return 0;
}
