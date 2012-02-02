#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAXDEPTH 22
#define POPSIZE 500
#define STRINGSIZE 1000
#define TERMINATIONDUPLICATES 100
#define BASETYPE 0

#define ADDRLINES 7
#define TESTLINES 9
#define TOTALLINES 16
#define MIDUPPER 9
//#define TESTANSWER "IF(&(a0,a1),d3,IF(a0,d2,IF(a1,d1,d0)))"
#define TESTANSWER "IF(a0,IF(&(a1,a2),d7,IF(a1,d6,IF(a2,d5,d4))),IF(&(a1,a2),d3,IF(a1,d2,IF(a2,d1,d0))))"
#define PRUNE
#define PR 0.1
#define TESTSUBSET 300
#define DOTS

enum nodeType { XOR, IF, AND, OR, NOT, aTerm, dTerm, ERR };

struct node {
    enum nodeType type;
    int no;
    struct node *X;
    struct node *Y;
    struct node *Z;
    
    // used by Xover, cached here for speed
    int height;
    int label;
    
    // garbage collection
    int used;
};

char *displayTree(struct node *tree, char* display);
char *displayTreeMin(struct node *tree, char* display);
void printTree(struct node *tree);

struct node *parseTreeMin(char *str);
struct node *parseTree(char *str);

int labeltree(struct node *tree);
int relabeltree(struct node *tree);
int testNode(struct node *tree, int* test);
int compare(struct node *a, struct node *b);

struct node *growTree(void);

struct node *insertTree(struct node *tree, struct node *subtree, int *count);
struct node *getSubTree(struct node *tree, int label);
struct node *copyTree(struct node *tree);

struct node *pruneTree(struct node *tree, int height);

void garbageCollect(struct node **curTrees, int noTrees);
//mersenne twister functions
void init_genrand(unsigned long s);
unsigned long genrand_int32(void);
