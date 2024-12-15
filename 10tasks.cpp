#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>  
#endif

#define NUM_TASKS 10      // 10 tasks
#define NUM_FILES 5       // 5 files
#define TASKS_PER_FILE 2  // 2 tasks per file

// Function to read tasks from the file
void read_tasks(const char* filename, char tasks[NUM_TASKS][10]) {
    FILE* file = NULL;
    if (fopen_s(&file, filename, "r") != 0 || file == NULL) {
        printf("Can not open this file!\n");
        exit(1);
    }

    for (int i = 0; i < NUM_TASKS; i++) {
        fgets(tasks[i], 10, file);
    }

    fclose(file);
}

// Function to save tasks to a new file
void write_tasks_to_file(int file_id, char tasks[TASKS_PER_FILE][100]) {
    FILE* file = NULL;
    char filename[100];
    snprintf(filename, sizeof(filename), "tasks_%d.txt", file_id);

    if (fopen_s(&file, filename, "w") != 0 || file == NULL) {
        printf("Can not open this file for edit!\n");
        exit(1);
    }

    for (int i = 0; i < TASKS_PER_FILE; i++) {
        fprintf(file, "%s", tasks[i]);
    }

    fclose(file);
}

//Function to distribute tasks into 5 files without repetitions
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
        int start_index = thread_id * TASKS_PER_FILE;
        int task_indices_for_file[TASKS_PER_FILE];

        DWORD_PTR affinity_mask = (1 << thread_id); 
        SetThreadAffinityMask(GetCurrentThread(), affinity_mask);

        int core_id = GetCurrentProcessorNumber();

        for (int i = 0; i < TASKS_PER_FILE; i++) {
            task_indices_for_file[i] = task_indices[start_index + i];
        }

        char tasks_for_file[TASKS_PER_FILE][100];
        for (int i = 0; i < TASKS_PER_FILE; i++) {
            strcpy_s(tasks_for_file[i], sizeof(tasks_for_file[i]), tasks[task_indices_for_file[i]]);
        }

        write_tasks_to_file(thread_id + 1, tasks_for_file);
    }
}

int main() {
    char tasks[NUM_TASKS][100];

    read_tasks("tasks.txt", tasks);

    srand(time(NULL));

    distribute_tasks(tasks);

    return 0;
}
