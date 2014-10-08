#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"
#include "nodes.h"
#include "hash.h"

int main() {
    hash *table = create_hash();
    index_node *first = create_index_node("world.txt");
    put(table, "hello", create_hash_node("hello", first));
    hash_node *found = get(table, "hello");
    drop(table, "hello");
    destroy_hash(table);
    return 0;
}
