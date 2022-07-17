#include <stdio.h> /* Standard IO */
#include <pthread.h> /* Multi Threading */
#include <stdlib.h> /* Included for random numbers */
#include <time.h> /* Included for timing our program */
#include <math.h> /* Included for getting PI and power functions etc. */

/* DEFINITION OF SYSTEM SPECS */
#define NUM_THREADS = 2 /* We use 2 threads for now. This will be changed to a higher number later */

/* DEFINITION OF PROGRAM PARAMETERS */
#define RANDOM_SEED 1843397

/* Forward Declarations */
/* Structs */

/* Functions */
double randf_uniform(void);
double randf_std_norm(void);

int main(void)
{
    /* Starting timer */
    clock_t start_time = clock();

    /* Setting random number seed */
    srand(RANDOM_SEED);

    /* Generating random floats */ 
    for(int i = 0; i < 1000; i++)
    {
        printf("Random number %d is: %lf\n", i, randf_std_norm());
    }

    /* End timer and print elapsed time */
    clock_t end_time = clock();
    double time_taken = ((double)(end_time-start_time)/CLOCKS_PER_SEC);
    printf("Time taken was %lf seconds.\n", time_taken);
    return 0;
}

/* Function that returns a double between 0 and 1 */ 
double randf_uniform(void)
{
    double rnd = (double)rand()/(double)(RAND_MAX);
    return rnd;
}

/* Function that returns a gaussian distributed random number. Uses the Box-Muller transform */
double randf_std_norm(void)
{
    /* Generating 2 random numbers used to generate a random normal number.
    We check if u1 is 0 such that we can immediately return it before generating u2. */
    /* First random number */
    double u1 = randf_uniform();
    /* If u1 is 0, the natural logarithm of u1 is undefined. Therefore, we return 0. This is not exact but should not affect results by a lot. */
    if (u1 == 0)
    {
        return 0;
    }
    /* Second random number */
    double u2 = randf_uniform();

    /* Generate normal random number */
    double rnd = sqrt(-2 * log(u1)) * cos(2*M_PI*u2);
    /* Return random number */
    return rnd;    
}
