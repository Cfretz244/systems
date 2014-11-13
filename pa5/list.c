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
    }

    return node;
}

void destroy_list_node(list_node *node) {
    destroy_order(node->data);
    free(node);
}
