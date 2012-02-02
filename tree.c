#include "gp.h"
#define NODESIZEINC 1000

// garbage collection info
struct node **allNodes = NULL;
int noNodes = 0;
int nodesAllocated = 0;

struct node *newNode() {
    
    //is there a spare node?
    if(noNodes + 1 > nodesAllocated) {
        int i,j=0;
        struct node **allNodes2 = malloc((nodesAllocated + NODESIZEINC) * sizeof(struct node));
        
        // we need a bigger boat
        for(i=0; i<nodesAllocated; i++) allNodes2[i] = allNodes[i];
        nodesAllocated = nodesAllocated + NODESIZEINC;
        
        // compact away null nodes
        for(i=0; i<noNodes; i++)
            if(allNodes[i] != NULL) allNodes2[j++] = allNodes[i];
        
        //transfer to bigger boat
        if(allNodes != NULL) free(allNodes);
        allNodes = allNodes2;
        noNodes = j;
    }
    struct node *n = malloc(sizeof(struct node));
    if(n == NULL) {
        garbageCollect(NULL,-1);
        n = malloc(sizeof(struct node));
    }
    n->used = 1;
    allNodes[noNodes] = n;
    
    noNodes++;
    return n;
}
void markNode(struct node *n) {
    n->used = 1;
    switch (n->type) {
        case IF:
            markNode(n->Z);
        case AND:
        case XOR:
        case OR:
            markNode(n->Y);
        case NOT:   
            markNode(n->X);
            break;
       
        case aTerm:
        case dTerm:
        case ERR:
            break;
    }
    return;
}
void garbageCollect(struct node **curTrees, int noTrees) {
    int i,deleted = 0,nodesLeft=0;
    static struct node **trees = NULL;
    
    if(curTrees != NULL) trees = curTrees;
    else if(noTrees == -1 && trees == NULL) return;
    else if(noTrees == -1) curTrees = trees;
    
    #ifdef GARBAGEINFO
    int nodesNull=0;
    printf("\nchecking %i nodes..",noNodes);
    #endif
    
    for(i=0; i<noNodes; i++) 
        if(allNodes[i] != NULL) allNodes[i]->used = 0;
    
    if(curTrees != NULL) 
        for(i=0; i<noTrees; i++) markNode(curTrees[i]);
    
    for(i=0; i<noNodes; i++) {
        if(allNodes[i] != NULL && !allNodes[i]->used) {
            free(allNodes[i]);
            allNodes[i] = NULL;
            deleted++;
        } 
        #ifdef GARBAGEINFO
        else if(allNodes[i] == NULL) nodesNull++;
        #endif
    }
    
    #ifdef GARBAGEINFO
    printf("deleted %i nodes..",deleted);
    
    for(i=0; i<noNodes; i++)
        if(allNodes[i] != NULL) nodesLeft++;
        
    printf("%i nodes left in list, %i are null\n",nodesLeft,nodesNull);
    #endif
    
    if(curTrees == NULL) {
        for(i=0; i<noNodes; i++)
            if(allNodes[i] != NULL) nodesLeft++;
            
        if(nodesLeft == 0) {
            noNodes = nodesAllocated = 0;
            free(allNodes);
            allNodes = NULL;
        }
    }
}

