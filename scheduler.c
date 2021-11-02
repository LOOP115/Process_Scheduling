#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "list.h"
#include "tool.h"


/* Single and Dual are not used in the final version
 * since multi can do all the tasks */

// Single processor scheduler
void single(list_t *proc_list, int proc_count) {
    int time = 0;
    int remain = 0;
    int complete = 0;
    double total_turnaround = 0;
    double total_overhead = 0;
    double max_overhead = 0;

    // Add coming processes to the list
    // Sort the list and find the process with shortest time remaining
    list_t* running_list = make_empty_list();
    add_sort(proc_list, running_list, time, &remain);
    node_t *running_proc = running_list->head;

    // Start scheduling
    while (complete != proc_count) {
        // Execute the picked process
        if (running_proc->data->state == 0) {
            printf("%d,RUNNING,pid=%d,remaining_time=%g,cpu=0\n", time, running_proc->data->pid, running_proc->data->rtime);
            running_proc->data->state = 1;
        }
        time++;
        running_proc->data->rtime--;

        // Check if the process is completed
        if (running_proc->data->rtime == 0) {
            complete++;
            remain--;
            double turnaround = time - running_proc->data->atime;
            total_turnaround += turnaround;
            double overhead = turnaround/running_proc->data->etime;
            total_overhead += overhead;
            if (overhead > max_overhead) {
                max_overhead = overhead;
            }
            printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", time, running_proc->data->pid, remain);
            delete_head(running_list);
        }

        // Add coming processes to the list
        // Sort the list and find the process with shortest time remaining
        add_sort(proc_list, running_list, time, &remain);
        // Check if the process founded is the previous one and change the state if necessary
        node_t *prev_proc = running_proc;
        running_proc = running_list->head;
        if (prev_proc != running_proc) {
            prev_proc->data->state = 0;
        }
    }
    // Performance statistics
    printf("Turnaround time %.0f\n", ceil(total_turnaround/proc_count));
    printf("Time overhead %g %g\n", round(max_overhead*100)/100, round(total_overhead/proc_count*100)/100);
    printf("Makespan %d\n", time);
}


// Two processor scheduler
void dual(list_t *proc_list, int proc_count) {
    int time = 0;
    int remain = 0;
    int complete = 0;
    double total_turnaround = 0;
    double total_overhead = 0;
    double max_overhead = 0;

    // Make lists for each processor
    list_t *cpu0_list = make_empty_list();
    list_t *cpu1_list = make_empty_list();

    // Start scheduling
    while (complete != proc_count) {
        // Add coming processes to the list
        list_t *new_list = make_empty_list();
        int count = add_process(proc_list, new_list, time);
        merge_sort_rtime(&new_list->head);
        adjust_foot(new_list);
        if (count > 0) {
            for (int i = 0; i < count; i++) {
                delete_head(proc_list);
            }
            // Allocate new processes to cpu
            dual_alloc(cpu0_list, cpu1_list, new_list, &remain);
        }
        free_list(new_list);

        // Run cpu0 and cpu1
        dual_cpu_run(cpu0_list, cpu1_list, time, &remain, &complete, 0, &total_turnaround, &total_overhead, &max_overhead);
        dual_cpu_run(cpu1_list, cpu0_list, time, &remain, &complete, 1, &total_turnaround, &total_overhead, &max_overhead);
        time++;
    }
    // Performance statistics
    printf("Turnaround time %.0f\n", ceil(total_turnaround/proc_count));
    printf("Time overhead %g %g\n", round(max_overhead*100)/100, round(total_overhead/proc_count*100)/100);
    printf("Makespan %d\n", time);
    free_list(cpu0_list);
    free_list(cpu1_list);
}


