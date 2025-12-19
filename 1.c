#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

void print_process_info(const char* process_name) {
    struct timespec ts;
    struct tm *time_info;

    clock_gettime(CLOCK_REALTIME, &ts);
    time_info = localtime(&ts.tv_sec);

    printf("%s: PID = %d, PPID = %d, Time = %02d:%02d:%02d:%03ld\n",
           process_name,
           getpid(),
           getppid(),
           time_info->tm_hour,
           time_info->tm_min,
           time_info->tm_sec,
           ts.tv_nsec / 1000000);
}

int main() {
    pid_t pid1, pid2;

    pid1 = fork();

    if (pid1 < 0) {
        perror("Ошибка при первом вызове fork()");
        exit(EXIT_FAILURE);
    }
    if (pid1 == 0) {
        print_process_info("Дочерний процесс 1");
       exit(EXIT_SUCCESS);
    }

    pid2 = fork();

    if (pid2 < 0) {
        perror("Ошибка при втором вызове fork()");
        exit(EXIT_FAILURE);
    }
    if (pid2 == 0) {
        print_process_info("Дочерний процесс 2");
        exit(EXIT_SUCCESS);
    }

    sleep(1);

    print_process_info("Родительский процесс");
    printf("\nВыполнение команды ps -x\n");

    int result = system("ps -x");

    if (result == -1) {
        perror("Ошибка при выполнении system()");
    }

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return EXIT_SUCCESS;
}