char* displayNode(struct node *tree, char* display) {
    char digit[2] = {'0','\0'};
    switch (tree->type) {
        case AND:
            strcat (display, "&(");
            displayNode (tree->X, display);
            strcat (display, ", ");
            displayNode (tree->Y, display);
            strcat (display, ")");
            break;
        case XOR:
            strcat (display, "X(");
            displayNode (tree->X, display);
            strcat (display, ", ");
            displayNode (tree->Y, display);
            strcat (display, ")");
            break;
        case OR:
            strcat (display, "|(");
            displayNode (tree->X, display);
            strcat (display, ", ");
            displayNode (tree->Y, display);
            strcat (display, ")");
            break;
        case NOT:
            strcat (display, "!");
            displayNode (tree->X, display);
            break;
        case IF:
            strcat (display, "if(");
            displayNode (tree->X, display);
            strcat (display, ", ");
            displayNode (tree->Y, display);
            strcat (display, ", ");
            displayNode (tree->Z, display);
            strcat (display, ")");
            break;
        case aTerm:
            strcat (display, "a");
            digit[0] = (char)(48+tree->no);
            strcat (display, (char*)&digit);
            break;
        case dTerm:
            strcat (display, "d");
            digit[0] = (char)(48+tree->no);
            strcat (display, (char*)&digit);
            break;
        case ERR:
            strcat (display, "E");
            break;
    }
    return display;
}
char *displayTree(struct node *tree, char* display) {
    *display = '\0';
    return displayNode(tree, display);
}
char* displayNodeMin(struct node *tree, char* display) {
    char digit[2] = {'0','\0'};
    switch (tree->type) {
        case IF:
            strcat (display, "?");
            break;
        case AND:
            strcat (display, "&");
            break;
        case XOR:
            strcat (display, "x");
            break;
        case OR:
            strcat (display, "|");
            break;
        case NOT:
            strcat (display, "!");
            break;
        case aTerm:
            strcat (display, "a");
            break;
        case dTerm:
            strcat (display, "d");
            break;
        case ERR:
            strcat (display, "E");
            break;
    }
                     
    switch (tree->type) {
        case IF:
            displayNodeMin (tree->Z, display);
        case OR:
        case XOR:
        case AND:
            displayNodeMin (tree->Y, display);
        case NOT:
            displayNodeMin (tree->X, display);
            break;
        case aTerm:
        case dTerm:
            digit[0] = (char)(48+tree->no);
            strcat (display, (char*)&digit);
            break;
        case ERR:
            break;
    }
    return display;
}
char *displayTreeMin(struct node *tree, char* display) {
    *display = '\0';
    return displayNodeMin(tree, display);
}
struct node *parseNodeMin(char *str, int *n) {
    struct node *nn = newNode();
    nn->no = nn->height = nn->label = 0;
    
    switch (str[(*n)++]) {
        case '&':
            nn->type = AND;
            nn->X = parseNodeMin(str, n);     
            nn->Y = parseNodeMin(str, n);
            break;
        case '|':
            nn->type = OR;
            nn->X = parseNodeMin(str, n);
            nn->Y = parseNodeMin(str, n);
            break;
        case '!':
            nn->type = NOT;
            nn->X = parseNodeMin(str, n);
            break;
        case 'I':
            nn->type = IF;
            nn->X = parseNodeMin(str, n);
            nn->Y = parseNodeMin(str, n);
            nn->Z = parseNodeMin(str, n);
            break;
        case 'd':
            nn->type = dTerm;       
            nn->no = (int)str[(*n)++]-48;
            break;
        case 'a':
            nn->type = aTerm;
            nn->no = (int)str[(*n)++]-48;
            break;
        default:
            nn->type = ERR;
    }
    return nn;
}
struct node *parseTreeMin(char *str) {
    int n = 0;
    return parseNodeMin(str, &n);
}
struct node *parseNode(char *str, int *n) {
    struct node *nn = newNode();
    nn->no = nn->height = nn->label = 0;
    
    switch (str[(*n)++]) {
        case '&':
            (*n)++;  //(
            nn->type = AND;
            nn->X = parseNode(str, n);
            (*n)++;  //,
            nn->Y = parseNode(str, n);
            (*n)++;  //)
            break;
        case '|':
            (*n)++;  //(
            nn->type = OR;
            nn->X = parseNode(str, n);
            (*n)++;  //,
            nn->Y = parseNode(str, n);
            (*n)++;  //)
            break;
        case '!':
            nn->type = NOT;
            nn->X = parseNode(str, n);
            break;
        case 'I':
            (*n)+=2;  //F(
            nn->type = IF;
            nn->X = parseNode(str, n);
            (*n)++;  //,
            nn->Y = parseNode(str, n);
            (*n)++;  //,
            nn->Z = parseNode(str, n);
            (*n)++;  //)
            break;
        case 'd':
            nn->type = dTerm;       
            nn->no = (int)str[(*n)++]-48;
            break;
        case 'a':
            nn->type = aTerm;
            nn->no = (int)str[(*n)++]-48;
            break;
        default:
            nn->type = ERR;
    }
    return nn;
}
struct node *parseTree(char *str) {
    int n = 0;
    return parseNode(str, &n);
}
int testNode(struct node *tree, int* test) {
    int a,b;
    switch (tree->type) {
        case AND:
            return testNode(tree->X, test) && testNode(tree->Y, test);
        case XOR:
            a = testNode(tree->X, test);
            b = testNode(tree->Y, test);
            return (a || !b) && (!a || b);
        case OR:
            return testNode(tree->X, test) || testNode(tree->Y, test);
        case NOT:
            return !testNode(tree->X, test);
        case IF:
            if(testNode(tree->X, test)) return testNode(tree->Y, test);
            return testNode(tree->Z, test);
        case aTerm:
            return test[tree->no];
        case dTerm:
            return test[tree->no+ADDRLINES];
         default:
            return 0;
    }
    return 0;
}
struct node *randomNode(int n, int* label) {
    struct node *rtnNode = newNode();
    rtnNode->X = rtnNode->Y = rtnNode->Z = NULL;
    
