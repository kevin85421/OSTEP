#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void process(char* path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        printf("wunzip: cannot open file\n");
        exit(1);
    }

    char bytes[5];
    int index = 0;
    do {
        bytes[index] = fgetc(fp);
        if( feof(fp) ) {
            break ;
        }

        index ++;
        if (index == 5) {
            int n = (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
            char ch = bytes[4];
            for (int i=0; i < n; i++) {
                fwrite(&ch , sizeof(char), 1, stdout);
            }
            index = 0;
        }
    } while(1);

    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }

    // Process the first file
    for (int i=1; i < argc; i++) {
        process(argv[i]);
    }
    return 0;
}