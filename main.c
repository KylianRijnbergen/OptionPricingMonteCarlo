#include <stdio.h> /* Standard IO */
#include <pthread.h> /* Multi Threading */
#include <stdlib.h> /* Included for random numbers */
#include <time.h> /* Included for timing our program */

/* DEFINITION OF SYSTEM SPECS */
#define NUM_THREADS = 2 /* We use 2 threads for now. This will be changed to a higher number later */

/* DEFINITION OF PROGRAM PARAMETERS */
#define RANDOM_SEED 1843397

/* Forward Declarations */
/* Structs */

/* Functions */
double randf(void);


int main(void)
{
    /* Starting timer */
    clock_t start_time = clock();

    /* Setting random number seed */
    srand(RANDOM_SEED);

    /* Generating random floats */ 
    for(int i = 0; i < 100000; i++)
    {
        printf("Random number %d is: %lf\n", i, randf());
    }

    /* End timer and print elapsed time */
    clock_t end_time = clock();
    double time_taken = ((double)(end_time-start_time)/CLOCKS_PER_SEC);
    printf("Time taken was %lf seconds.\n", time_taken);
    return 0;
}

/* Function that returns a double between 0 and 1 */ 
double randf(void)
{
    double rnd = (double)rand()/(double)(RAND_MAX);
    return rnd;
}