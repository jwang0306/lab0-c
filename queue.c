#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#ifndef strlcpy
#define strlcpy(dst, src, sz) snprintf((dst), (sz), "%s", (src))
#endif

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;
    if (q->size > 0) {
        list_ele_t *curr = q->head;
        list_ele_t *prev = NULL;
        while (curr) {
            prev = curr;
            curr = curr->next;
            free(prev->value);
            free(prev);
        }
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;
    list_ele_t *newh;
    newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;
    newh->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!newh->value) {
        free(newh);
        return false;
    }
    memcpy(newh->value, s, (strlen(s) + 1));
    if (q->size == 0) {
        newh->next = NULL;
        q->head = q->tail = newh;
    } else {
        newh->next = q->head;  // insert to head
        q->head = newh;        // update head
    }
    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;
    list_ele_t *newt;
    newt = malloc(sizeof(list_ele_t));
    if (!newt)
        return false;
    newt->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!newt->value) {
        free(newt);
        return false;
    }
    memcpy(newt->value, s, (strlen(s) + 1));
    newt->next = NULL;
    if (q->size == 0) {
        q->head = q->tail = newt;
    } else {
        q->tail->next = newt;  // insert to tail
        q->tail = newt;        // update tail
    }
    q->size++;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || q->size == 0)
        return false;
    list_ele_t *tmp = q->head;
    q->head = q->head->next;  // remove head
    q->size--;
    if (sp) {
        strncpy(sp, tmp->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    tmp->next = NULL;
    free(tmp->value);
    free(tmp);
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return (!q) ? 0 : q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || q->size <= 1)
        return;
    list_ele_t *curr = q->head;
    list_ele_t *next = NULL;
    list_ele_t *prev = NULL;
    while (curr != NULL) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    q->tail = q->head;
    q->head = prev;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || q->size <= 1)
        return;
    /* Merge Sort */
    q->head = merge_sort(q->head, q->size);

    list_ele_t *tmp = q->head;
    while (tmp->next) {
        tmp = tmp->next;
    }
    q->tail = tmp;
}

list_ele_t *merge_sort(list_ele_t *head, int q_size)
{
    if (!head || !head->next)
        return head;

    // /* Split the list */
    list_ele_t *list_l = NULL;
    list_ele_t *list_r = NULL;
    split(head, &list_l, &list_r);

    /* Sort the list*/
    list_l = merge_sort(list_l, q_size);
    list_r = merge_sort(list_r, q_size);

    /* Merge the list*/
    return merge(list_l, list_r);

    /* Iterative version */
    // list_ele_t *tmp_head = NULL;
    // for (int block_size = 1; block_size < q_size; block_size *= 2) {
    //     tmp_head = head;
    //     for (int start = 0; start < q_size - 1; start += 2 * block_size) {
    //         /* move tmp head to next block*/
    //         for (int i = 1; i < start && tmp_head->next; ++i) {
    //             tmp_head = tmp_head->next;
    //         }
    //         /*Split the list according to block size*/
    //         list_ele_t *slow = tmp_head;
    //         list_ele_t *fast = tmp_head->next;
    //         for (int i = 0; i < block_size && fast && fast->next; ++i) {
    //             slow = slow->next;
    //             fast = fast->next->next;
    //         }

    //         list_l = slow->next;
    //         list_r = tmp_head;
    //         slow->next = NULL;  // to actually cut the list

    //         /* Sort and merge splitted list */
    //         tmp_head = merge(list_l, list_r);
    //         if (start == 0) {
    //             head = tmp_head; // store the head of the first list
    //         }
    //     }
    // }

    return head;
}

bool smaller_than(list_ele_t *ele_l, list_ele_t *ele_r)
{
    return (strcmp(ele_l->value, ele_r->value) < 0) ? true : false;
}

list_ele_t *merge(list_ele_t *list_l, list_ele_t *list_r)
{
    if (!list_l)
        return list_l;
    if (!list_r)
        return list_r;

    list_ele_t *tmp = NULL;
    list_ele_t *head = NULL;

    /* Compare each element and link together */
    while (list_l && list_r) {
        if (smaller_than(list_l, list_r)) {
            if (tmp) {
                tmp->next = list_l;
                tmp = tmp->next;
            } else {  // first access
                tmp = list_l;
                head = tmp;
            }
            list_l = list_l->next;
        } else {
            if (tmp) {
                tmp->next = list_r;
                tmp = tmp->next;
            } else {  // first access
                tmp = list_r;
                head = tmp;
            }
            list_r = list_r->next;
        }
    }
    if (list_l)
        tmp->next = list_l;
    if (list_r)
        tmp->next = list_r;

    return head;
}

void split(list_ele_t *head, list_ele_t **list_l, list_ele_t **list_r)
{
    list_ele_t *slow = head;
    list_ele_t *fast = head->next;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    *list_l = slow->next;
    *list_r = head;
    slow->next = NULL;  // to actually cut the list
}