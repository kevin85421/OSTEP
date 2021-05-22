#define CVECTOR_LOGARITHMIC_GROWTH
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "c-vector/cvector.h"

/** for debug **/
static void print_vector(cvector_vector_type(char*) vec) {
    if (vec) {
        char **it;
        int i = 0;
        for (it = cvector_begin(vec); it != cvector_end(vec); ++it) {
            printf("v[%d] = %s / len = %lu \n", i, *it, strlen(*it));
            ++i;
        }
    }
}

static void error_function() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

static cvector_vector_type(char*) parse_cmd(char* line, cvector_vector_type(char*) path, char* first) {
    cvector_vector_type(char*) cmd = NULL;
    line[strlen(line) - 1] = 0; // replace newline char with null
    char* tmp = strtok(line, " \0");
    strcpy(first, tmp);
    char* token = malloc(sizeof(char) * 1024);

    size_t size = cvector_size(path);
    // char exec_path[1024];
    // printf("path:\n");
    // print_vector(path);

    for (size_t i = 0; i < size; i++) {
        strcpy(token, path[i]);
        strcat(token, "/");
        strcat(token, tmp);
        // printf("%s\n", token);
        int fd = access(token, X_OK);
        if (fd != -1) { // no error
            break;
        }
    }

    // printf("exec path: %s\n", token);

    while (token != NULL) {
        cvector_push_back(cmd, token);
        token = strtok(NULL, " \0");
    }

    free(token);

    return cmd;
}

static void exec_cmd(cvector_vector_type(char*) cmd, cvector_vector_type(char*) path) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(0);
    } else if (rc == 0) { // child process (new process) 
        // printf("(cmd: %s) child process (pid: %d)\n", cmd[0], (int)getpid());
        /*
            Important: The second argument of execv is a pointer to an array of pointers to null-terminated character strings. 
            A NULL pointer is used to mark the end of the array.
        */
        cvector_push_back(cmd, NULL); 
        int status_code = execv(cmd[0], cmd);
        if (status_code == -1) {
            error_function();
        }
        exit(0);
    } else { // parent process
        int wc = wait(NULL);
        // printf("(cmd: %s) parent process of %d (wc: %d) (pid: %d)\n", cmd[0], rc, wc, (int)getpid());
    }
}

static void exit_function(cvector_vector_type(char*) cmd, cvector_vector_type(char*) path) {
    size_t size = cvector_size(cmd);
    if (size == 1) { // the last element is null
        // printf("exit_function: (pid: %d)\n", (int)getpid());
        cvector_free(cmd);
        cvector_free(path);
        exit(0);
    }
    error_function();
} 

static void chdir_function(cvector_vector_type(char*) cmd) {
    size_t size = cvector_size(cmd);
    if (size == 2) {
        if (chdir(cmd[1]) == 0) {
            return;
        }
    }
    error_function();
}

static void path_function(cvector_vector_type(char*) cmd, cvector_vector_type(char*) path) {
    size_t new_path_num = cvector_size(cmd) - 1;
    size_t cur_path_num = cvector_size(path);
    while (cur_path_num != 0) {
        cvector_pop_back(path);
        cur_path_num --;
    }

    for (size_t i=0; i < new_path_num; i++) {
        char* tmp = malloc(sizeof(char) * 512);
        strcpy(tmp, cmd[i+1]);
        cvector_push_back(path, tmp);
    }

    assert(cvector_size(path) == new_path_num);
}

int main(int argc, char *argv[]) {
    cvector_vector_type(char*) path = NULL;
    cvector_push_back(path, "/bin");

    // printf("main (pid:%d)\n", (int)getpid());

    if (argc == 1) { // interactive mode
        char buffer[1024];
        char first[1024];
        while (1) {
            printf("wish> ");

            // printf("path:\n");
            // print_vector(path);

            fgets(buffer, sizeof(buffer), stdin);

            // printf("path:\n");
            // print_vector(path);

            cvector_vector_type(char*) cmd = parse_cmd(buffer, path, first);

            // printf("cmd:\n");
            // print_vector(cmd);

            // built-in commands: "exit", "cd", "path"
            if (strcmp(first, "exit") == 0) {
                exit_function(cmd, path);
            } else if (strcmp(first, "cd") == 0) {
                chdir_function(cmd);
            } else if (strcmp(first, "path") == 0) {
                path_function(cmd, path);
                // printf("path:\n");
                // print_vector(path);
            } else {
                // not built-in commands
                // printf("path (before exec_cmd):\n");
                // print_vector(path);
                exec_cmd(cmd, path);
            }
            cvector_free(cmd);
            // printf("path:\n");
            // print_vector(path);
        }
    } else { // batch mode
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            perror("Error opening file");
            exit(1);
        }

        char buffer[1024];
        char first[1024];
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            cvector_vector_type(char*) cmd = parse_cmd(buffer, path, first);

            // built-in commands: "exit", "cd", "path"
            if (strcmp(first, "exit") == 0) {
                exit_function(cmd, path);
            } else if (strcmp(first, "cd") == 0) {
                chdir_function(cmd);
            } else if (strcmp(first, "path") == 0) {
                path_function(cmd, path);
            } else {
                // not built-in commands
                exec_cmd(cmd, path);
            }
            cvector_free(cmd);
        }
    }
}