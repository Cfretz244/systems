#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "definitions.h"
#include "nodes.h"
#include "hash.h"
#include "splitter.h"

int main() {
    hash *table = create_hash();
    assert(table);
    for (int i = 0; i < 100000; i++) {
        char asdf[101], fdsa[101];
        for (int k = 0; k < 100; k++) {
            asdf[k] = (char) (rand() % 26) + 97;
            fdsa[k] = (char) (rand() % 26) + 97;
        }
        asdf[100] = '\0';
        fdsa[100] = '\0';
        index_node *tmp = create_index_node(asdf);
        put(table, fdsa, create_hash_node(fdsa, tmp));
    }
    index_node *first = create_index_node("world.txt");
    put(table, "hello", create_hash_node("hello", first));
    hash_node *found = get(table, "hello");
    drop(table, "hello");
    destroy_hash(table);

    splitter *split = create_splitter("test.txt");
    assert(split);
    char *token;
    while (token = split_next(split)) {
        printf("%s\n", token);
        free(token);
    }
    destroy_splitter(split);

    splitter *nosplit = create_splitter("doesntexist.txt");
    assert(!nosplit);

    return 0;
}
