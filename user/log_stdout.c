#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include <stdarg.h>

char buf[1024];

int read_stdin(char *buf)
{
    /*
    Description: Read stdin into buf
    Example:
        - read_stdin(); // Read the stdin into buf
    Parameters:
        - buf (char*): A buffer to store all characters
    Return:
        - 0 (int)
    */
    // Your code here
    if (read(0, buf, 1024) < 0)
    {
        return -1;
    }
    // End
    return 0;
}

int log_stdout(uint i)
{
    /*
    Description: Redirect stdout to a log file named i.log
    Example:
        - log_stdout(1); // Redirect the stdout to 1.log and return 0
    Parameters:
        - i (uint): A number
    Return:
        - 0 (int)
    */
    char log_name[15] = "0.log";
    // Your code here
    char name[15];
    int temp;
    int end = 15;
    while ((i > 0 && end > 0) || end == 15)
    {
        temp = i % 10;
        i = (i - temp) / 10;
        name[--end] = '0' + temp;
    }
    int k = 0;
    int j;
    for (j = end; j < 15;)
    {
        log_name[k++] = name[j++];
    }
    strcpy(log_name + k, ".log");
    int fd;
    if ((fd = open(log_name, O_WRONLY | O_CREATE | O_TRUNC)) < 0)
    {
        fprintf(2, "log_stdout: open failed\n");
        return -1;
    }
    close(1);
    if (dup(fd) != 1)
    {
        fprintf(2, "log_stdout: redirect failed\n");
        return -1;
    }
    // End
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(2, "Usage: log_stdout number\n");
        exit(1);
    }
    if (log_stdout(atoi(argv[1])) != 0)
    {
        fprintf(2, "log_stdout: log_stdout failed\n");
        exit(1);
    }
    if (read_stdin(buf) != 0)
    {
        fprintf(2, "log_stdout: read_stdin failed\n");
        exit(1);
    }
    printf(buf);
    exit(0);
}