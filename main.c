#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

char* prompt(){
    printf("$ ");
    char* line = NULL;
    size_t buff_size;
    getline(&line, &buff_size, stdin);
    return line;
}

char** tokenize_line(char* line) {
    int s = strcmp(line, "exit\n");
    if (s == 0) {
        exit(EXIT_SUCCESS);
    }
    #define TOKENS_BUFFER_SIZE 128
    char** tokens = malloc(TOKENS_BUFFER_SIZE * sizeof(char*));
    if (!tokens) {
        perror("Allocation failed!!");
    }
    #define TOKEN_DELIM " \r\t\n\a"
    char* token;
    token = strtok(line, TOKEN_DELIM);
    for (int i = 0; i < TOKENS_BUFFER_SIZE; i++) {
        if (token == NULL) {
            tokens[i] = NULL;
            break;
        }
        tokens[i] = token;
        if (i == TOKENS_BUFFER_SIZE - 1) {
            tokens = realloc(tokens, TOKENS_BUFFER_SIZE * 2);
        }
        token = strtok(NULL, TOKEN_DELIM);
    }
    return tokens;
}

void execute_command(char** args) {
    int pid = fork();
    if (pid < 0) {
        perror("Fork failed!!");
    }
    if (pid == 0) {
        execvp(*args, args);
    } else {
        wait(NULL);
    }
}

void shell_loop(){
    while (1) {
        char* line = prompt();
        char** args = tokenize_line(line);
        execute_command(args);
    }
}

int main(){
    shell_loop();
    return 0;
}
