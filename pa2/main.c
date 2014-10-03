#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "sorted-list.h"

int compareInts(void *p1, void *p2) {
    int i1 = *(int*)p1;
    int i2 = *(int*)p2;

    return i1 - i2;
}

int compareDoubles(void *p1, void *p2) {
    double d1 = *(double*)p1;
    double d2 = *(double*)p2;

    return (d1 < d2) ? -1 : ((d1 > d2) ? 1 : 0);
}

int compareStrings(void *p1, void *p2) {
    char *s1 = p1;
    char *s2 = p2;

    return strcmp(s1, s2);
}

//Destructor functions
void destroyBasicTypeAlloc(void *p) {
    //For pointers to basic data types (int*,char*,double*,...)
    //Use for allocated memory (malloc,calloc,etc.)
    free(p);
}

void destroyBasicTypeNoAlloc(void *p) {
    //For pointers to basic data types (int*,char*,double*,...)
    //Use for memory that has not been allocated (e.g., "int x = 5;SLInsert(mylist,&x);SLRemove(mylist,&x);")
    return;
}


int main() {
    SortedList *list_one = SLCreate(&compareInts, &destroyBasicTypeAlloc);
    SortedList *list_two = SLCreate(&compareStrings, &destroyBasicTypeAlloc);
    SortedList *list_three = SLCreate(&compareInts, &destroyBasicTypeAlloc);
    SortedList *list_four = SLCreate(&compareStrings, &destroyBasicTypeAlloc);
    for (int i = 1, k = 126; i < 127; i++, k--) {
        int *temp_one = (int *) malloc(sizeof(int)), *temp_two = (int *) malloc(sizeof(int));
        char *str_one = (char *) malloc(sizeof(char) * 6), *str_two = (char *) malloc(sizeof(char) * 6);
        for (int j = 0; j < 5; j++) {
            str_one[j] = (char) i;
            str_two[j] = (char) k;
        }
        str_one[5] = '\0';
        str_two[5] = '\0';
        *temp_one = i;
        *temp_two = k;
        SLInsert(list_one, temp_one);
        SLInsert(list_two, str_one);
        SLInsert(list_three, temp_two);
        SLInsert(list_four, str_two);
    }

    SortedListIterator *it_one = SLCreateIterator(list_one), *it_two = SLCreateIterator(list_two);
    SortedListIterator *it_three = SLCreateIterator(list_three), *it_four = SLCreateIterator(list_four);

    int *data_one = (int *) SLNextItem(it_one), *data_three = (int *) SLNextItem(it_three);
    char *data_two = (char *) SLNextItem(it_two), *data_four = (char *) SLNextItem(it_four);
    int *prev_one = data_one, prev_three = *data_three;
    char *prev_two = malloc(sizeof(char) * 6), *prev_four = data_four;
    strcpy(prev_two, data_two);
    int current_size = 126;

    while (data_one || data_two || data_three || data_four) {
        if (data_one) {
            data_one = SLNextItem(it_one);
            if (data_one) {
                assert(*prev_one >= *data_one);
            }
            prev_one = data_one;
        }
        if (data_two) {
            data_two = SLNextItem(it_two);
            if (data_two) {
                assert(strcmp(prev_two, data_two) >= 0);
                strcpy(prev_two, data_two);
                int random = rand() % current_size;
                char tmp_str[6];
                for (int i  = 0; i < 5; i++) {
                    tmp_str[i] = (char) random;
                }
                tmp_str[5] = '\0';
                SLRemove(list_two, &tmp_str);
                current_size--;
            }
        }
        if (data_three) {
            SLRemove(list_three, data_three);
            data_three = SLNextItem(it_three);
            if (data_three) {
                assert(prev_three >= *data_three);
                prev_three = *data_three;
            }
        }
        if (data_four) {
            data_four = SLNextItem(it_four);
            if (data_four) {
                assert(*prev_four >= *data_four);
            }
            prev_four = data_four;
        }
    }

    return 0;
}
