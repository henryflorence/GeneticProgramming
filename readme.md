Introduction
============

This is a solution to a GP (Genetic Programming) algorithm set as a course work on the Evolutionary Computation module at UCL. A GP aims to find the solution to a problem by evolving solutions from a pool of candidate of (hopefully) less able solutions, each candidate is tested against the problem to gain a fitness, two candidate syntax trees can then be 'crossed over' and introduced to the next generation, in this manner GP is an example of a GA (Genetic Algorithm).

Although fallen out of fashion as a useful tool for solving interesting problems, a GP is still an interesting algorithm to implement. I decided to implement it in C for speed and minimise memory usage, and also C being designed to implement compilers and this problems requirement for syntax trees. Due to the amount of computation required, optimising and tuning the code and parameters to run faster was both useful and a rewarding process.

I decided to implement a mark and sweep garbage collector on the nodes for the syntax tree, as the random nature of the cross over operation made bug free memory management tricky. The garbage collector can be fired manually, at the end of the creation of each generation, for example; or when the predifined pool size is exhausted. There is also an interesting dualality between storing the candidate solution in memory as trees, being faster to execute; or strings, being smaller in size.

Compilation
===========

Options are set in the gp.h file as preproccessor macros, for speed and because command line parameter parsing is dull, ;)

cc -o gpTest gp.c tree.c  mt/mt19937ar.c -lm
