typedef struct process {
    int atime;      // time-arrived
    int pid;        // process-id
    double etime;   // execution-time
    int parallel;   // parallelisable
    double rtime;   // remaining time
    int state;      // if the process is running
    int cpu_id;     // which cpu is the process running on
}process_t;


// Construct a linked list
typedef struct node node_t;

struct node {
    process_t *data;
    node_t *next;
};

typedef struct {
    node_t *head;
    node_t *foot;
} list_t;


list_t *make_empty_list(void);

void free_list(list_t *list);

void insert_at_foot(list_t *list, process_t *value);

void delete_head(list_t *list);

void merge_sort_rtime(node_t **head_node);

node_t* sorted_merge_rtime(node_t *a, node_t *b);

void merge_sort_cpuid(node_t **head_node);

node_t* sorted_merge_cpuid(node_t *a, node_t *b);

void merge_sort_pid(node_t **head_node);

node_t* sorted_merge_pid(node_t *a, node_t *b);

void split(node_t *src, node_t **front, node_t **back);

void adjust_foot(list_t *list);

int len(list_t *list);
