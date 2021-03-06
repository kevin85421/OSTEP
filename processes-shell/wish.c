#define CVECTOR_LOGARITHMIC_GROWTH
#include "c-vector/cvector.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/** for debug **/
static void print_vector(cvector_vector_type(char *) vec) {
  if (vec) {
    char **it;
    int i = 0;
    for (it = cvector_begin(vec); it != cvector_end(vec); ++it) {
      printf("v[%d] = %s| len = %lu \n", i, *it, strlen(*it));
      ++i;
    }
  }
}

static void print_prompt(bool interactive) {
  if (interactive) {
    printf("wish> ");
  }
}

static void error_function() {
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

static bool redirect_function(char *line, char **filename) {
  if (strchr(line, '>') == NULL) {
    return false;
  }

  char *before = strtok(line, ">"); // "before" is assigned to "line"
  char *after = strtok(NULL, "\n");
  if (after == NULL) {
    return true;
  }

  // More than 1 '>'
  if (strchr(after, '>') != NULL) {
    return true;
  }

  // multiple files or no file
  // Ex: (1) ls > output1 output2 (2) ls >
  int filenum = 0;
  char *token = strtok(after, " \0");

  if (token != NULL) {
    (*filename) = malloc(sizeof(char) * strlen(token));
    strcpy(*filename, token);
  }

  while (token != NULL) {
    filenum++;
    token = strtok(NULL, " \0");
  }

  if (filenum != 1) {
    free(*filename);
    (*filename) = NULL;
    return true;
  }

  return true;
}

static cvector_vector_type(char *)
    parse_cmd(char *line, cvector_vector_type(char *) path, char *first) {
  cvector_vector_type(char *) cmd = NULL;
  if (line[strlen(line) - 1] == '\n') {
    line[strlen(line) - 1] = 0; // replace newline char with null
  }

  char *tmp = strtok(line, " \0");
  if (tmp == NULL) { // A string only has spaces. Example: "    "
    return cmd;
  }

  strcpy(first, tmp);
  char *token = malloc(sizeof(char) * 1024);
  size_t size = cvector_size(path);

  for (size_t i = 0; i < size; i++) {
    strcpy(token, path[i]);
    strcat(token, "/");
    strcat(token, tmp);
    int fd = access(token, X_OK);
    if (fd != -1) { // no error
      break;
    }
  }

  while (token != NULL) {
    cvector_push_back(cmd, token);
    token = strtok(NULL, " \0");
  }

  free(token);
  return cmd;
}

static cvector_vector_type(char *)
    parallel_command(char *line, bool *parallel) {
  cvector_vector_type(char *) cmdlines = NULL;

  if (strchr(line, '&') != NULL) {
    (*parallel) = true;
  }

  if (line[strlen(line) - 1] == '\n') {
    line[strlen(line) - 1] = 0; // replace newline char with null
  }

  char *token = strtok(line, "&"); // line will be modified
  while (token != NULL) {
    cvector_push_back(cmdlines, token);
    token = strtok(NULL, "&");
  }

  free(token);
  return cmdlines;
}

static void exec_cmd(cvector_vector_type(char *) cmd,
                     cvector_vector_type(char *) path, bool is_redirect,
                     char *redirect_output, bool parallel) {
  int rc = fork();
  if (rc < 0) {
    fprintf(stderr, "fork failed\n");
    exit(0);
  } else if (rc == 0) { // child process (new process)
    /*
        Important: The second argument of execv is a pointer to an array of
       pointers to null-terminated character strings. A NULL pointer is used to
       mark the end of the array.
    */
    cvector_push_back(cmd, NULL);
    int fd = 0;
    if (is_redirect) {
      int fd = open(redirect_output, O_WRONLY | O_TRUNC | O_CREAT, 0666);
      dup2(fd, STDOUT_FILENO);
    }

    int status_code = execv(cmd[0], cmd);
    if (status_code == -1) {
      error_function();
    }

    if (is_redirect) {
      close(fd);
    }

    exit(0);
  } else { // parent process
    if (!parallel) {
      int wc = wait(NULL);
    }
  }
}

static void exit_function(cvector_vector_type(char *) cmd,
                          cvector_vector_type(char *) path) {
  size_t size = cvector_size(cmd);
  if (size == 1) { // the last element is null
    cvector_free(cmd);
    cvector_free(path);
    exit(0);
  }
  error_function();
}

static void chdir_function(cvector_vector_type(char *) cmd) {
  size_t size = cvector_size(cmd);
  if (size == 2) {
    if (chdir(cmd[1]) == 0) {
      return;
    }
  }
  error_function();
}

static void path_function(cvector_vector_type(char *) cmd,
                          cvector_vector_type(char *) path) {
  size_t new_path_num = cvector_size(cmd) - 1;
  size_t cur_path_num = cvector_size(path);
  while (cur_path_num != 0) {
    cvector_pop_back(path);
    cur_path_num--;
  }

  for (size_t i = 0; i < new_path_num; i++) {
    char *tmp = malloc(sizeof(char) * 512);
    strcpy(tmp, cmd[i + 1]);
    cvector_push_back(path, tmp);
  }

  assert(cvector_size(path) == new_path_num);
}

int main(int argc, char *argv[]) {
  if (argc > 2) { // More than 1 input file
    error_function();
    exit(1);
  }

  cvector_vector_type(char *) path = NULL;
  cvector_push_back(path, "/bin");
  bool interactive = false;

  FILE *input = NULL;
  if (argc == 1) { // interactive mode
    interactive = true;
    input = stdin;
  } else if (argc == 2) { // batch mode
    input = fopen(argv[1], "r");
    if (input == NULL) {
      error_function();
      exit(1);
    }
  }

  char buffer[1024];
  char first[1024];
  print_prompt(interactive);
  while (fgets(buffer, sizeof(buffer), input) != NULL) {
    print_prompt(interactive);
    bool parallel = false;
    cvector_vector_type(char *) cmdlines = parallel_command(buffer, &parallel);
    size_t ncmd = cvector_size(cmdlines);
    if ((ncmd == 0) && (parallel)) { // tests/16.in (Command only contains &)
      continue;
    }

    for (size_t i = 0; i < ncmd; i++) {
      char *redirect_output = NULL;
      bool redirect = redirect_function(cmdlines[i], &redirect_output);
      if ((redirect == true) && (redirect_output == NULL)) {
        error_function();
        continue;
      }

      cvector_vector_type(char *) cmd = parse_cmd(cmdlines[i], path, first);
      if (cmd == NULL) { // A string only has spaces. Example: "    "
        continue;
      }

      // built-in commands: "exit", "cd", "path"
      if (strcmp(first, "exit") == 0) {
        exit_function(cmd, path);
      } else if (strcmp(first, "cd") == 0) {
        chdir_function(cmd);
      } else if (strcmp(first, "path") == 0) {
        path_function(cmd, path);
      } else {
        // not built-in commands
        exec_cmd(cmd, path, redirect, redirect_output, parallel);
      }
      cvector_free(cmd);
    }

    for (size_t i = 0; i < ncmd; i++) {
      wait(NULL);
    }
    cvector_free(cmdlines);
  }
}