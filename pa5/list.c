#include "list.h"

/*----- List Functions -----*/

list *create_list(bool threaded) {
    list *lst = (list *) malloc(sizeof(list));

    if (lst) {
        lst->head = NULL;
        lst->size = 0;
        lst->threaded = threaded;
        if (threaded) {
            pthread_mutex_init(lst->mutex, NULL);
        } else {
            lst->mutex = NULL;
        }
    }

    return lst;
}

void lpush(list *lst, order *data) {
    if (!lst || !data) {
        return;
    }

    list_node *node = create_list_node(data);
    pthread_mutex_lock(lst->mutex);

    if (lst->head) {
        node->next = lst->head;
        lst->head->prev = node;
        lst->head = node;
    } else {
        lst->head = node;
        lst->tail = node;
    }
    lst->size++;

    pthread_mutex_unlock(lst->mutex);
}

order *rpop(list *lst) {
    if (!lst || !lst->head) {
        return NULL;
    }

    pthread_mutex_lock(lst->mutex);

    list_node *node = lst->tail;
    lst->tail = lst->tail->prev;
    lst->tail->next = NULL;
    lst->size--;

    pthread_mutex_unlock(lst->mutex);
    order *book = node->data;
    destroy_list_node(node);
    return book;
}

void destroy_list(list *lst) {
    if (lst->head) {
        list_node *current = lst->head;
        while (current) {
            list_node *tmp = current;
            current = current->next;
            destroy_list_node(tmp);
        }
    }

    if (lst->mutex) {
        pthread_mutex_destroy(lst->mutex);
    }

    free(lst);
}

/*----- List Node Functions -----*/

list_node *create_list_node(order *data) {
    list_node *node = (list_node *) malloc(sizeof(list_node));

    if (node) {
        node->data = data;
        node->next = NULL;
        node->prev = NULL;
    }

    return node;
}

void destroy_list_node(list_node *node) {
    free(node);
}
