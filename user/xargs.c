#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXLINE 128

void freeArgs(char **args)
{
    for (int i = 0; args[i]; i++)
    {
        free(args[i]);
    }
}

void run(char **args)
{
    int pid;
    if ((pid = fork()) < 0)
    {
        fprintf(2, "xargs: fork error\n");
        exit(1);
    }
    if (pid == 0)
    {
        exec(args[0], args);
        exit(1);
    }
    else
    {
        int status;
        wait(&status);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "Usage: xargs files...\n");
        exit(1);
    }
    char *args[MAXARG];
    int cnt = 0;
    for (int i = 1; i < argc; i++)
    {
        if (cnt >= MAXARG - 2)
        {
            fprintf(2, "xargs: too many arguments\n");
            exit(1);
        }
        args[cnt] = (char *)malloc(strlen(argv[i]));
        strcpy(args[cnt++], argv[i]);
    }
    args[cnt] = (char *)malloc(MAXLINE);
    args[cnt][0] = '\0';
    args[cnt + 1] = 0;
    char c = 0;
    int idx = -1;
    int num_read = 0;
    while (1)
    {
        num_read = read(0, &c, 1);
        if (num_read == -1)
        {
            fprintf(2, "xargs: read error\n");
            exit(1);
        }
        if (num_read == 0)
        {
            break;
        }
        if (c == '\n')
        {
            if (idx > 0)
            {
                args[cnt][idx] = '\0';
                run(args);
                idx = 0;
            }
        }
        else
        {
            if (idx < 0)
            {
                idx = 0;
            }
            if (idx >= MAXLINE - 1)
            {
                fprintf(2, "xargs: argument too long\n");
                exit(1);
            }
            args[cnt][idx++] = c;
        }
    }
    if (idx > 0)
    {
        args[cnt][idx] = '\0';
        run(args);
    }
    else if (idx == -1)
    {
        args[cnt][0] = '\0';
        run(args);
    }
    freeArgs(args);
    exit(0);
}