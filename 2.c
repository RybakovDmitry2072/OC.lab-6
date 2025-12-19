#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define MICRO_TO_MILLI 1000
#define MILLIS_PER_SECOND 1000

typedef struct {
    long total_milliseconds;
} Timestamp;

static void show_process_info(int process_id) {
    struct timeval time_val;

    gettimeofday(&time_val, NULL);

    Timestamp ts = {
        .total_milliseconds = time_val.tv_sec * MILLIS_PER_SECOND +
                             time_val.tv_usec / MICRO_TO_MILLI
    };

    printf("Процесс=%d PID=%d PPID=%d Время=%ld\n",
           process_id, getpid(), getppid(), ts.total_milliseconds);

    fflush(stdout);
}

static void continuous_execution(int id_num) {
    int delay_ms = id_num * 200;

    while (1) {
        show_process_info(id_num);
        usleep(delay_ms * MICRO_TO_MILLI);
    }
}

static void generate_process_tree(int current_level, int max_levels,
                                 int child_count, int node_index) {
    if (current_level >= max_levels) {
        return;
    }

    for (int child_num = 1; child_num <= child_count; child_num++) {
        pid_t child_pid = fork();

        if (child_pid < 0) {
            perror("Ошибка при первом вызове fork()");
            exit(EXIT_FAILURE);
        }

        if (child_pid == 0) {
            int new_id = (node_index - 1) * child_count + child_num + 1;
            generate_process_tree(current_level + 1, max_levels,
                                 child_count, new_id);
            continuous_execution(new_id);
        }
    }
}

int main(int parameter_count, char *parameter_values[]) {
    if (parameter_count != 3) {
        fprintf(stderr, "Использование: %s <уровни> <потомки>\n", parameter_values[0]);
        return EXIT_FAILURE;
    }

    int tree_levels = atoi(parameter_values[1]);
    int offspring_count = atoi(parameter_values[2]);

    if (tree_levels < 0 || offspring_count < 0) {
        fprintf(stderr, "уровни и потомки должны быть >= 0\n");
        return EXIT_FAILURE;
    }

    generate_process_tree(0, tree_levels, offspring_count, 1);
    continuous_execution(1);

    return 0;
}
