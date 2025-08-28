//          ╭─────────────────────────────────────────────────────────╮
//          │ author : l-bloodstone                                   │
//          │ email  : lucas.bloodstone@outlook.com                   │
//          ╰─────────────────────────────────────────────────────────╯
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

// Prints a prompt with a '$' sign
char* prompt(){
    printf("$ ");
    char* line = NULL;
    size_t buff_size;
    getline(&line, &buff_size, stdin);
    return line;
}

// Tokenize the args list by whitespaces
char** tokenize_line(char* line) {
    // hardcoded 'exit' shell builtin command
    int s = strcmp(line, "exit\n");
    if (s == 0) {
        exit(EXIT_SUCCESS);
    }
    int TOKENS_BUFFER_SIZE = 128;
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
        // reallocate the buffer if needed
        if (i == TOKENS_BUFFER_SIZE - 1) {
            tokens = realloc(tokens, TOKENS_BUFFER_SIZE * 2);
            if (!tokens) {
                perror("Allocation Failed!!");
            }
            TOKENS_BUFFER_SIZE *= 2;
        }
        token = strtok(NULL, TOKEN_DELIM);
    }
    return tokens;
}

int execute_command(char** args) {
    int fd[2];
    // Creating a pipe to pass status code from child to parent
    pipe(fd);
    int pid = fork();
    if (pid < 0) {
        perror("Fork failed!!");
        close(fd[0]);
        close(fd[1]);
    }
    if (pid == 0) {
        close(fd[0]);
        int exit_status = execvp(args[0], args);
        write(fd[1], (void*)&exit_status, 4);
        close(fd[1]);
        // explicit exit from child process
        exit(exit_status);
    } else {
        close(fd[1]);
        wait(NULL);
        int status = 0;
        read(fd[0], (void*)&status, 4);
        close(fd[0]);
        return status;
    }
}

void shell_loop(){
    while (1) {
        char* line = prompt();
        char** args = tokenize_line(line);
        int status = execute_command(args);
        // print status with red color text
        if (status) {
            printf("\033[31m%d\033[0m ", status);
        }
        free(line);
        free(args);
    }
}

int main(){
    shell_loop();
    return 0;
}
