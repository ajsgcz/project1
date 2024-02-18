#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int opt, proc = 0, simul = 0, iter = 0;
    pid_t pid;
    char iter_str[10];

    while ((opt = getopt(argc, argv, "hn:s:t:")) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: oss [-h] [-n proc] [-s simul] [-t iter]\n");
                return 0;
            case 'n':
                proc = atoi(optarg);
                break;
            case 's':
                simul = atoi(optarg);
                break;
            case 't':
                iter = atoi(optarg);
                break;
            default:
                printf("Invalid option\n");
                return 1;
        }
    }

    sprintf(iter_str, "%d", iter);

    for (int i = 0; i < proc; i++) {
        if ((pid = fork()) == 0) {
            execl("./user", "./user", iter_str, NULL);
        }

        if (i % simul == 0 && i != 0) {
            wait(NULL);
        }
    }

    while (wait(NULL) > 0);

    return 0;
}
