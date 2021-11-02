#include <stdlib.h>
#include <assert.h>

#include "list.h"


/* Some of list functions are based on codes from COMP10002 */

// Create an empty list
list_t *make_empty_list(void) {
    list_t *list;
    list = (list_t*)malloc(sizeof(*list));
    assert(list!=NULL);
    list->head = list->foot = NULL;
    return list;
}


// Free the linked list
void free_list(list_t *list) {
    node_t *curr, *prev;
    assert(list!=NULL);
    curr = list->head;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev->data);
        free(prev);
    }
    free(list);
    list = NULL;
}


// Insert a node at foot
void insert_at_foot(list_t *list, process_t *value) {
    node_t *new = (node_t*)malloc(sizeof(*new));
    assert(list!=NULL && new!=NULL);
    new->data = value;
    new->next = NULL;
    if (list->foot==NULL) {
        // First insertion into the list
        list->head = list->foot = new;
    } else {
        list->foot->next = new;
        list->foot = new;
    }
}


// Delete head node
void delete_head(list_t *list) {
    assert(list!=NULL);
    node_t *node = list->head;
    if (list->head->next == NULL) {
        list->foot = NULL;
    }
    list->head = list->head->next;
    free(node->data);
    free(node);
}


// Sort the list using merge sort base on remaining time
void merge_sort_rtime(node_t **head_node) {
    node_t *head = *head_node;
    node_t *a;
    node_t *b;
    // Base case
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }
    // Split the list
    split(head, &a, &b);
    // Sort the lists recursively
    merge_sort_rtime(&a);
    merge_sort_rtime(&b);
    // merge sorted lists;
    *head_node = sorted_merge_rtime(a, b);
}


// Merge sorted lists base on remaining time;
node_t* sorted_merge_rtime(node_t *a, node_t *b) {
    node_t *res = NULL;
    // Base case
    if (a == NULL) {
        return b;
    } else if (b == NULL) {
        return a;
    }
    // merge
    if (a->data->rtime < b->data->rtime || ((a->data->rtime == b->data->rtime) && (a->data->pid < b->data->pid))) {
        res = a;
        res->next = sorted_merge_rtime(a->next, b);
    } else {
        res = b;
        res->next = sorted_merge_rtime(a, b->next);
    }
    return res;
}


// Sort the list using merge sort base on cpu_id
void merge_sort_cpuid(node_t **head_node) {
    node_t *head = *head_node;
    node_t *a;
    node_t *b;
    // Base case
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }
    // Split the list
    split(head, &a, &b);
    // Sort the lists recursively
    merge_sort_cpuid(&a);
    merge_sort_cpuid(&b);
    // merge sorted lists;
    *head_node = sorted_merge_cpuid(a, b);
}


// Merge sorted lists base on cpu_id;
node_t* sorted_merge_cpuid(node_t *a, node_t *b) {
    node_t *res = NULL;
    // Base case
    if (a == NULL) {
        return b;
    } else if (b == NULL) {
        return a;
    }
    // merge
    if (a->data->cpu_id < b->data->cpu_id) {
        res = a;
        res->next = sorted_merge_cpuid(a->next, b);
    } else {
        res = b;
        res->next = sorted_merge_cpuid(a, b->next);
    }
    return res;
}


// Sort the list using merge sort base on pid
void merge_sort_pid(node_t **head_node) {
    node_t *head = *head_node;
    node_t *a;
    node_t *b;
    // Base case
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }
    // Split the list
    split(head, &a, &b);
    // Sort the lists recursively
    merge_sort_pid(&a);
    merge_sort_pid(&b);
    // merge sorted lists;
    *head_node = sorted_merge_pid(a, b);
}


// Merge sorted lists base on pid;
node_t* sorted_merge_pid(node_t *a, node_t *b) {
    node_t *res = NULL;
    // Base case
    if (a == NULL) {
        return b;
    } else if (b == NULL) {
        return a;
    }
    // merge
    if (a->data->pid < b->data->pid) {
        res = a;
        res->next = sorted_merge_cpuid(a->next, b);
    } else {
        res = b;
        res->next = sorted_merge_cpuid(a, b->next);
    }
    return res;
}


// Split the list into two halves
void split(node_t *src, node_t **front, node_t **back) {
    node_t *fast;
    node_t *slow;
    slow = src;
    fast = src->next;

    // fast/slow pointer strategy
    while(fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    *front = src;
    *back = slow->next;
    slow->next = NULL;
}


// Change the foot of the linked list after sort
void adjust_foot(list_t *list) {
    node_t *curr = list->head;
    if (curr == NULL) {
        return;
    }
    node_t *next = curr->next;

    while (1) {
        if (next == NULL) {
            list->foot = curr;
            return;
        }
        curr = curr->next;
        next = next->next;
    }
}


// Get the length of a linked list
int len(list_t *list) {
    int count = 0;
    if (list->head == NULL) {
        return 0;
    }
    node_t *node = list->head;
    while(node != NULL) {
        count++;
        node = node->next;
    }
    return count;
}
