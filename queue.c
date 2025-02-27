#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *h = (struct list_head *) malloc(sizeof(struct list_head));
    if (!h)
        return NULL;
    INIT_LIST_HEAD(h);
    return h;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    struct list_head *n, *s;
    list_for_each_safe (n, s, head) {
        list_del(n);
        q_release_element(list_entry(n, element_t, list));
    }
    free(head);
}

#ifndef strlcpy
#define strlcpy(dst, src, sz) snprintf((dst), (sz), "%s", (src))
#endif

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;
    const int len = strlen(s) + 1;
    e->value = malloc(sizeof(char) * len);
    if (!e->value) {
        free(e);
        return false;
    }
    list_add(&e->list, head);
    strlcpy(e->value, s, len);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;
    const int len = strlen(s) + 1;
    e->value = malloc(sizeof(char) * len);
    if (!e->value) {
        free(e);
        return false;
    }
    list_add_tail(&e->list, head);
    strlcpy(e->value, s, len);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;
    element_t *e = list_entry(head->next, element_t, list);
    list_del(&e->list);
    if (sp) {
        strlcpy(sp, e->value, bufsize);
    }
    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;
    element_t *e = list_entry(head->prev, element_t, list);
    list_del(&e->list);
    if (sp) {
        strlcpy(sp, e->value, bufsize);
    }
    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || head->next == head)
        return 0;

    int cnt = 0;
    struct list_head *n;
    list_for_each (n, head)
        cnt++;
    return cnt;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || head->next == head)
        return false;
    struct list_head *f = head, *s;
    list_for_each (s, head) {
        if (f->next == head)
            break;
        if (f->next->next == head)
            break;
        f = f->next->next;
    }
    list_del(s);
    q_release_element(list_entry(s, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || head->next == head)
        return;
    struct list_head *n, *s;
    list_for_each_safe (n, s, head)
        list_move(n, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

#define qu(node) list_entry(node, queue_contex_t, chain)->q

/* Merge two sorted queues into a sorted non-empty queue with specified head.
 */
void merge_two_list(struct list_head *qa,
                    struct list_head *qb,
                    struct list_head *head_to)
{
    struct list_head *ta = (qa->prev != qa ? qa->prev : NULL);
    struct list_head *tb = (qb->prev != qb ? qb->prev : NULL);

    int na = q_size(qa), nb = q_size(qb);

    while (na && nb) {
        const char *va = list_entry(qa->next, element_t, list)->value;
        const char *vb = list_entry(qb->next, element_t, list)->value;

        if (strcmp(va, vb) < 0) {
            list_move_tail(qa->next, head_to);
            na--;
        } else {
            list_move_tail(qb->next, head_to);
            nb--;
        }
    }

    if (na || nb) {
        struct list_head **head_from = (na ? &qa : &qb);
        struct list_head **tail_from = (na ? &ta : &tb);
        struct list_head tmp;
        list_cut_position(&tmp, *head_from, *tail_from);
        list_splice_tail(&tmp, head_to);
    }
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (list_empty(head) || list_is_singular(head))
        return 0;

    int len = q_size(head);

    while (len > 1) {
        struct list_head *slow = head, *fast = head;
        for (int i = 0; i < len / 2; i++) {
            slow = slow->next;
            fast = fast->next->next;
            merge_two_list(qu(fast->prev), qu(fast), qu(slow));
        }

        if (len % 2 == 1)
            list_splice_init(qu(fast->next), qu(slow->next));

        len = (len + 1) / 2;
    }

    if (descend)
        q_reverse(qu(head->next));

    return q_size(qu(head->next));
}
