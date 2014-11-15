#include "list.h"

/*----- List Functions -----*/

// Function is responsible for creating a list struct.
list *create_list(bool threaded) {
    list *lst = (list *) malloc(sizeof(list));

    if (lst) {
        lst->head = NULL;
        lst->size = 0;
        lst->threaded = threaded;
        if (threaded) {
            // List is going to be shared betweent threads.
            lst->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
            lst->signal = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
            pthread_mutex_init(lst->mutex, NULL);
            pthread_cond_init(lst->signal, NULL);
        } else {
            // List will not be shared between threads.
            lst->mutex = NULL;
            lst->signal = NULL;
        }
    }

    return lst;
}

void lpush(list *lst, order *data) {
    // Validate given parameters.
    if (!lst || !data) {
        return;
    }

    // Check if list is begin used in a multithreaded environment,
    // and lock its mutex if so.
    list_node *node = create_list_node(data);
    if (lst->threaded) {
        pthread_mutex_lock(lst->mutex);
    }

    // Push data into list at head and increment size.
    if (lst->head) {
        node->next = lst->head;
        lst->head->prev = node;
        lst->head = node;
    } else {
        lst->head = node;
        lst->tail = node;
    }
    lst->size++;

    // Check if list is being used in a multithreaded environment, and
    // if so, unlock its mutex, and signal consumer thread that data has
    // been added.
    if (lst->threaded) {
        pthread_cond_signal(lst->signal);
        pthread_mutex_unlock(lst->mutex);
    }
}

order *rpop(list *lst) {
    // Validate given parameters and return immediately if given
    // list is empty and not slated for multithreaded use.
    if (!lst || (!lst->size && !lst->threaded)) {
        return NULL;
    }

    // Check if list is being used in a multithreaded environment, and
    // if so, check if the list currently contains any orders. If not,
    // release its lock and wait until signaled by the producer that data
    // has been added.
    if (lst->threaded) {
        pthread_mutex_lock(lst->mutex);
        if (!lst->size) {
            pthread_cond_wait(lst->signal, lst->mutex);
        }
    }

    // Pop data off the end of the queue and decrement size.
    list_node *node = lst->tail;
    if (lst->size > 1) {
        lst->tail = lst->tail->prev;
        lst->tail->next = NULL;
    } else {
        lst->head = NULL;
        lst->tail = NULL;
    }
    lst->size--;

    // Check if list is being used in a multithreaded environemnt, and
    // unlock its mutex if so.
    if (lst->threaded) {
        pthread_mutex_unlock(lst->mutex);
    }

    // Isolate the data, destroy the node, and return.
    order *book = node->data;
    destroy_list_node(node);
    return book;
}

// Function is responsible for destroying a list.
void destroy_list(list *lst) {
    // If list contains data, iterate across it, freeing nodes as we go.
    if (lst->head) {
        list_node *current = lst->head;
        while (current) {
            list_node *tmp = current;
            current = current->next;
            destroy_order(tmp->data);
            destroy_list_node(tmp);
        }
    }

    // Release the list's mutex if it has one.
    if (lst->mutex) {
        pthread_mutex_destroy(lst->mutex);
    }

    free(lst);
}

/*----- List Node Functions -----*/

// Function is responsible for creating a list node struct.
list_node *create_list_node(order *data) {
    list_node *node = (list_node *) malloc(sizeof(list_node));

    if (node) {
        node->data = data;
        node->next = NULL;
        node->prev = NULL;
    }

    return node;
}

// Function is responsible for destroying a list node.
void destroy_list_node(list_node *node) {
    free(node);
}
