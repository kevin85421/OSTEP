#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// For MacOS
#include <sys/sysctl.h>

#define DEBUGx

#ifdef DEBUG
# define DBG_PRINTF(...) printf(__VA_ARGS__)
# define FW(...) 
#else
# define DBG_PRINTF(...)
# define FW(...) fwrite(__VA_ARGS__) 
#endif
// Ref: ostep-code include/common_threads.h
#define Pthread_create(thread, attr, start_routine, arg)                       \
  assert(pthread_create(thread, attr, start_routine, arg) == 0);
#define Pthread_join(thread, value_ptr)                                        \
  assert(pthread_join(thread, value_ptr) == 0);

static int nproc;

int getNumberOfCores() {
    int nm[2];
    size_t len = 4;
    uint32_t count;

    nm[0] = CTL_HW;
    nm[1] = HW_AVAILCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);

    if (count < 1) {
        nm[1] = HW_NCPU;
        sysctl(nm, 2, &count, &len, NULL, 0);
        if (count < 1) {
        count = 1;
        }
    }
    return count;
}

typedef struct zip_arg_t {
    char *ptr;
    int offset;
    int len;
} zip_arg_t;

typedef struct zip_ret_t {
    char   val[10000000];
    int    num[10000000];
    int    n;
} zip_ret_t;

void *thread_zip(void *args) {
    zip_ret_t* ret = malloc(sizeof(zip_ret_t));
    ret->n = 0;

    char *ptr = ((zip_arg_t *)args)->ptr;
    int offset = ((zip_arg_t *)args)->offset;
    int len = ((zip_arg_t *)args)->len;
    char ch = *(ptr + offset);
    int nch = 0;

    for (int i = 0; i < len; i++) {
        if (*(ptr + offset + i) == ch) {
            nch++;
        } else {
            ret->val[ret->n] = ch;
            ret->num[ret->n] = nch;
            ret->n ++;
            nch = 1;
            ch = *(ptr + offset + i);
            assert(ret->n < 10000000);
        }
    }

    ret->val[ret->n] = ch;
    ret->num[ret->n] = nch; 

    return (void*)ret;
}

void merge(char* ch, int* nch, zip_ret_t* rets[], int nthread, bool first) {
    if (first) {
        (*ch) = rets[0]->val[0];
        (*nch) = 0; 
    }
    
    for (int i=0; i < nthread; i++) {
        for (int j = 0; j <= rets[i]->n; j++) {
            if ((*ch) == rets[i]->val[j]) {
                (*nch) += rets[i]->num[j]; 
            } else {
                DBG_PRINTF("write[num: %d, val: %c]\n", (*nch), (*ch));
                FW(nch, sizeof(int), 1 ,stdout);
                FW(ch, sizeof(char), 1 ,stdout);
                (*ch) = rets[i]->val[j];
                (*nch) = rets[i]->num[j];
            }
        }
    }
}

void process(char* ch, int* nch, char *path, bool first) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("wzip: cannot open file\n");
        exit(1);
    }

    struct stat st;
    int err = fstat(fd, &st);
    if (err < 0) {
        printf("fstat fails\n");
        exit(1);
    }

    int filesize = st.st_size;
    int offset = 0;
    char *ptr = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, offset);
    if (ptr == MAP_FAILED) {
        printf("mmap fails\n");
        exit(1);
    }

    int nthread = getNumberOfCores();
    int len = 0;
    pthread_t threads[nthread];
    zip_arg_t args[nthread];

    for (int i = 0; i < nthread; i++) {
        len = ((i+1) * filesize / nthread) - offset;
        args[i] = (struct zip_arg_t){ptr, offset, len};
        Pthread_create(&threads[i], NULL, thread_zip, &args[i]);
        offset += len;
    }

    zip_ret_t* rets[nthread];
    for (int i = 0; i < nthread; i++) {
        Pthread_join(threads[i], (void*)(&rets[i]));
    }

    merge(ch, nch, rets, nthread, first);

    for (int i = 0; i < nthread; i++) {
        free(rets[i]);
    }

    err = munmap(ptr, filesize);
    if (err != 0) {
        printf("munmap fails\n");
        exit(1);
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    nproc = getNumberOfCores();
    // Process the first file
    char ch;
    int nch = 0;
    process(&ch, &nch, argv[1], true);

    for (int i = 2; i < argc; i++) {
        process(&ch, &nch, argv[i], false);
    }
    
    DBG_PRINTF("write[num: %d, val: %c]\n", nch, ch);
    FW(&nch, sizeof(int), 1 ,stdout);
    FW(&ch, sizeof(char), 1 ,stdout);
    return 0;
}