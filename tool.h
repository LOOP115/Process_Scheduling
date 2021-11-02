#define FILENAME "-f"
#define PROCESSORS "-p"
#define CHALLENGE "-c"


typedef struct cpuList {
    list_t *run_list;       // processes on the cpu
    int id;                 // cpu id
    double remain_time;     // current remaining execution time
}cpuList_t;


process_t *data_extract(char* str);

int add_process(list_t *proc_list, list_t *running_list, int time);

void copy_data(process_t *dest, process_t *src);

void add_sort(list_t *proc_list, list_t *running_list, int time, int *remain);

void dual_alloc(list_t *cpu0_list, list_t *cpu1_list, list_t *new_list, int *remain);

double remain_time(list_t *cpu_list);

int id_exist(list_t *list, int pid);

double min(double x, double y);

void insertionSort(cpuList_t **cpu_lists, int n);

void dual_cpu_run(list_t *run_list, list_t *other_list, int time, int *remain, int *complete, int cpu_id, double *total_turnaround, double *total_overhead, double *max_overhead);

int sub_finished(cpuList_t **cpu_lists, int cpu_num, int curr_cpu, int pid);

void print_finished(list_t *list, int time, int remain);

void print_running(list_t *list, int time);

void multi_alloc(cpuList_t **cpu_lists, list_t *new_list, int *remain, int cpu_num);

list_t **multi_cpu_run(cpuList_t **cpu_lists, int cpu_num, int time, int *remain, int *complete, double *total_turnaround, double *total_overhead, double *max_overhead);

int free_cpu(cpuList_t **cpu_lists, int cpu_count);

int optimal_alloc(cpuList_t **cpu_lists, node_t *arrive, list_t *future_proc, int cpu_count, int time);

void c_alloc(cpuList_t **cpu_lists, list_t *new_list, int *remain, int cpu_num, list_t *future_proc, int time);

list_t **c_cpu_run(cpuList_t **cpu_lists, int cpu_num, int time, int *remain, int *complete, double *total_turnaround, double *total_overhead, double *max_overhead);
