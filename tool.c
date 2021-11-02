#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "list.h"
#include "tool.h"


// Extract the information of each process
process_t *data_extract(char* str) {
    process_t *res = (process_t*)malloc(sizeof(*res));
    // initial state is not running
    res->state = 0;
    res->cpu_id = 0;
    // time-arrived
    char* token = strtok(str, " ");
    res->atime = atoi(token);
    int index = 1;
    while (token != NULL && index < 4) {
        token = strtok(NULL, " ");
        // process-id
        if (index == 1) {
            res->pid = atoi(token);
        // execution-time
        } else if (index == 2){
            res->etime = atoi(token);
            res->rtime = res->etime;
        // parallelisable, 1 for n and 0 for p
        } else if (index == 3) {
            res->parallel = strcmp(token, "n\n") == 0 || strcmp(token, "n ") == 0 || strcmp(token, "n") == 0 ? 0 : 1;
        }
        index++;
    }
    return res;
}


// Add arriving processes
int add_process(list_t *proc_list, list_t *running_list, int time) {
    node_t *curr = proc_list->head;
    // No more new process
    if (curr == NULL) {
        return 0;
    }

    // Find new processes arriving now and add them to the running list
    int count = 0;
    while(curr->data->atime <= time) {
        if (curr->data->atime == time) {
            process_t *clone = (process_t *)malloc(sizeof(*clone));
            copy_data(clone, curr->data);
            insert_at_foot(running_list, clone);
            count++;
        }
        curr = curr->next;
        // No more new process
        if (curr == NULL) {
            return count;
        }
    }
    return count;
}


// Copy process data from another node
void copy_data(process_t *dest, process_t *src) {
    dest->atime = src->atime;
    dest->pid = src->pid;
    dest->etime = src->etime;
    dest->parallel = src->parallel;
    dest->rtime = src->rtime;
    dest->state = src->state;
}


// Combine add_process and merge_sort
void add_sort(list_t *proc_list, list_t *running_list, int time, int *remain) {
    int count = add_process(proc_list, running_list, time);
    if (count > 0) {
        for (int i=0; i<count; i++) {
            delete_head(proc_list);
            *remain += 1;
        }
        merge_sort_rtime(&running_list->head);
        adjust_foot(running_list);
    }
}


// Calculate remaining execution time of a cpu
double remain_time(list_t *cpu_list) {
    node_t *curr = cpu_list->head;
    if (curr == NULL) {
        return 0;
    }
    double res = 0;

    while (1) {
        res += curr->data->rtime;
        curr = curr->next;
        // No more process
        if (curr == NULL) {
            return res;
        }
    }
}


// Check if a process exist on a list
int id_exist(list_t *list, int pid) {
    node_t *curr = list->head;
    if (curr == NULL) {
        return 0;
    }

    while(1) {
        if (curr->data->pid == pid) {
            return 1;
        }
        curr = curr->next;
        // No more process
        if (curr == NULL) {
            return 0;
        }
    }
}


// Get min value of two inputs
double min(double x, double y) {
    return x < y ? x : y;
}


// Sort array of cpu_lists base on remaining time
void insertionSort(cpuList_t **cpu_lists, int n) {
    int i, j;
    cpuList_t *key;
    for (i = 1; i < n; i++) {
        key = cpu_lists[i];
        j = i - 1;

        while (j >= 0 && cpu_lists[j]->remain_time >= key->remain_time) {
            if (cpu_lists[j]->remain_time == key->remain_time) {
                if (cpu_lists[j]->id < key->id) {
                    break;
                }
            }
            cpu_lists[j + 1] = cpu_lists[j];
            j = j - 1;
        }
        cpu_lists[j + 1] = key;
    }
}


// Allocate processes to 2 cpus
void dual_alloc(list_t *cpu0_list, list_t *cpu1_list, list_t *new_list, int *remain) {
    node_t *curr = new_list->head;
    // No new process
    if (curr == NULL) {
        return;
    }

    while(1) {
        *remain += 1;
        // parallelisable
        if (curr->data->parallel) {
            double sub_etime = ceil(curr->data->etime/2)+1;
            process_t *sub0 = (process_t*)malloc(sizeof(*sub0));
            process_t *sub1 = (process_t*)malloc(sizeof(*sub0));
            copy_data(sub0, curr->data);
            copy_data(sub1, curr->data);
            sub0->rtime = sub_etime;
            sub1->rtime = sub_etime;

            insert_at_foot(cpu0_list, sub0);
            insert_at_foot(cpu1_list, sub1);
        } else {
            // not parallelisable
            process_t *clone = (process_t*)malloc(sizeof(*clone));
            copy_data(clone, curr->data);
            if (remain_time(cpu0_list) <= remain_time(cpu1_list)) {
                insert_at_foot(cpu0_list, clone);
            } else {
                insert_at_foot(cpu1_list, clone);
            }
        }
        curr = curr->next;
        // No more new process
        if (curr == NULL) {
            return;
        }
    }
}


