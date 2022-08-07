#include <stdlib.h>
#include <math.h>

#define M_PI 3.14159265358979323846

/* Function that returns a double between 0 and 1 */ 
double randd_uniform(void)
{
    double rnd = (double)rand()/(double)(RAND_MAX);
    return rnd;
}

/* Function that returns a normal distributed random number. Uses the Box-Muller transform */
double randd_std_norm(void)
{
    /* Generating 2 random numbers used to generate a random normal number.
    We check if u1 is 0 such that we can immediately return it before generating u2. */
    /* First random number */
    double u1 = randd_uniform();
    /* If u1 is 0, the natural logarithm of u1 is undefined. Therefore, we return 0. This is not exact but should not affect results by a lot. */
    if (u1 == 0)
    {
        return 0;
    }
    /* Second random number */
    double u2 = randd_uniform();

    /* Generate normal random number */
    double rnd = sqrt(-2 * log(u1)) * cos(2*M_PI*u2);
    /* Return random number */
    return rnd;    
}

/* Returns a random int between lb and ub. lb is included, ub is not. */ 
int randint(int lb, int ub)
{
    int rnd = rand() % (ub-lb);
    return lb + rnd;

}