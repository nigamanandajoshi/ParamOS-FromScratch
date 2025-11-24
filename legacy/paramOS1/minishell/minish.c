// Param OS - Mini Shell (ParamShell)
// Features:
//  - Builtins: cd, exit
//  - External commands via fork + execvp
//  - Simple pipelines: cmd1 | cmd2 | cmd3
//  - Background jobs: cmd &

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_TOKENS 128
#define MAX_CMDS   16

struct command {
    char *argv[MAX_TOKENS];
    int argc;
};

static void print_prompt(void) {
    printf("paramsh> ");
    fflush(stdout);
}

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n')
        s[len - 1] = '\0';
}

static int parse_line(char *line, struct command cmds[], int *is_background) {
    char *saveptr;
    char *segment;
    int cmd_count = 0;

    *is_background = 0;

    // Check for background '&'
    char *amp = strrchr(line, '&');
    if (amp && *(amp + 1) == '\0') {
        *amp = '\0';
        *is_background = 1;
    }

    // Split by '|'
    segment = strtok_r(line, "|", &saveptr);
    while (segment && cmd_count < MAX_CMDS) {
        struct command *cmd = &cmds[cmd_count];
        cmd->argc = 0;

        // Tokenize this segment
        char *tok;
        char *seg_saveptr;
        tok = strtok_r(segment, " \t", &seg_saveptr);
        while (tok && cmd->argc < MAX_TOKENS - 1) {
            cmd->argv[cmd->argc++] = tok;
            tok = strtok_r(NULL, " \t", &seg_saveptr);
        }
        cmd->argv[cmd->argc] = NULL;

        if (cmd->argc > 0)
            cmd_count++;

        segment = strtok_r(NULL, "|", &saveptr);
    }

    return cmd_count;
}

static int is_builtin(struct command *cmd) {
    if (cmd->argc == 0) return 0;
    return (strcmp(cmd->argv[0], "cd") == 0 ||
            strcmp(cmd->argv[0], "exit") == 0);
}

static int run_builtin(struct command *cmd) {
    if (strcmp(cmd->argv[0], "cd") == 0) {
        const char *target = cmd->argc > 1 ? cmd->argv[1] : getenv("HOME");
        if (!target) target = "/";
        if (chdir(target) != 0) {
            perror("cd");
            return -1;
        }
        return 0;
    } else if (strcmp(cmd->argv[0], "exit") == 0) {
        exit(0);
    }
    return -1;
}

static void execute_pipeline(struct command cmds[], int cmd_count, int is_background) {
    int i;
    int prev_fd[2] = {-1, -1};
    pid_t pids[MAX_CMDS];

    for (i = 0; i < cmd_count; i++) {
        int pipe_fd[2] = {-1, -1};

        if (i < cmd_count - 1) {
            if (pipe(pipe_fd) == -1) {
                perror("pipe");
                return;
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return;
        } else if (pid == 0) {
            // Child

            // stdin from previous pipe
            if (i > 0) {
                dup2(prev_fd[0], STDIN_FILENO);
            }

            // stdout to next pipe
            if (i < cmd_count - 1) {
                dup2(pipe_fd[1], STDOUT_FILENO);
            }

            // close all ends
            if (prev_fd[0] != -1) close(prev_fd[0]);
            if (prev_fd[1] != -1) close(prev_fd[1]);
            if (pipe_fd[0] != -1) close(pipe_fd[0]);
            if (pipe_fd[1] != -1) close(pipe_fd[1]);

            execvp(cmds[i].argv[0], cmds[i].argv);
            fprintf(stderr, "paramsh: command not found: %s\n", cmds[i].argv[0]);
            _exit(127);
        } else {
            // Parent
            pids[i] = pid;

            // close previous pipe ends
            if (prev_fd[0] != -1) close(prev_fd[0]);
            if (prev_fd[1] != -1) close(prev_fd[1]);

            // move pipe_fd to prev_fd for next iteration
            prev_fd[0] = pipe_fd[0];
            prev_fd[1] = pipe_fd[1];

            // close write end in parent if exists
            if (prev_fd[1] != -1)
                close(prev_fd[1]);
        }
    }

    // parent closes last read end
    if (prev_fd[0] != -1) close(prev_fd[0]);

    if (!is_background) {
        for (int j = 0; j < cmd_count; j++) {
            int status;
            waitpid(pids[j], &status, 0);
        }
    } else {
        printf("[background] started pipeline with %d commands\n", cmd_count);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;

    while (1) {
        print_prompt();

        if (getline(&line, &len, stdin) == -1) {
            if (feof(stdin)) {
                printf("\n");
                break;
            }
            perror("getline");
            continue;
        }

        trim_newline(line);
        if (line[0] == '\0')
            continue;

        struct command cmds[MAX_CMDS];
        int is_background = 0;
        int cmd_count = parse_line(line, cmds, &is_background);

        if (cmd_count == 0)
            continue;

        // If single built-in command, run in main process (no fork)
        if (cmd_count == 1 && is_builtin(&cmds[0])) {
            run_builtin(&cmds[0]);
        } else {
            execute_pipeline(cmds, cmd_count, is_background);
        }
    }

    free(line);
    return 0;
}