    rtnNode->type = n > 1 ? rand() % (7 - BASETYPE) + BASETYPE : rand() % 2 + 5;
    rtnNode->label = (*label)++;
    
    return rtnNode;
}
void growNode(struct node *curNode, int n, int* label) {   
    n--;
    
    //inititalise value for max height labelling
    curNode->height = 1;
    
    switch (curNode->type) {
        case IF:
            curNode->Z = randomNode(n, label);
            growNode(curNode->Z, n, label);
            
            //post order traversal to find max height of subtrees
            curNode->height = curNode->Z->height + 1;
        case AND:
        case XOR:
        case OR:
            curNode->Y = randomNode(n, label);
            growNode(curNode->Y, n, label);
            
            //post order traversal to find max height of subtrees
            if(curNode->Y->height + 1 > curNode->height)
                curNode->height = curNode->Y->height + 1;
        case NOT:   
            curNode->X = randomNode(n, label);
            growNode(curNode->X, n, label);
            
            //post order traversal to find max height of subtrees
            if(curNode->X->height + 1 > curNode->height)
                curNode->height = curNode->X->height + 1;
            break;
       
        case aTerm:
            curNode->no = rand() % ADDRLINES;
            break;
        case dTerm:
            curNode->no = rand() % TESTLINES;
        case ERR:
            break;
    }
    return;
}

struct node *growTree(void) {
    struct node *rootNode = newNode();
    int label = 1;
    
    rootNode->type = rand() % (7 - BASETYPE) + BASETYPE; //no terminals here please
    growNode(rootNode, MAXDEPTH, &label);
    
    //this node's label == 0, so store max label here
    rootNode->label = label;
    return rootNode;
}
int compare(struct node *a, struct node *b) {
    if(a->type != b->type) return 0;
    switch (a->type) {
        case IF:
            if(!compare(a->Z,b->Z)) return 0;
        case AND:
        case XOR:
        case OR:
            if(!compare(a->Y,b->Y)) return 0;
        case NOT:   
            if(!compare(a->X,b->X)) return 0;
            break;
       
        case aTerm:
        case dTerm:
            if(a->no != b->no) return 0;
            break;
        case ERR:
            return 0;
            break;
    }
    return 1;
}
struct node *getNode(struct node *tree, int label) {
    struct node *test = NULL;
    
    //termination condition => this is the subtree
    if(tree->label == label) return tree;
    
    switch (tree->type) {
        case IF:
            if((test = getNode(tree->Z, label)) != NULL) return test;
        case AND:
        case XOR:
        case OR:
            if((test = getNode(tree->Y, label)) != NULL) return test;
        case NOT:
            if((test = getNode(tree->X, label)) != NULL) return test;
            break;
       
        case aTerm:
        case dTerm:
        case ERR:
            return NULL;
    }
    return NULL;
}
struct node *getSubTree(struct node *tree, int label) {
    if(label == 0) return tree;
    return getNode(tree,label);
}
void printTree(struct node *tree) {
    char str[STRINGSIZE];
    printf("%i:%i - %s\n",tree->height, tree->label, displayTree(tree,str));
    
    switch (tree->type) {
        case IF:
            printTree(tree->Z);
        case AND:
        case XOR:
        case OR:
            printTree(tree->Y);
        case NOT:
            printTree(tree->X);
            break;
       
        case aTerm:
        case dTerm:
        case ERR:
            break;
    }
}
struct node *copyTree(struct node *tree) {
    struct node *n = newNode();
    n->height = tree->height;
    n->label = tree->label;
    n->type = tree->type;
    
    switch (tree->type) {
        case IF:
            n->Z = copyTree(tree->Z);
        case AND:
        case XOR:
        case OR:
            n->Y = copyTree(tree->Y);
        case NOT:
            n->X = copyTree(tree->X);
            break;
       
        case aTerm:
        case dTerm:
            n->no = tree->no;
            n->type = tree->type;
            break;
        case ERR:
            printf("error: node present in this tree\n");
            return NULL;
    }
    return n;
}

struct node *pruneNode(struct node *tree, int height, int* count) {
    if(height == 1) {
        int n = -1;
        struct node *terminal = randomNode(0, &n);
        
        terminal->no = rand() % (terminal->type == aTerm ? ADDRLINES : TESTLINES);
        terminal->label = (*count)++;
        terminal->height = 1;
        return terminal;
    }
    
