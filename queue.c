#include <stdint.h>
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
    if (!head) {
        return false;
    }
    if (list_empty(head) || list_is_singular(head)) {
        return true;
    }
    struct list_head *ptr = head->next;
    bool need_del = false;
    while (ptr->next != head && ptr != head) {
        element_t *cur = container_of(ptr, element_t, list);
        element_t *next = container_of(ptr->next, element_t, list);
        bool cmp = strcmp(cur->value, next->value) == 0;
        if (cmp) {
            list_del(&cur->list);
            q_release_element(cur);
            need_del = cmp;
        } else if (need_del) {
            list_del(&cur->list);
            q_release_element(cur);
            need_del = cmp;
        }
        ptr = &next->list;
    }
    if (need_del) {
        element_t *cur = container_of(ptr, element_t, list);
        list_del(&cur->list);
        q_release_element(cur);
    }
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
    while (true) {
        list_move_tail(cur->next, cur);
        cur = cur->next;
        if (cur != head || cur->next != head)
            break;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *pos, *safe;
    list_for_each_safe (pos, safe, head) {
        list_move(pos, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k <= 1) {
        return;
    }

    LIST_HEAD(tmp);
    int cnt = 1;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        list_del(node);
        list_add(node, &tmp);
        if (cnt == k) {
            list_splice_tail(&tmp, node);
            cnt = 0;
        }
        cnt++;
    }
    if (cnt != 1) {
        q_reverse(&tmp);
        list_splice_tail(&tmp, head);
    }
}

/* Merge two queues into one sorted queue which is in ascending/descending
 * order */
struct list_head *__q_merge_two(struct list_head *L1,
                                struct list_head *L2,
                                bool descend)
{
    if (L1 == L2) {
        return L1;
    }
    struct list_head *head = NULL, **ptr = &head, **node;

    for (node = NULL; L1 && L2; *node = (*node)->next) {
        bool cmp =
            descend ^ (strcmp(list_entry(L1, element_t, list)->value,
                              list_entry(L2, element_t, list)->value) < 0);
        node = cmp ? &L1 : &L2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) L1 | (uintptr_t) L2);
    return head;
}

struct list_head *__q__sort(struct list_head *head, bool descend)
{
    if (!head || !head->next) {
        return head;
    }
    struct list_head *slow = head, *fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    // Unlink the first half from the second half
    slow->next = NULL;
    head = __q__sort(head, descend);
    fast = __q__sort(fast, descend);
    return __q_merge_two(head, fast, descend);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || head->next == head) {
        return;
    }
    // Unloop the queue
    head->prev->next = NULL;
    head->next = __q__sort(head->next, descend);
    struct list_head *curr = head;
    while (curr->next) {
        // Update prev
        curr->next->prev = curr;
        curr = curr->next;
    }
    curr->next = head;
    head->prev = curr;
    return;
}

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

/* Remove every node which has a node with a strictly greater value anywhere
 * to the right side of it */
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


/* Merge all the queues into one sorted queue, which is in
 * ascending/descending order */
int q_merge(struct list_head *head, bool descend)
{
    return 0;
}