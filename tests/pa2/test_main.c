/*
 * sorted-list.c
 */

#include	<string.h>
#include    <stdio.h>
#include    <stdlib.h>
#include	"sorted-list.h"
#define MAX(a,b) (((a)>(b))?(a):(b))

enum TYPE{
    STRING,
    INT,
    DOUBLE,
    STRUCT
};


struct TestStructT_
{
    int field;
};

typedef struct TestStructT_ TestStruct;



//Comparison functions
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

int compareTestStructs(void*p1,void*p2){
    TestStruct*s1 = p1;
    TestStruct*s2 = p2;
    return s1->field - s2->field;
}

//Destructors
void destroyBasicTypeAlloc(void*p){
    free(p);
}

void destroyBasicTypeNoAlloc(void*p){
    return;
}

void destroyTestStruct(void*p){
    //For now, just a basic free.
    //But if the struct had allocated memory, we'd need to do more.
    free(p);
}



//A reason to use opaque typing
//Did not use.  For illustrative purposes.
struct Tree_
{
    int num_children;
    int value; //weight, some unique id, etc...
    struct Tree_ *children[];
};
typedef struct Tree_ Tree;

// some comparison function for these trees
int compareTrees(void*p1,void*p2){
    Tree*t1 = p1;
    Tree*t2 = p2;
    int max_n_children;
    int i;
    int ret_val;
    
    if(t1->value > t2->value){
        return 1;
    }else if(t2->value > t1->value){
        return -1;
    }
    
    max_n_children = MAX(t1->num_children,t2->num_children);
    
    for(i = 0; i < max_n_children;i++){
        ret_val = compareTrees(t1->children[i],t2->children[i]);
        if(ret_val != 0)
            return ret_val;
    }
    
    return (t1->num_children < t2->num_children) ? -1 : ((t1->num_children > t2->num_children) ? 1 : 0);
}

void destroyTree(Tree* t){
    int i;
    
    for(i = 0; i < t->num_children;i++){
        destroyTree(t->children[i]);
    }
    
    free(t);
}




/*
 *
 *  Helper functions
 *
 */
int populate_list(SortedListPtr s, int type, int len, int iargs[], double dargs[],char*sargs[], TestStruct strtargs[]){
    
    
    int i;
    if(type == INT){
        for(i = 0; i < len; i++){
            int *toinsert = (int*)malloc(sizeof(int));
            *toinsert = iargs[i];
            if(SLInsert(s,toinsert) == 0){
                return 0;
            }
        }
    }else if (type == DOUBLE){
        for(i = 0; i < len; i++){
            double *toinsert = (double*)malloc(sizeof(double));
            *toinsert = dargs[i];
            if(SLInsert(s,toinsert) == 0){
                return 0;
            }
        }
    }else if(type==STRING){
        for(i = 0; i < len; i++){
            char *toinsert = (char*)malloc(strlen(sargs[i])+1);
            toinsert[strlen(sargs[i])] = 0;
            strcpy(toinsert,sargs[i]);
            
            if(SLInsert(s,toinsert) == 0){
                return 0;
            }
        }
    }else if (type == STRUCT){
        for(i = 0; i < len; i++){
            TestStruct *toinsert = (TestStruct*)malloc(sizeof(TestStruct));
            toinsert->field = (strtargs[i]).field;
            if(SLInsert(s,toinsert) == 0){
                return 0;
            }
        }
    }
    
    return 1;
    
}




void iterprint_all(SortedListPtr s, SortedListIteratorPtr iter, int type){
    void *item = SLGetItem(iter);
    while(1){
        //item = SLGetItem(iter);//modify by lezi
        if (item == NULL){
            break;
        }else{
            if(type == INT){
                printf("%d ", *((int*)item));
            }else if(type == DOUBLE){
                printf("%f ", *((double*)item));
            }
            else if(type==STRING){
                printf("%s ", ((char*)item));
            }else if(type == STRUCT){
                printf("struct={%d} ", ((TestStruct*)item)->field);
            }
        }
	item = SLNextItem(iter);//modify by lezi
    }
    printf("\n");
}

void iterprint_all_int(SortedListPtr s, SortedListIteratorPtr iter){
    iterprint_all(s,iter,INT);
}

void success(){
    char*passmessage = "Success";
    printf("%s\n",passmessage);
}

void failure(){
    char*failmessage = "Failure";
    printf("%s\n",failmessage);
}

/*
 *
 *
 *  Main
 *
 *
 *
 */

