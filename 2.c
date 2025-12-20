#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define MICRO_TO_MILLI 1000
#define MILLIS_PER_SECOND 1000

static long global_start_time = 0;

static void init_start_time() {
    if (global_start_time == 0) {
        struct timeval time_val;
        gettimeofday(&time_val, NULL);
        global_start_time = time_val.tv_sec * MILLIS_PER_SECOND +
                           time_val.tv_usec / MICRO_TO_MILLI;
    }
}

static void show_process_info(int process_id, int child_order) {
    struct timeval time_val;
    gettimeofday(&time_val, NULL);

    long current_time = time_val.tv_sec * MILLIS_PER_SECOND +
                       time_val.tv_usec / MICRO_TO_MILLI;

    long elapsed_time = current_time - global_start_time;

    if (child_order > 0) {
        printf("Процесс=%d PID=%d PPID=%d Время=%ld Дочерний №%d\n",
               process_id, getpid(), getppid(), elapsed_time, child_order);
   } else {
        printf("Процесс=%d PID=%d PPID=%d Время=%ld (корневой)\n",
               process_id, getpid(), getppid(), elapsed_time);
    }

    fflush(stdout);
}

static void continuous_execution(int id_num, int child_order) {
    int delay_ms = id_num * 200;

    while (1) {
        show_process_info(id_num, child_order);
        usleep(delay_ms * MICRO_TO_MILLI);
    }
}

static void generate_process_tree(int current_level, int max_levels,
                                 int child_count, int node_index,
                                 int parent_child_order) {
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
                                 child_count, new_id, child_num);
            continuous_execution(new_id, child_num);
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

    init_start_time();

    generate_process_tree(0, tree_levels, offspring_count, 1, 0);

    continuous_execution(1, 0);
    return 0;
}
