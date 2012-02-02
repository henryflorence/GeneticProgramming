#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
    int i;
    for(i=0; i<10; i++) printf("%f\n", 1. * rand() / RAND_MAX);
}
