#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stddef.h>

char **parse_command(char *);

int main(void) {

  char error_message[30] = "An error has occurred\n";
  char *prompt = "prompt> ";
  bool smash = false;
  bool exit = false;
  while (!exit) {
    printf("%s", prompt);
    size_t size = 256;
    char *command = malloc(size);
    getline(&command, &size, stdin);

    // count number of unique commands
    char *temp = strdup(command);
    char *new = malloc(strlen(temp)-1);
    strncpy(new, temp, strlen(temp)-1);

    do {
      char *str = strsep(&temp, "&");
      
      char **tokens = parse_command(str);
      
      if (strcmp(tokens[0], "./smash") == 0 && smash == false) {
        if (tokens[2] != NULL) {
          write(STDERR_FILENO, error_message, strlen(error_message));
        } else if (tokens[1] != NULL) {
          smash = true;
          FILE *in = fopen(tokens[1], "r");
          if (in != NULL) {
            char *line = malloc(1000);

            while (fgets(line, 1000, in) != NULL) {
              char *temp2 = strdup(line);
              char *new2 = malloc(strlen(temp2)-1);
              strncpy(new2, temp2, strlen(temp2)-1);
              do{
                char *str2 = strsep(&temp2, "&");
                char **tokens2 = parse_command(str2);
                
                if (fork()) {
                  wait(0);
                  printf("\n");
                  continue;
                } else { // child process

                  if (strstr(str2, ">")) { // redirect output
                    tokens2 = parse_command(strsep(&str2, ">"));
                    int fd = open(str2, O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
                    dup2(fd, 1);
                    close(fd);
                    execvp(tokens2[0], tokens2);

                  } else { // output to console
                    tokens2 = parse_command(str2);
                    execvp(tokens2[0], tokens2);
                  }

                  return (0);
                }
              } while(temp2 != NULL);
            }
          } else {
            write(STDERR_FILENO, error_message, strlen(error_message));
          }
          smash = false;
        } else {
          smash = true;
          prompt = "smash> ";
        }

      } else if (strcmp(tokens[0], "exit") == 0) {
        exit = true;
      } else if (strcmp(tokens[0], "cd") == 0) {
        if (tokens[2] != NULL) {
          write(STDERR_FILENO, error_message, strlen(error_message));
        } else {
          chdir(tokens[1]);
        }
      } else if (strcmp(tokens[0], "path") == 0) {
        if (strcmp(tokens[1], "add") == 0) {

        } else if (strcmp(tokens[1], "remove") == 0) {

        } else if (strcmp(tokens[1], "clear") == 0) {

        } else {
          write(STDERR_FILENO, error_message, strlen(error_message));
        }
      } else {
        if (smash) {
          if (fork()) {
            wait(0);
						printf("\n");
            continue;
          } else {                  // child process
            if (strstr(str, ">")) { // redirect output
              tokens = parse_command(strsep(&str, ">"));
              int fd = open(str, O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
              dup2(fd, 1);
              close(fd);
              execvp(tokens[0], tokens);

            } else { // output to console
              execvp(tokens[0], tokens);
            }

            return (0);
          }
        }
				else{
					write(STDERR_FILENO, error_message, strlen(error_message));
				}
      }
    } while (temp != NULL);
  }
}

// Command borrowed from previous project from 337
char **parse_command(char *command) {
  const char whitespace[7] = " \t\n\v\f\r";
  char *token;

  char *copy = malloc(strlen(command) + 1);
  strncpy(copy, command, strlen(command));
  // count tokens, start with first
  int count = 0;
  token = strtok(copy, whitespace);

  /* walk through other tokens */
  while (token != NULL) {
    token = strtok(NULL, whitespace);
    count++;
  }
  char **ret = malloc((count + 1) * sizeof(char *));
  // make last cell of ret point to NULL
  ret[count] = NULL;

  // read first token into ret
  count = 0;
  strncpy(copy, command, strlen(command));
  token = strtok(copy, whitespace);
  ret[count] = token;
  count++;

  /* walk through other tokens */
  while (token != NULL) {
    token = strtok(NULL, whitespace);
    ret[count] = token;
    count++;
  }
  return ret;
}
