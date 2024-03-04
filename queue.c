#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an element */
element_t *ele_new(char *s)
{
    element_t *ele = (element_t *) malloc(sizeof(element_t));
    if (!ele)
        return NULL;

    ele->value = strdup(s);
    if (!ele->value) {
        free(ele);
        return NULL;
    }


    INIT_LIST_HEAD(&ele->list);
    return ele;
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *entry, *tmp;
    list_for_each_entry_safe (entry, tmp, head, list) {
        list_del(&entry->list);
        free(entry->value);
        free(entry);
    }

    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *ele = ele_new(s);
    if (!ele)
        return false;

    list_add(&ele->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *ele = ele_new(s);
    if (!ele)
        return false;

    list_add_tail(&ele->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head) || !sp)
        return NULL;

    element_t *first_elem = list_first_entry(head, element_t, list);
    if (bufsize > 0) {
        strncpy(sp, first_elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(&first_elem->list);
    return first_elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head) || !sp)
        return NULL;

    element_t *tail_elem = list_last_entry(head, element_t, list);
    if (bufsize > 0) {
        strncpy(sp, tail_elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(&tail_elem->list);
    return tail_elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *fast = head->next;
    struct list_head *slow = head->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

    list_del(slow);
    element_t *entry = list_entry(slow, element_t, list);
    free(entry->value);
    free(entry);
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
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *cur, *next;
    list_for_each_safe (cur, next, head)
        list_move(cur, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int K)
{
    int k = K;
    int cnt = q_size(head);
    struct list_head tmp;
    // struct list_head **last = &head->prev;
    while (cnt > 0) {
        struct list_head *node = head;
        INIT_LIST_HEAD(&tmp);
        while (k--) {
            node = node->next;
        }
        list_cut_position(&tmp, head, node);
        if (cnt / K)
            q_reverse(&tmp);
        list_splice_tail(&tmp, head);

        cnt -= K;
        k = (cnt / K) ? K : cnt;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head list_less, list_greater;
    INIT_LIST_HEAD(&list_less);
    INIT_LIST_HEAD(&list_greater);

    element_t *pivot;
    pivot = list_first_entry(head, element_t, list);
    list_del(&pivot->list);

    element_t *cur, *safe;
    list_for_each_entry_safe (cur, safe, head, list) {
        if (strcmp(cur->value, pivot->value) < 0)
            list_move_tail(&cur->list, &list_less);
        else
            list_move_tail(&cur->list, &list_greater);
    }

    q_sort(&list_less, descend);
    q_sort(&list_greater, descend);

    list_add(&pivot->list, head);
    if (descend) {
        list_splice(&list_greater, head);
        list_splice_tail(&list_less, head);
    } else {
        list_splice(&list_less, head);
        list_splice_tail(&list_greater, head);
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    char *max_val = NULL;
    struct list_head *cur, *safe;
    // after reverse, keep this big & delete ones bigger than it
    for (cur = (head)->prev, safe = cur->prev; cur != (head);
         cur = safe, safe = cur->prev) {
        element_t *cur_ele = list_entry(cur, element_t, list);
        if (cur != head->prev && strcmp(max_val, cur_ele->value) < 0) {
            list_del(cur);
            free(cur_ele->value);
            free(cur_ele);
        } else {
            free(max_val);
            max_val = strdup(cur_ele->value);
        }
    }

    free(max_val);
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    char *max_val = NULL;
    struct list_head *cur, *safe;
    // after reverse, keep this big & delete ones bigger than it
    for (cur = (head)->prev, safe = cur->prev; cur != (head);
         cur = safe, safe = cur->prev) {
        element_t *cur_ele = list_entry(cur, element_t, list);
        if (cur != head->prev && strcmp(max_val, cur_ele->value) > 0) {
            list_del(cur);
            free(cur_ele->value);
            free(cur_ele);
        } else {
            free(max_val);
            max_val = strdup(cur_ele->value);
        }
    }

    free(max_val);
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