    tree->label = (*count)++;
    tree->height = 1;
    switch (tree->type) {
        case IF:
            tree->Z = pruneNode(tree->Z, height-1, count);
            
            if(tree->Z->height + 1 > tree->height) tree->height = tree->Z->height + 1;
        case AND:
        case XOR:
        case OR:
            tree->Y = pruneNode(tree->Y, height-1, count);
            
            if(tree->Y->height + 1 > tree->height) tree->height = tree->Y->height + 1;
        case NOT:
            tree->X = pruneNode(tree->X, height-1, count);
            
            if(tree->X->height + 1 > tree->height) tree->height = tree->X->height + 1;
        case aTerm:
        case dTerm:
            break;
        case ERR:
            printf("error: node present in this tree\n");
    }
    return tree;
}

struct node *pruneTree(struct node *tree, int height) {
    int label = 0;
    tree = pruneNode(tree,height,&label);
    tree->label = label;
    return tree;
}
void labelnode(struct node *tree, int* count) {
    tree->label = (*count)++;
    
    tree->height = 1;
    switch (tree->type) {
        case IF:
            labelnode(tree->Z,count);
            
            if(tree->Z->height + 1 > tree->height) tree->height = tree->Z->height + 1;
        case AND:
        case XOR:
        case OR:
            labelnode(tree->Y,count);
            
            if(tree->Y->height + 1 > tree->height) tree->height = tree->Y->height + 1;
        case NOT:
            labelnode(tree->X,count);
            
            if(tree->X->height + 1 > tree->height) tree->height = tree->X->height + 1;
        case aTerm:
        case dTerm:
            break;
        case ERR:
            printf("error: node present in this tree\n");
    }
}
int labeltree(struct node *tree) {
    int n=0;
    labelnode(tree, &n);
    tree->label = n;
    return 1;
}
void relabelnode(struct node *tree, int* count) {
    int oldHeight = tree->height;
    if(tree->label != ++(*count)) printf("l%i:%i\n",tree->label,*count);
    tree->label = (*count);
    
    tree->height = 1;
    switch (tree->type) {
        case IF:
            relabelnode(tree->Z,count);
            
            if(tree->Z->height + 1 > tree->height) tree->height = tree->Z->height + 1;
        case AND:
        case XOR:
        case OR:
            relabelnode(tree->Y,count);
            
            if(tree->Y->height + 1 > tree->height) tree->height = tree->Y->height + 1;
        case NOT:
            relabelnode(tree->X,count);
            
            if(tree->X->height + 1 > tree->height) tree->height = tree->X->height + 1;
        case aTerm:
        case dTerm:
            break;
        case ERR:
            printf("error: node present in this tree\n");
    }
    if(oldHeight != tree->height) printf("h%i:%i\n",tree->height,oldHeight);
}
int relabeltree(struct node *tree) {
    int n=0;
    int oldHeight = tree->height;
    int oldLabel = tree->label;
    
    labelnode(tree, &n);
    
    if(oldLabel != n) printf("rl%i:%i\n",oldLabel,n);
    if(oldHeight != tree->height) printf("rh%i:%i\n",tree->height,oldHeight);
    tree->label = n;
    return 1;
}
struct node *insertNode(struct node *tree, struct node *subtree, int* count, int* newcount, int maxHeight) {
    if((*count)-- == 0)
        return pruneNode(subtree, maxHeight, newcount);

    tree->label = (*newcount)++;
    tree->height = 1;
    switch (tree->type) {
        case IF:
            tree->Z = insertNode(tree->Z,subtree,count,newcount,maxHeight-1);
            
            if(tree->Z->height + 1 > tree->height) tree->height = tree->Z->height + 1;
        case AND:
        case XOR:
        case OR:
            tree->Y = insertNode(tree->Y,subtree,count,newcount,maxHeight-1);
            
            if(tree->Y->height + 1 > tree->height) tree->height = tree->Y->height + 1;
        case NOT:
            tree->X = insertNode(tree->X,subtree,count,newcount,maxHeight-1);
            
            if(tree->X->height + 1 > tree->height) tree->height = tree->X->height + 1;
        case aTerm:
        case dTerm:
            break;
        case ERR:
            printf("error: ERR node present in this tree\n");
            return NULL;
    }
    return tree;
}
struct node *insertTree(struct node *tree, struct node *subtree, int *count) { 
    if((*count) == 0) {
        labeltree(subtree);
        return subtree;
    }
    
    if((*count) >= tree->label)
        printf("error: attempt to look for %i label in tree of %i labels, %i height\n",*count,tree->label,tree->height);
    else {
        int newcount = 0;
        tree = insertNode(tree,subtree,count,&newcount,MAXDEPTH);
        tree->label = newcount;
    }
    return tree;
}
