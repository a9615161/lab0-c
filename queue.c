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
    /* note that list_entry() - Calculate address of entry that contains list
     * node
     * Every list_head* is stored in the entry(element_t),except the head
     * The head pointing to the head and tail of the queue,
     * pointing to itself when the q is empty.
     */
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head)
        INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    /* note that list_for_each_entry_safe - iterate over list entries and allow
     * to delete entry
     * entry:element_t , the iterator
     * safe :the ptr to next entry, allowing us to
     * delete entry while iterating
     * head : head of the list
     * member : name element_t uses to ref the list ptrs
     */
    if (l == NULL)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list)
        q_release_element(entry);
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool iniNode(char *s, element_t **newNode)
{
    *newNode = (element_t *) malloc(sizeof(element_t));
    char *newVal = strdup(s);
    if (*newNode && newVal) {
        (*newNode)->value = newVal;
        (*newNode)->list.next = NULL;
        (*newNode)->list.prev = NULL;
        return true;
    } else
        return false;
}
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *newNode = NULL;
    if (!head || !s)
        return false;
    if (!iniNode(s, &newNode))
        return false;
    list_add(&(newNode->list), head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *newNode = NULL;
    if (!head || !s)
        return false;
    if (!iniNode(s, &newNode))
        return false;
    list_add_tail(&(newNode->list), head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *remove_Node(element_t *node, char *sp, size_t bufsize)
{
    if (!node)
        return NULL;
    // copy the value to sp
    if (sp && bufsize != 0) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del_init(&(node->list));
    return node;
}
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // queue is NULL or empty
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_first_entry(head, element_t, list);
    return remove_Node(node, sp, bufsize);
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    // queue is NULL or empty
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_last_entry(head, element_t, list);
    return remove_Node(node, sp, bufsize);
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


/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // list is NULL or empty
    if (!head || list_empty(head))
        return false;
    struct list_head *forwarding = head->next;
    struct list_head *backwarding = head->prev;
    // Erasing the ⌊n / 2⌋th node , move foward first.
    bool movingf = true;
    while (forwarding != backwarding) {
        if (movingf)
            forwarding = forwarding->next;
        else
            backwarding = backwarding->prev;
        movingf = !movingf;
    }
    element_t *tmp = list_entry(forwarding, element_t, list);
    list_del_init(forwarding);
    q_release_element(tmp);
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool isDup(element_t *a, element_t *b)
{
    return !strcmp(a->value, b->value);
}
bool q_delete_dup(struct list_head *head)
{
    // list is NULL
    if (!head)
        return false;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        // note that safe->list could be head
        if (&safe->list == head)
            break;
        if (isDup(entry, safe)) {
            list_del(&(entry->list));
            q_release_element(entry);
        }
    }
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // this is done by chiangkd
    if (!head || list_empty(head))
        return;
    struct list_head *n = head->next;
    while (n != head && n->next != head) {
        struct list_head *t = n;
        list_move(n, t->next);
        n = n->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    // swapping the next and prev of each node

    struct list_head *node, *safe, *tmp;
    list_for_each_safe (node, safe, head) {
        tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
    }
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
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

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
