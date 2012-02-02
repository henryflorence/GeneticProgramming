#include "gp.h"

int generateTests(int** testCases) {
    int i,j;
    
    int* tests = *testCases = (int*)malloc(pow(2,TOTALLINES) * TOTALLINES * sizeof(testCases));
    
    for(i=0; i<pow(2,TOTALLINES); i++)
        for(j=0; j<TOTALLINES; j++) tests[i * TOTALLINES + j] = (i & (int) pow(2,(TOTALLINES - j - 1))) > 0;
    
    return 0;
}
int generateAnswers(int* tests, int** answers, struct node *ansTree) {
    int i;
    int* ans = *answers = (int*)malloc(pow(2,TOTALLINES) * sizeof(answers));
    
    for(i=0; i<pow(2,TOTALLINES); i++) ans[i] = testNode(ansTree,&tests[TOTALLINES*i]);
    
    return 1;
}
int generateAnswersAlgo(int* tests, int** answers) {
    int i,j;
    int* ans = *answers = (int*)malloc(pow(2,TOTALLINES) * sizeof(answers));
    
    for(i=0; i<pow(2,TOTALLINES); i++) {
        int addrline = 0;
        
        // calculate dataline no
        for(j=0; j<ADDRLINES; j++) addrline += tests[TOTALLINES*i+j] * pow(2,(ADDRLINES - j - 1));
        
        // transfer choosen dataline to answers 
        ans[i] = tests[TOTALLINES*i+ADDRLINES+addrline];
    }
    return 1;
}
int generateAnswersXmidY(int* tests, int** answers) {
    int i,j;
    int* ans = *answers = (int*)malloc(pow(2,TOTALLINES) * sizeof(answers));
    
    for(i=0; i<pow(2,TOTALLINES); i++) {
        //int sum1 = 0;
        int sum2 = 0;
        // calculate sum
        //for(j=0; j<TESTLINES; j++) sum1 += tests[TOTALLINES*i+j+ADDRLINES];
        for(j=0; j<TOTALLINES; j++) sum2 += tests[TOTALLINES*i+j];
        
        // check 7 <= sum <= 9 
        ans[i] = ADDRLINES <= sum2 && sum2 <= MIDUPPER;
        if(ans[i]) {
            for(j=0; j<TOTALLINES; j++) printf("%i", tests[TOTALLINES*i+j]);
            printf(" - %i:%i\n", sum2, ans[i]); 
        }
    }
    return 1;
}
double fitness(int* tests, int* answers, int* testIndices, struct node *tree) {
    int i, r=0;
    #ifdef TESTSUBSET
    int noOfTests = TESTSUBSET;
    #else
    int noOfTests = pow(2,TOTALLINES);
    #endif
    
    for(i=0; i<noOfTests; i++)
        r += (testNode(tree,&tests[testIndices[i]*TOTALLINES]) == answers[testIndices[i]]);
    
    return 1. * r / noOfTests;
}
double fullFitness(int* tests, int* answers, struct node *tree) {
    int i, r=0;
    int noOfTests = pow(2,TOTALLINES);
    
    for(i=0; i<noOfTests; i++)
        r += (testNode(tree,&tests[i*TOTALLINES]) == answers[i]);
    
    return 1. * r / noOfTests;
}
#ifdef TESTSUBSET
int pickTests(int* testIndices) {
    int maxTests = pow(2,TOTALLINES); // < TESTSUBSET ? pow(2,TOTALLINES) : TESTSUBSET;
    int i, j, k;
    
    for(i=0; i<TESTSUBSET; i++) {
        j = genrand_int32() % maxTests;
        for(k=0; k<i; k++) {
            if(testIndices[k] == j) i--;
            else testIndices[i] = j;
        }
    }
    return 1;
}
#else
int pickTests(int* testIndices) {
    int maxTests = pow(2,TOTALLINES);
    int i;
    
    for(i=0; i<maxTests; i++) testIndices[i] = i;
        
    return 1;
}
#endif
int Xover(struct node **tree1, struct node **tree2) {

    // random cross over points for sub trees
    int cp1 = genrand_int32() % (*tree1)->label;
    int cp2 = genrand_int32() % (*tree2)->label;
    
    
    // create copies of sub trees
    struct node *subtree1 = copyTree(getSubTree(*tree1, cp1));
    struct node *subtree2 = copyTree(getSubTree(*tree2, cp2));
    
    #ifdef PRUNE
    // create candidate children
    *tree1 = insertTree(*tree1, subtree2, &cp1);
    *tree2 = insertTree(*tree2, subtree1, &cp2); 
    
    #else
    // create copies of parents
    struct node *candidate1 = copyTree(*tree1);
    struct node *candidate2 = copyTree(*tree2);
    
    // create candidate children
    tree1 = insertTree(candidate1, subtree2, cp1);
    tree2 = insertTree(candidate2, subtree1, cp2); 
    
    if(candidate1->height <= MAXDEPTH) *tree1 = candidate1;   
    if(candidate2->height <= MAXDEPTH) *tree2 = candidate2; 
    #endif
    
    return 0;
}
int selection(double fitnesses[POPSIZE]) {
    int i1 = genrand_int32() % POPSIZE;
    int i2 = genrand_int32() % POPSIZE;
    
    if(fitnesses[i1] > fitnesses[i2]) return i1;
    return i2;
}
int main() {
    int i,j,k=0;
    int* tests;
    int* answers;
    int* testIndices;
    char *exampleAns = TESTANSWER;
    //char *sampleAns = "a0"; //"IF(&(a0,a1),d3,d0)))";
    double best = 0;
    double worst = 0;
    int bestCount = 0;
    //floating point annoyance
    double bestFloor = 1.0 - 0.5 / pow(2,TOTALLINES);
    
    struct node *exampleAnsTree = parseTree(exampleAns);
    //struct node *sampleAnsTree = parseTree(sampleAns);
    struct node *temp, *temp2;
    struct node *pop[POPSIZE];
    char *strs[POPSIZE];
    struct node *newpop[POPSIZE];
    double fs[POPSIZE];
    double newfs[POPSIZE];
    
    char outStr[STRINGSIZE]; 
    
    srand(time(NULL)); 
    init_genrand(time(NULL));   
    generateTests(&tests);
    //generateAnswers(tests,&answers,exampleAnsTree);
    //generateAnswersAlgo(tests,&answers);
    generateAnswersXmidY(tests,&answers);
    
    return 0;
    #ifdef TESTSUBSET
    testIndices = malloc(TESTSUBSET * sizeof(int));
    #else
    testIndices = malloc(pow(2,TOTALLINES) * sizeof(int));
    #endif
    
    pickTests(testIndices);
    
    labeltree(exampleAnsTree);
    printf("%.2f:%.2f - %i:%i - %s\n", fitness(tests,answers,testIndices,exampleAnsTree), fullFitness(tests,answers,exampleAnsTree), exampleAnsTree->height, exampleAnsTree->label, displayTree(exampleAnsTree,outStr));
    
    printf("Generating initial population ");
    for(i=0; i<POPSIZE;) {
        struct node *tree;
        char *str;
        
        printf("%i",i);
        fflush(stdout);
        
        tree = growTree();
        str = malloc(tree->label * 2);
        str = displayTreeMin(tree,str);
        
        printf("-%i",tree->height);
        fflush(stdout);
        
        int duplicate = 0;
        for(j=0; j<i && !duplicate; j++)
            duplicate = (strcmp(str,strs[j]) == 0);
        
        printf(" ");
        fflush(stdout);
        if(!duplicate) {
            strs[i] = str;
            pop[i++] = tree;
        } else free(str);
    }
    
    garbageCollect(pop,POPSIZE);
    
    printf("done\nchecking fitnesses.");
    for(i=0; i<POPSIZE; i++) {
        fs[i] = fitness(tests,answers,testIndices,pop[i]);
        printf("%i.",i);
        fflush(stdout);
    }
    
    printf("\niterating population");
    fflush(stdout);
    
    for(k=0;;k++) {
        static double prevBest = -1.0;
        double realBest = 0.0;
        best = 0.0;
        worst = 1.0;
        
        // create next population
        for(i=0; i<POPSIZE;) {
            #ifdef DOTS
            printf(".");
            fflush(stdout);
            #endif
            #ifdef PR
            int fsi;
            if(1. * genrand_int32() / RAND_MAX > PR) {
            #endif
                temp = copyTree(pop[selection(fs)]);
                temp2 = copyTree(pop[selection(fs)]);
                
                Xover(&temp, &temp2);
                
                newpop[i] = temp;
                newfs[i++] = -1.; 
                newpop[i] = temp2;
                newfs[i++] = -1.;
            #ifdef PR
            } else {
                fsi = selection(fs);
                
                newpop[i] = copyTree(pop[fsi]);
                newfs[i++] = fs[fsi];
            }
            #endif
        }
        printf(" ");
        fflush(stdout);
        // transfer population
        for(i=0; i<POPSIZE; i++) pop[i] = newpop[i];
        
        garbageCollect(pop, POPSIZE);
        
        // generate fitness
        pickTests(testIndices);
        for(i=0; i<POPSIZE; i++) {
            if(newfs[i] > -0.5)
                fs[i] = newfs[i];
            else
                fs[i] = fitness(tests,answers,testIndices,pop[i]);
                
            best = fs[i] > best ? fs[i] : best;
            worst = fs[i] < worst ? fs[i] : worst;
            if(fs[i] > bestFloor) {
                double testBest = fullFitness(tests,answers,pop[i]);
                realBest = testBest > realBest ? testBest : realBest;
            }
        }
        
        if(best > bestFloor) printf("r%.2f:%.2f",realBest,worst);
        else printf("%.2f:%.2f",best,worst);
        
        fflush(stdout);
        if(prevBest - best < 0.001 && prevBest - best > -0.001) bestCount++;
        else bestCount = 0;
        if(realBest > bestFloor) break;
        prevBest = best;     
    }
    
    printf("generations: %i\n",k);
    for(i=0; i<POPSIZE; i++)
        if(fs[i] > bestFloor) printf("\n%.2f:%i:%i - %s\n", fullFitness(tests,answers,pop[i]), pop[i]->height, pop[i]->label, displayTree(pop[i],outStr));
    
    garbageCollect(NULL, POPSIZE);
    
    free(testIndices);
    free(tests);
    free(answers);
    
    printf("all done\n");
    return 0;
}
