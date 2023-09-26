#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void find(char* path, char* name, char* path_name) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    switch(st.type) {
    case T_DEVICE:
    case T_FILE:
        if (path_name == 0) {
            int len_path = strlen(path), len_name = strlen(name), i, j;
            for (i = len_path - 1, j = len_name - 1; i >= 0 && path[i] != '/' && j >= 0 && path[i] == name[j]; --i, --j);
            if ((i < 0 || path[i] == '/') && j < 0) {
                printf("%s\n", path);
            }
        }
        else if (strcmp(name, path_name) == 0) {
            printf("%s\n", path);
        }
        break;
    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0) {
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0) {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            if (strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0) {
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0) {
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                find(buf, name, de.name);
            }
        }
        break;
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(2, "Usage: find path name\n");
        exit(1);
    }
    find(argv[1], argv[2], 0);
    exit(0);
}
