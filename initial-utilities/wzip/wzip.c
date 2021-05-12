#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void process(char* ch, int* nch, char* path, bool first) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        printf("wzip: cannot open file\n");
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    int line_index = 0;

    while ((nread = getline(&line, &len, fp)) != -1) {
        if (first && (line_index == 0)) {
            (*ch) = line[0];
        }

        for (int j=0; j < nread; j++) {
            if (line[j] == (*ch)) {
                (*nch)++;
            } else {
                fwrite(nch, sizeof(int), 1 ,stdout);
                fwrite(ch, sizeof(char), 1 ,stdout);

                (*nch) = 1;
                (*ch) = line[j];
            }
        }

        line_index ++; 
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    char ch;
    int nch = 0;

    // Process the first file
    process(&ch, &nch, argv[1], true);

    for (int i=2; i < argc; i++) {
        process(&ch, &nch, argv[i], false);
    }
    
    fwrite(&nch, sizeof(int), 1 ,stdout);
    fwrite(&ch, sizeof(char), 1 ,stdout);
    return 0;
}