// N processor scheduler
void multi(list_t *proc_list, int proc_count, int cpu_count) {
    int time = 0;
    int remain = 0;
    int complete = 0;
    double total_turnaround = 0;
    double total_overhead = 0;
    double max_overhead = 0;

    // Initialize all cpus
    cpuList_t **cpu_lists = malloc(sizeof(cpuList_t*)*cpu_count);
    for (int i=0; i<cpu_count; i++) {
        cpu_lists[i] = malloc(sizeof(cpuList_t));
        cpu_lists[i]->run_list = make_empty_list();
        cpu_lists[i]->id = i;
        cpu_lists[i]->remain_time = 0;
    }
    // Start scheduling
    while(complete != proc_count) {
        // Add coming processes to the list
        list_t *new_list = make_empty_list();
        int count = add_process(proc_list, new_list, time);
        merge_sort_rtime(&new_list->head);
        adjust_foot(new_list);
        if (count > 0) {
            for (int i = 0; i < count; i++) {
                delete_head(proc_list);
            }
            // Allocate new processes to cpu
            multi_alloc(cpu_lists, new_list, &remain, cpu_count);
            insertionSort(cpu_lists, cpu_count);
        }
        free_list(new_list);

        // Run all cpus
        list_t **run_finish_list;
        run_finish_list = multi_cpu_run(cpu_lists, cpu_count, time, &remain, &complete, &total_turnaround, &total_overhead, &max_overhead);
        if (run_finish_list[0]->head != NULL) {
            print_running(run_finish_list[0], time);
        }
        if (run_finish_list[1]->head != NULL) {

            print_finished(run_finish_list[1], time, remain);
        }
        free_list(run_finish_list[0]);
        free_list(run_finish_list[1]);
        free(run_finish_list);
        time++;
    }

    // Performance statistics
    printf("Turnaround time %.0f\n", ceil(total_turnaround/proc_count));
    printf("Time overhead %g %g\n", round(max_overhead*100)/100, round(total_overhead/proc_count*100)/100);
    printf("Makespan %d\n", time);

    for (int i=0; i<cpu_count; i++) {
        free_list(cpu_lists[i]->run_list);
    }
    free(cpu_lists);
}


// Challenge
void improve(list_t *proc_list, int proc_count, int cpu_count) {
    int time = 0;
    int remain = 0;
    int complete = 0;
    double total_turnaround = 0;
    double total_overhead = 0;
    double max_overhead = 0;

    // Initialize all cpus
    cpuList_t **cpu_lists = malloc(sizeof(cpuList_t*)*cpu_count);
    for (int i=0; i<cpu_count; i++) {
        cpu_lists[i] = malloc(sizeof(cpuList_t));
        cpu_lists[i]->run_list = make_empty_list();
        cpu_lists[i]->id = i;
        cpu_lists[i]->remain_time = 0;
    }
    // Start scheduling
    while(complete != proc_count) {
        // Add coming processes to the list
        list_t *new_list = make_empty_list();
        int count = add_process(proc_list, new_list, time);
        merge_sort_rtime(&new_list->head);
        adjust_foot(new_list);
        if (count > 0) {
            for (int i = 0; i < count; i++) {
                delete_head(proc_list);
            }
            // Allocate new processes to cpu
            c_alloc(cpu_lists, new_list, &remain, cpu_count, proc_list, time);
            insertionSort(cpu_lists, cpu_count);
        }
        free_list(new_list);

        // Run all cpus
        list_t **run_finish_list;
        run_finish_list = c_cpu_run(cpu_lists, cpu_count, time, &remain, &complete, &total_turnaround, &total_overhead, &max_overhead);
        if (run_finish_list[0]->head != NULL) {
            print_running(run_finish_list[0], time);
        }
        if (run_finish_list[1]->head != NULL) {
            print_finished(run_finish_list[1], time, remain);
        }
        free_list(run_finish_list[0]);
        free_list(run_finish_list[1]);
        free(run_finish_list);
        time++;
    }

    // Performance statistics
    printf("Turnaround time %.0f\n", ceil(total_turnaround/proc_count));
    printf("Time overhead %g %g\n", round(max_overhead*100)/100, round(total_overhead/proc_count*100)/100);
    printf("Makespan %d\n", time);

    for (int i=0; i<cpu_count; i++) {
        free_list(cpu_lists[i]->run_list);
    }
    free(cpu_lists);
}
