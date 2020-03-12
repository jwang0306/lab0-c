#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "natsort/strnatcmp.h"
#include "queue.h"

#ifndef strlcpy
#define strlcpy(dst, src, sz) snprintf((dst), (sz), "%s", (src))
#endif

#define compare(val1, val2) strnatcmp(val1, val2) < 0

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
    if (!q || q->size == 0 || !sp)
        return false;
    list_ele_t *tmp = q->head;
    q->head = q->head->next;  // remove head
    q->size--;
    strncpy(sp, tmp->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
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
static list_ele_t *merge(list_ele_t *list_l, list_ele_t *list_r)
{
    list_ele_t *head = NULL;
    list_ele_t **tmp = &head;

    while (list_l && list_r) {
        if (strnatcmp(list_l->value, list_r->value) < 0) {
            *tmp = list_l;
            list_l = list_l->next;
        } else {
            *tmp = list_r;
            list_r = list_r->next;
        }
        tmp = &((*tmp)->next);
    }
    if (list_l)
        *tmp = list_l;
    if (list_r)
        *tmp = list_r;

    return head;
}

static void split(list_ele_t *head, list_ele_t **list_l, list_ele_t **list_r)
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

static list_ele_t *merge_sort(list_ele_t *head, int q_size)
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
}

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