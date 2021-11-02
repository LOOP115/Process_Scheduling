#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"
#include "tool.h"
#include "scheduler.h"


int main(int argc, char **argv) {
    // Read command line arguments
    char *filename;
    char *processors;
    int challenge = 0;
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], FILENAME) == 0) {
            filename = argv[i+1];
        } else if (strcmp(argv[i], PROCESSORS) == 0) {
            processors = argv[i+1];
        } else if (strcmp(argv[i], CHALLENGE) == 0) {
            challenge = 1;
        }
    }
    
    // Read the file and retrieve information of all processes
    char* line = NULL;
    size_t lineBufferLength = 1000;
    FILE* input = NULL;
    input = fopen(filename, "r");
    assert(input);

    int proc_count = 0;
    list_t* proc_list = make_empty_list();
    process_t* data;

    while(getline(&line, &lineBufferLength, input) > 0){
        data = data_extract(line);
        insert_at_foot(proc_list, data);
        proc_count++;
    }

    // Free memory
    if(line){
        free(line);
        line = NULL;
    }
    if(input){
        fclose(input);
    }

    // Process scheduling
    int cpu_count = atoi(processors);
    if (!challenge) {
        multi(proc_list, proc_count, cpu_count);
    } else {
        improve(proc_list, proc_count, cpu_count);
    }

    free_list(proc_list);
    return 0;
}
