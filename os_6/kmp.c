
static int str_size (const char* str){
    int i = 0;
    while(str[i] != '\0'){
        i++;
    }
    return i;
}

static void prefix (const char* sub, int *prefix, int sub_size){
    prefix[0] = 0;
    int i = 1, j = 0;
    while (i != sub_size){
        if (sub[i] != sub[j]){
            if (j == 0){
                prefix[i] = 0;
                i++;
            } else {
                j = prefix[j - 1];
            }
        } else {
            prefix[i] = j + 1;
            i++;
            j++;
        }
    }
}
int KMP (const char* str, const char* sub){
    int strSize = str_size(str);
    int subSize = str_size(sub);
    int p[512];
    prefix(sub, p, subSize);
    int j = 0, i = 0;
    while (i != strSize){

        if (str[i] == sub[j]){
            i++;
            j++;
        }
        if (j == subSize){
            return i - subSize;
        }
        else if (str[i] != sub[j] && j != 0){
            j = p[j - 1];
        } else if (str[i] != sub[j] && j == 0){
            i++;
        }
    }
    return -1;
}
