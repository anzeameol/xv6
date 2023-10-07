#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        fprintf(2, "Usage: pingpong files...\n");
        exit(1);
    }
    int p[2];
    if (pipe(p) < 0)
    {
        fprintf(2, "pingpong: pipe failed\n");
        exit(1);
    }
    int pid = fork();
    char c;
    if (pid == 0)
    {
        read(p[0], &c, 1);
        printf("%d: received ping\n", getpid());
        write(p[1], &c, 1);
    }
    else
    {
        c = 'h';
        write(p[1], &c, 1);
        read(p[0], &c, 1);
        printf("%d: received pong\n", getpid());
    }
    exit(0);
}