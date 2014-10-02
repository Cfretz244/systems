/*
 * sorted-list.c
 */

#include	<string.h>
#include	"sorted-list.h"

int compareInts(void *p1, void *p2)
{
	int i1 = *(int*)p1;
	int i2 = *(int*)p2;

	return i1 - i2;
}

int compareDoubles(void *p1, void *p2)
{
	double d1 = *(double*)p1;
	double d2 = *(double*)p2;

	return (d1 < d2) ? -1 : ((d1 > d2) ? 1 : 0);
}

int compareStrings(void *p1, void *p2)
{
	char *s1 = p1;
	char *s2 = p2;

	return strcmp(s1, s2);
}

//Destructor functions
void destroyBasicTypeAlloc(void *p){
	//For pointers to basic data types (int*,char*,double*,...)
	//Use for allocated memory (malloc,calloc,etc.)
	free(p);
}

void destroyBasicTypeNoAlloc(void *p) {
	//For pointers to basic data types (int*,char*,double*,...)
	//Use for memory that has not been allocated (e.g., "int x = 5;SLInsert(mylist,&x);SLRemove(mylist,&x);")
	return;
}


int main()
{
    SortedList *list = SLCreate(&compareStrings, &destroyBasicTypeNoAlloc);
    char *one = "hello";
    char *two = "goodbye";
    char *three = "yes";
    char *four = "no";
    char *five = "stop";
    char *six = "go";
    SLInsert(list, one);
    SLInsert(list, two);
    SLInsert(list, three);
    SLInsert(list, four);
    SLInsert(list, five);
    SLInsert(list, six);
    SLRemove(list, three);
    SLRemove(list, "asdf");
    SLRemove(list, one);
    SLDestroy(list);
}
