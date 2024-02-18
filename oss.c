#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>

#define MAX_CHILDREN 100

void print_help(const char *progname) {
    printf("Usage: %s [-h] [-n proc] [-s simul] [-t iter]\n", progname);
    printf("Options:\n");
    printf("  -h               Display this help message\n");
    printf("  -n proc          Number of total children to launch\n");
    printf("  -s simul         Maximum number of children running simultaneously\n");
    printf("  -t iter          Number of iterations for each child process\n");
}

int main(int argc, char *argv[]) {
    int num_children = 0;
    int max_simultaneous = 0;
    int iterations = 0;

    int opt;
    while ((opt = getopt(argc, argv, "hn:s:t:")) != -1) {
        switch (opt) {
            case 'h':
                print_help(argv[0]);
                exit(EXIT_SUCCESS);
            case 'n':
                num_children = atoi(optarg);
                break;
            case 's':
                max_simultaneous = atoi(optarg);
                break;
            case 't':
                iterations = atoi(optarg);
                break;
            default:
                print_help(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (num_children <= 0 || max_simultaneous <= 0 || iterations <= 0) {
        fprintf(stderr, "Error: Invalid arguments\n");
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (num_children > MAX_CHILDREN) {
        fprintf(stderr, "Error: Maximum number of children exceeded\n");
        exit(EXIT_FAILURE);
    }

    int children_running = 0;
    for (int i = 0; i < num_children; i++) {
        if (children_running >= max_simultaneous) {
            // Wait for a child to terminate before forking a new one
            wait(NULL);
            children_running--;
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            char iter_str[10];
            snprintf(iter_str, sizeof(iter_str), "%d", iterations);
            execl("./user", "user", iter_str, NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        } else {
            children_running++;
        }
    }

    // Wait for all remaining children to terminate
    while (wait(NULL) > 0);

    return 0;
}
