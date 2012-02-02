#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct items {
    int a;
    int b;
};
void swap(int *iptr, int *jptr) {
    int t = *jptr;
    *jptr = *iptr;
    *iptr = t;
}
void swapStructs(struct items **i1, struct items **i2) {
    struct items *t = *i1;
    *i1 = *i2;
    *i2 = t;
}
int main() {
    int i = 1;
    int j = 2;
    struct items *items1 = malloc(sizeof(struct items));
    struct items *items2 = malloc(sizeof(struct items));
    
    printf("i:%i, j:%i\n",i,j);
    swap(&i,&j);
    printf("i:%i, j:%i\n",i,j);
    
    items1->a = 1;
    items1->b = 2;
    items2->a = 3;
    items2->b = 4;
    
    printf("1->a=%i, 1->b=%i, 2->a=%i, 2->b=%i\n",items1->a,items1->b,items2->a,items2->b);
    swapStructs(&items1,&items2);
    printf("1->a=%i, 1->b=%i, 2->a=%i, 2->b=%i\n",items1->a,items1->b,items2->a,items2->b);
}