int main(int argc, char*argv[])
{
    int choice = atoi(argv[1]);

    
    

    
    printf("Choice: %d\n", choice);
    
    if(choice == 1){
        //1.  Normal SLInserts (@ beginning, middle, end of list)
        //a. integers
        SortedListPtr s = SLCreate(compareInts,destroyBasicTypeAlloc);
        int iarr[6] = {7,5,155,42,-1,6};
        if(populate_list(s,INT,sizeof(iarr)/sizeof(int),iarr,NULL,NULL,NULL) == 0){
            failure();
            return 1;
        }
        SortedListIteratorPtr iter = SLCreateIterator(s);
        iterprint_all(s,iter,INT); //prints 155 42 7 6 5 -1
        
        
    }else if(choice == 2){
        //b. doubles
        SortedListPtr s = SLCreate(compareDoubles,destroyBasicTypeAlloc);
        
        double darr[6] = {7.43,5.55,155.26,42.1,-1.5,6.7};
        if(populate_list(s,DOUBLE,sizeof(darr)/sizeof(double),NULL,darr,NULL,NULL) == 0){
            failure();
            return 1;
        }

        
        SortedListIteratorPtr iter = SLCreateIterator(s);
        iterprint_all(s,iter,DOUBLE); //prints 155.26 42.1 7.43 6.7 5.55 -1.5
        
        
    }else if(choice == 3){
        //c. strings
        SortedListPtr s = SLCreate(compareStrings,destroyBasicTypeAlloc);
        
        char *carr[6] = {"cat","dog","catamaran","apple","cormorant","zebra"};
        if(populate_list(s,STRING,6,0,0,carr,NULL) == 0){
            failure();
            return 1;
        }

        SortedListIteratorPtr iter = SLCreateIterator(s);
        iterprint_all(s,iter,STRING); //prints apple cat catamaran cormorant dog zebra
				      // should print zebra dog cormorant catamaran cat apple
        
        
        
    }else if(choice == 4){
        
        //d. structs
        SortedListPtr s = SLCreate(compareTestStructs,destroyBasicTypeAlloc);
        
        TestStruct strtarr[6];
        strtarr[0].field = 7;
        strtarr[1].field = 5;
        strtarr[2].field = 155;
        strtarr[3].field = 42;
        strtarr[4].field = -1;
        strtarr[5].field = 6;
        
        if(populate_list(s,STRUCT,6,0,0,0,strtarr) == 0){
            failure();
            return 1;
        }
        
        SortedListIteratorPtr iter = SLCreateIterator(s);
        iterprint_all(s,iter,STRUCT); //prints 155 42 7 6 5 -1
    
    }else if(choice == 5){
        //SLRemove nonexistent element
        SortedListPtr s = SLCreate(compareInts,destroyBasicTypeAlloc);
        int iarr[6] = {7,5,155,42,-1,6};
        populate_list(s,INT,sizeof(iarr)/sizeof(int),iarr,0,0,NULL);

        int toremove = 54;
        if (SLRemove(s,&toremove) != 0) {
            failure();
        }else{
            success();
        }
    }else if(choice == 6){
        //SLRemove existing element (no duplicates; didn't handle them anyway)
        SortedListPtr s = SLCreate(compareInts,destroyBasicTypeAlloc);
        int iarr[6] = {7,5,155,42,-1,6};
        populate_list(s,INT,sizeof(iarr)/sizeof(int),iarr,0,0,NULL);


        int toremove = 5;
        
        if (SLRemove(s,&toremove) != 0) {
            SortedListIteratorPtr iter = SLCreateIterator(s);
            iterprint_all_int(s,iter); //prints 155 42 7 6 -1
        }else{
            failure();
        }
        
    }else if(choice == 7){
        //Iterate on empty list
        //Should not fail
        SortedListPtr s = SLCreate(compareInts,destroyBasicTypeAlloc);
        SortedListIteratorPtr iter = SLCreateIterator(s);
        iterprint_all_int(s,iter);
    }else if (choice == 8){
        //SLInsert for item beyond iterator
        SortedListPtr s = SLCreate(compareInts,destroyBasicTypeAlloc);
        int iarr[6] = {7,5,155,42,-1,6};
        populate_list(s,INT,sizeof(iarr)/sizeof(int),iarr,0,0,NULL);
        
        SortedListIteratorPtr iter = SLCreateIterator(s);
        void *item = SLNextItem(iter);
        item = SLNextItem(iter);
        
        int *toins = malloc(sizeof(int));
        *toins = 4;
        SLInsert(s,toins);
        iterprint_all_int(s,iter); //prints 7 6 5 4 -1
        
    }else if(choice == 9){
        //Create multiple iterators on same list, interleave iterations.
        //Delete item between iterators.
        
        SortedListPtr s = SLCreate(compareInts,destroyBasicTypeAlloc);
        int iarr[6] = {7,5,155,42,-1,6};
        populate_list(s,INT,sizeof(iarr)/sizeof(int),iarr,0,0,NULL);
        
        SortedListIteratorPtr olditer = SLCreateIterator(s);
        
        SortedListIteratorPtr iter = SLCreateIterator(s);

        
        void *item;
        
        int i;
        for(i = 0; i < 2; i++){
            item = SLGetItem(olditer);//modify by lezi
            printf("%d ", *((int*)item));
	    item = SLNextItem(olditer);//modify by lezi
        } //prints 155 42
        printf("\n");

        for(i = 0; i < 4; i++){
            item = SLGetItem(iter); //modify by lezi
            printf("%d ", *((int*)item));
            item = SLNextItem(iter);//modify by lezi
        } //prints 155 42 7 6
        printf("\n");  
               
        int itoremove = 6;
        if (SLRemove(s,&itoremove) == 0) {
            failure();
            return 1;
        }
        
        item = SLGetItem(iter);  //prints 5  //modufy by lezi
        printf("%d\n", *((int*)item));
        item = SLGetItem(olditer);
        printf("%d\n", *((int*)item)); //prints 7  //modify by lezi
         
        
        iterprint_all_int(s,iter); //prints 5 -1
        
        iterprint_all_int(s,olditer); //prints 7 5 -1
        
    }else if(choice == 10){
        //SLRemove end element, iterator positioned on it
        
        SortedListPtr s = SLCreate(compareInts,destroyBasicTypeAlloc);
        int iarr[3] = {7,5,3};
        populate_list(s,INT,sizeof(iarr)/sizeof(int),iarr,0,0,NULL);

        
        SortedListIteratorPtr iter = SLCreateIterator(s);

        int x3 = 3;
        void *item = SLNextItem(iter);
        item = SLNextItem(iter);//iterator points to 3
        if(SLRemove(s,&x3) && SLNextItem(iter) == NULL){
            success();
        }else{
            failure();
        }
        
    }else if(choice == 11){ //TODO
        //SLRemove element in middle, iterator positioned on it
        SortedListPtr s = SLCreate(compareInts,destroyBasicTypeAlloc);
        int iarr[3] = {7,5,3};
        populate_list(s,INT,sizeof(iarr)/sizeof(int),iarr,0,0,NULL);
        
        SortedListIteratorPtr iter = SLCreateIterator(s);
        void *item = SLNextItem(iter);
        
        int x1 = 5;
        if(SLRemove(s,&x1) && *((int*)SLNextItem(iter)) == 3 &&
           ((int*)SLNextItem(iter)) == NULL){
            success();
        }else{
            failure();
        }
        
        
        
        
    }else if(choice == 12){
        //SLRemove element positioned immediately after element iterator is pointing to
        SortedListPtr s = SLCreate(compareInts,destroyBasicTypeAlloc);
        int iarr[6] = {7,5,155,42,-1,6};
        populate_list(s,INT,sizeof(iarr)/sizeof(int),iarr,0,0,NULL);
        

        SortedListIteratorPtr iter = SLCreateIterator(s);
        void *item = SLNextItem(iter);
        item = SLNextItem(iter);
        int x1 = 6;
        if(SLRemove(s,&x1) && *((int*)SLGetItem(iter)) == 7 //modify by lezi
           && *((int*)SLNextItem(iter)) == 5
           && *((int*)SLNextItem(iter)) == -1
           && ((int*)SLNextItem(iter)) == NULL){
            success();
        }else{
            failure();
        }
        
    }else if(choice == 13){
        //Add item between recently returned element and element about to be returned.
        SortedListPtr s = SLCreate(compareInts,destroyBasicTypeAlloc);
        int iarr[3] = {7,5,3};
        populate_list(s,INT,sizeof(iarr)/sizeof(int),iarr,0,0,NULL);
        
        SortedListIteratorPtr iter = SLCreateIterator(s);
        void *item = SLNextItem(iter);
        item = SLNextItem(iter);//point to 3 now
        int *x4 = malloc(sizeof(int));
        *x4 = 4;
        SLInsert(s,x4);
        
        iterprint_all_int(s,iter); //prints 3
        

        
        
    }else if(choice == 14){
        //Add item to head and middle, after iterator has been created //modify by Lezi
        SortedListPtr s = SLCreate(compareInts,destroyBasicTypeAlloc);
        int iarr[3] = {7,5,3};
        populate_list(s,INT,sizeof(iarr)/sizeof(int),iarr,0,0,NULL);
        
        SortedListIteratorPtr iter = SLCreateIterator(s);
        int *toinsert = malloc(sizeof(int));
        *toinsert = 8;
        SLInsert(s,toinsert);
        int *toinsert1 = malloc(sizeof(int));
  	*toinsert1 = 4; // add by lezi
	SLInsert(s,toinsert1);//add by lezi
        
        iterprint_all_int(s,iter); //prints 7 5 4 3 //modify by lezi
    }else if(choice == 15){
        
        //Add item to tail after all items have been returned by iterator.
        SortedListPtr s = SLCreate(compareInts,destroyBasicTypeAlloc);
        int iarr[3] = {7,5,3};
        populate_list(s,INT,sizeof(iarr)/sizeof(int),iarr,0,0,NULL);
        
        SortedListIteratorPtr iter = SLCreateIterator(s);
        void *item;
        while((item = SLNextItem(iter)) != NULL);
        
        
        int *toins = malloc(sizeof(int));
        *toins = -1;
        SLInsert(s,toins);
        
        if(SLGetItem(iter) == NULL){ //modify by lezi
            success();
        }else{
            failure();
        }
    }else{
        printf("Bad input\n");
    }
    
    
    return 0;
    
}