// Run the cpu in a unit time
void dual_cpu_run(list_t *run_list, list_t *other_list, int time, int *remain, int *complete, int cpu_id, double *total_turnaround, double *total_overhead, double *max_overhead) {
    // Check if there is no process on the cpu
    if (run_list->head == NULL) {
        return;
    }
    node_t *prev_proc = run_list->head;
    merge_sort_rtime(&run_list->head);
    adjust_foot(run_list);
    node_t *running_proc =  run_list->head;
    // Check if the running process is the previous one
    if (prev_proc != running_proc) {
        prev_proc->data->state = 0;
    }
    if (running_proc->data->state == 0) {
        if (running_proc->data->parallel) {
            printf("%d,RUNNING,pid=%d.%d,remaining_time=%g,cpu=%d\n", time,
                   running_proc->data->pid, cpu_id, running_proc->data->rtime, cpu_id);
        } else {
            printf("%d,RUNNING,pid=%d,remaining_time=%g,cpu=%d\n", time,
                   running_proc->data->pid, running_proc->data->rtime, cpu_id);
        }
        running_proc->data->state = 1;
    }

    // Execute the process and check if it is finished
    running_proc->data->rtime--;
    if (running_proc->data->rtime == 0) {
        if (!running_proc->data->parallel) {
            *remain -= 1;
            *complete += 1;
            double turnaround = time+1-running_proc->data->atime;
            double overhead = turnaround/running_proc->data->etime;
            *total_turnaround += turnaround;
            *total_overhead += overhead;
            if (overhead > *max_overhead) {
                *max_overhead = overhead;
            }
            printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", time+1, running_proc->data->pid, *remain);
        } else {
            if (!id_exist(other_list, running_proc->data->pid)) {
                *remain -= 1;
                *complete += 1;
                double turnaround = time+1-running_proc->data->atime;
                double overhead = turnaround/running_proc->data->etime;
                *total_turnaround += turnaround;
                *total_overhead += overhead;
                if (overhead > *max_overhead) {
                    *max_overhead = overhead;
                }
                printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", time+1, running_proc->data->pid, *remain);
            }
        }
        delete_head(run_list);
    }
}


// Check if all the subprocesses are finished
int sub_finished(cpuList_t **cpu_lists, int cpu_count, int curr_cpu, int pid) {
    for (int i=0; i<cpu_count; i++) {
        if (i != curr_cpu && id_exist(cpu_lists[i]->run_list, pid)) {
            return 0;
        }
    }
    return 1;
}


// Print finished process
void print_finished(list_t *list, int time, int remain) {
    if (list->head == NULL) {
        return;
    }
    merge_sort_pid(&list->head);
    adjust_foot(list);
    node_t *curr =list->head;
    while(1) {
        printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", time+1, curr->data->pid, remain);
        if (curr->next == NULL) {
            return;
        }
        curr = curr->next;
    }
}


// Print running process
void print_running(list_t *list, int time) {
    if (list->head == NULL) {
        return;
    }
    merge_sort_cpuid(&list->head);
    adjust_foot(list);
    node_t *curr =list->head;
    while(1) {
        if (curr->data->parallel) {
            printf("%d,RUNNING,pid=%d.%d,remaining_time=%g,cpu=%d\n", time, curr->data->pid, curr->data->cpu_id, curr->data->rtime, curr->data->cpu_id);
        } else {
            printf("%d,RUNNING,pid=%d,remaining_time=%g,cpu=%d\n", time, curr->data->pid, curr->data->rtime, curr->data->cpu_id);
        }

        if (curr->next == NULL) {
            return;
        }
        curr = curr->next;
    }
}


