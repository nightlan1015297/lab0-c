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
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head) {
        INIT_LIST_HEAD(head);
    }
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }
    element_t *pos, *safe;
    list_for_each_entry_safe (pos, safe, head, list) {
        q_release_element(pos);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *node = malloc(sizeof(element_t));
    if (!node) {
        return false;
    }
    size_t len = strlen(s) + 1;
    char *val = malloc(len * sizeof(char));
    if (!val) {
        free(node);
        return false;
    }

    node->value = strncpy(val, s, len);
    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *node = malloc(sizeof(element_t));
    if (!node) {
        return false;
    }
    size_t len = strlen(s) + 1;
    char *val = malloc(len * sizeof(char));
    if (!val) {
        free(node);
        return false;
    }

    node->value = strncpy(val, s, len);
    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *node = container_of(head->next, element_t, list);
    list_del_init(head->next);

    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *node = container_of(head->prev, element_t, list);
    list_del_init(head->prev);

    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head) {
        return 0;
    }
    int count = 0;
    struct list_head *node;
    list_for_each (node, head) {
        count++;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *slow = head->next, *fast = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    element_t *node = container_of(slow, element_t, list);
    list_del(slow);
    q_release_element(node);
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
    if (!head || list_empty(head)) {
        return;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *cur = head->next;
    while (cur->next != head || cur != head) {
        list_move_tail(cur->next, cur);
        cur = cur->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *pos, *safe, *tmp;
    list_for_each_safe (pos, safe, head) {
        tmp = pos->prev;
        pos->prev = pos->next;
        pos->next = tmp;
    }
    tmp = head->prev;
    head->prev = head->next;
    head->next = tmp;
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
    if (!head || list_empty(head)) {
        return 0;
    }
    int cnt = 1;
    struct list_head *cur, *tar;
    cur = head->prev;
    tar = cur->prev;
    while (tar != head) {
        if (strcmp(container_of(cur, element_t, list)->value,
                   container_of(tar, element_t, list)->value) <= 0) {
            list_del(tar);
            tar = cur->prev;
        } else {
            cur = tar;
            tar = cur->prev;
            cnt++;
        }
    }
    return cnt;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return 0;
    }
    int cnt = 1;
    struct list_head *cur, *tar;
    cur = head->prev;
    tar = cur->prev;
    while (tar != head) {
        if (strcmp(container_of(cur, element_t, list)->value,
                   container_of(tar, element_t, list)->value) >= 0) {
            list_del(tar);
            tar = cur->prev;
        } else {
            cur = tar;
            tar = cur->prev;
            cnt++;
        }
    }
    return cnt;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
