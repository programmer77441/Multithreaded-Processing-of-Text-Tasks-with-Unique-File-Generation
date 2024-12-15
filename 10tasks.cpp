#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <string.h>
#include <windows.h> 

#define NUM_TASKS 10
#define NUM_FILES 5
#define TASKS_PER_FILE 2

// function for reading from file
void read_tasks(const char* filename, char tasks[NUM_TASKS][100]) {
    FILE* file = NULL;
    if (fopen_s(&file, filename, "r") != 0 || file == NULL) {
        printf("Can not open this file!\n");
        exit(1);
    }

    for (int i = 0; i < NUM_TASKS; i++) {
        fgets(tasks[i], 100, file);
    }

    fclose(file);
}

//function for writing tasks in new files
void write_tasks_to_file(int file_id, char task1[100], char task2[100]) {
    FILE* file = NULL;
    char filename[100];
    snprintf(filename, sizeof(filename), "tasks_%d.txt", file_id);

    if (fopen_s(&file, filename, "w") != 0 || file == NULL) {
        printf("Can not open this file for edit!\n");
        exit(1);
    }

    fprintf(file, "%s", task1);
    fprintf(file, "%s", task2);

    fclose(file);
}

// Function to distribute tasks in 5 files without repetitions 
void distribute_tasks(char tasks[NUM_TASKS][100]) {
    int task_indices[NUM_TASKS]; 
    for (int i = 0; i < NUM_TASKS; i++) {
        task_indices[i] = i;
    }

    for (int i = NUM_TASKS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = task_indices[i];
        task_indices[i] = task_indices[j];
        task_indices[j] = temp;
    }

#pragma omp parallel num_threads(NUM_FILES)
    {
        int thread_id = omp_get_thread_num();
        int task1_index = task_indices[2 * thread_id]; 
        int task2_index = task_indices[2 * thread_id + 1]; 

        write_tasks_to_file(thread_id + 1, tasks[task1_index], tasks[task2_index]);
    }
}

int main() {
    char tasks[NUM_TASKS][100];

    // Четене на задачите от файла
    read_tasks("tasks.txt", tasks);

    // Разпределяне на задачите в 5 файла без повторения
    distribute_tasks(tasks);

    return 0;
}