// Allocate process to N cpus
void multi_alloc(cpuList_t **cpu_lists, list_t *new_list, int *remain, int cpu_count) {
    node_t *curr = new_list->head;
    // No new process
    if (curr == NULL) {
        return;
    }
    while(1) {
        *remain += 1;
        // Sort the array of cpu lists
        insertionSort(cpu_lists, cpu_count);
        // parallelisable
        if (curr->data->parallel) {
            double num_sub = min(curr->data->rtime, cpu_count);
            for (int i=0; i<num_sub; i++) {
                process_t *sub = (process_t*)malloc(sizeof(*sub));
                copy_data(sub, curr->data);
                sub->rtime = ceil(curr->data->etime/num_sub)+1;
                sub->cpu_id = cpu_lists[i]->id;
                insert_at_foot(cpu_lists[i]->run_list, sub);
                cpu_lists[i]->remain_time = remain_time(cpu_lists[i]->run_list);
            }
        } else {
            // not parallelisable
            process_t *clone = (process_t*)malloc(sizeof(*clone));
            copy_data(clone, curr->data);
            clone->cpu_id = cpu_lists[0]->id;
            insert_at_foot(cpu_lists[0]->run_list, clone);
            cpu_lists[0]->remain_time = remain_time(cpu_lists[0]->run_list);
        }
        curr = curr->next;
        // No more new process
        if (curr == NULL) {
            return;
        }
    }
}


// Run the cpu in a unit time
list_t **multi_cpu_run(cpuList_t **cpu_lists, int cpu_count, int time, int *remain, int *complete, double *total_turnaround, double *total_overhead, double *max_overhead) {
    list_t **run_finish = malloc(sizeof(list_t *) * 2);
    list_t *run_list = make_empty_list();
    list_t *finish_list = make_empty_list();
    run_finish[0] = run_list;
    run_finish[1] = finish_list;

    for(int i=0; i<cpu_count; i++) {
        // No process on this cpu
        if (cpu_lists[i]->run_list->head == NULL) {
            continue;
        }

        node_t *prev_proc = cpu_lists[i]->run_list->head;
        merge_sort_rtime(&cpu_lists[i]->run_list->head);
        adjust_foot(cpu_lists[i]->run_list);
        // Check if the running process is the previous one
        node_t *running_proc = cpu_lists[i]->run_list->head;
        if (prev_proc != running_proc) {
            prev_proc->data->state = 0;
        }
        if (running_proc->data->state == 0) {
            process_t *clone = (process_t*)malloc(sizeof(*clone));
            copy_data(clone, running_proc->data);
            clone->cpu_id = cpu_lists[i]->id;
            insert_at_foot(run_list, clone);
            running_proc->data->state = 1;
        }

        // Execute the process and check if it is finished
        running_proc->data->rtime--;
        cpu_lists[i]->remain_time--;
        if (running_proc->data->rtime == 0) {
            if (!running_proc->data->parallel || ((running_proc->data->parallel) && (sub_finished(cpu_lists, cpu_count, i, running_proc->data->pid)))) {
                *remain -= 1;
                *complete += 1;
                double turnaround = time+1-running_proc->data->atime;
                double overhead = turnaround/running_proc->data->etime;
                *total_turnaround += turnaround;
                *total_overhead += overhead;
                if (overhead > *max_overhead) {
                    *max_overhead = overhead;
                }
                process_t *clone = (process_t*)malloc(sizeof(*clone));
                copy_data(clone, running_proc->data);
                insert_at_foot(finish_list, clone);
            }
            delete_head(cpu_lists[i]->run_list);
        }
    }
    return run_finish;
}


// Check if there is a free cpu
int free_cpu(cpuList_t **cpu_lists, int cpu_count) {
    int res = 0;
    for (int i = 0; i<cpu_count; i++) {
        if (cpu_lists[i]->remain_time == 0) {
            res++;
        }
    }
    return res;
}


// FInd optimal makespan base on future processes
int optimal_alloc(cpuList_t **cpu_lists, node_t *arrive, list_t *future_proc, int cpu_count, int time) {
    if (future_proc->head == NULL || cpu_count < 3 || cpu_count > 5) {
        return 0;
    }
    // Traverse all processes that will arrive within the current longest cpu's remaining time
    double range = cpu_lists[cpu_count-1]->remain_time;
    double min_remain = cpu_lists[0]->remain_time + arrive->data->rtime;
    int move = 0;
    int count = 0;
    node_t *node = future_proc->head;
    // Determine if the future process can be shorter than the current one if it is allocated to the shortest cpu queue
    while (node != NULL) {
        if (node->data->atime < time + range) {
            double prob_time = node->data->rtime + (node->data->atime - time) + cpu_lists[0]->remain_time;
            if (prob_time < min_remain) {
                min_remain = prob_time;
                move++;
            }
        }
        count++;
        if (count == cpu_count-1) {
            break;
        }
        node = node->next;
    }
    return move;
}


// Challenge: Allocate process to cpus
void c_alloc(cpuList_t **cpu_lists, list_t *new_list, int *remain, int cpu_count, list_t *future_proc, int time) {
    node_t *curr = new_list->head;
    // No new process
    if (curr == NULL) {
        return;
    }
    int new_count = len(new_list);
    while(1) {
        *remain += 1;
        // Sort the array of cpu lists base on remaining execution time
        insertionSort(cpu_lists, cpu_count);
        // parallelisable
        int free = free_cpu(cpu_lists, cpu_count);
        if (curr->data->parallel && new_count == 1 && free != 0 && curr->data->etime > cpu_count) {
            double k = free;
            double num_sub = min(curr->data->rtime, k);
            for (int i = 0; i < num_sub; i++) {
                process_t *sub = (process_t *) malloc(sizeof(*sub));
                copy_data(sub, curr->data);
                sub->rtime = ceil(curr->data->etime / num_sub) + 1;
                sub->cpu_id = cpu_lists[i]->id;
                insert_at_foot(cpu_lists[i]->run_list, sub);
                cpu_lists[i]->remain_time = remain_time(cpu_lists[i]->run_list);
            }
        } else {
            // not parallelisable
            if (new_count > 1 || (new_count == 1 && free > 0)) {
                process_t *clone = (process_t*)malloc(sizeof(*clone));
                copy_data(clone, curr->data);
                clone->cpu_id = cpu_lists[0]->id;
                clone->parallel = 0;
                insert_at_foot(cpu_lists[0]->run_list, clone);
                cpu_lists[0]->remain_time = remain_time(cpu_lists[0]->run_list);
            } else {
                int pos = optimal_alloc(cpu_lists, curr, future_proc, cpu_count, time);
                process_t *clone = (process_t*)malloc(sizeof(*clone));
                copy_data(clone, curr->data);
                clone->cpu_id = cpu_lists[pos]->id;
                clone->parallel = 0;
                insert_at_foot(cpu_lists[pos]->run_list, clone);
                cpu_lists[pos]->remain_time = remain_time(cpu_lists[pos]->run_list);
            }
        }
        new_count--;
        curr = curr->next;
        // No more new process
        if (curr == NULL) {
            return;
        }
    }
}


// Challenge: Run the cpu in a unit time
list_t **c_cpu_run(cpuList_t **cpu_lists, int cpu_count, int time, int *remain, int *complete, double *total_turnaround, double *total_overhead, double *max_overhead) {
    list_t **run_finish = malloc(sizeof(list_t *) * 2);
    list_t *run_list = make_empty_list();
    list_t *finish_list = make_empty_list();
    run_finish[0] = run_list;
    run_finish[1] = finish_list;

    for(int i=0; i<cpu_count; i++) {
        // No process on this cpu
        if (cpu_lists[i]->run_list->head == NULL) {
            continue;
        }

        node_t *prev_proc = cpu_lists[i]->run_list->head;
        merge_sort_rtime(&cpu_lists[i]->run_list->head);
        adjust_foot(cpu_lists[i]->run_list);
        // Check if the running process is the previous one
        node_t *running_proc = cpu_lists[i]->run_list->head;
        if (prev_proc != running_proc) {
            prev_proc->data->state = 0;
        }
        if (running_proc->data->state == 0) {
            process_t *clone = (process_t*)malloc(sizeof(*clone));
            copy_data(clone, running_proc->data);
            clone->cpu_id = cpu_lists[i]->id;
            insert_at_foot(run_list, clone);
            running_proc->data->state = 1;
        }

        // Execute the process and check if it is finished
        running_proc->data->rtime--;
        cpu_lists[i]->remain_time--;
        if (running_proc->data->rtime == 0) {
            if (!running_proc->data->parallel || ((running_proc->data->parallel) && (sub_finished(cpu_lists, cpu_count, i, running_proc->data->pid)))) {
                *remain -= 1;
                *complete += 1;
                double turnaround = time+1-running_proc->data->atime;
                double overhead = turnaround/running_proc->data->etime;
                *total_turnaround += turnaround;
                *total_overhead += overhead;
                if (overhead > *max_overhead) {
                    *max_overhead = overhead;
                }
                process_t *clone = (process_t*)malloc(sizeof(*clone));
                copy_data(clone, running_proc->data);
                insert_at_foot(finish_list, clone);
            }
            delete_head(cpu_lists[i]->run_list);
        }
    }
    return run_finish;
}
