#include <stdio.h> /* Standard IO */
#include <pthread.h> /* Multi Threading */
#include <stdlib.h> /* Included for random numbers */
#include <time.h> /* Included for timing our program */
#include <math.h> /* Included for getting PI and power functions, e powers, etc. */

/* DEFINITION OF SYSTEM SPECS */
#define NUM_THREADS = 2 /* We use 2 threads for now. This will be changed to a higher number later */

/* DEFINITION OF PROGRAM PARAMETERS */
#define RANDOM_SEED 1843397

/* Forward Declarations */
/* Structs */

/* Functions */
float randf_uniform(void);
float randf_std_norm(void);
float generate_asset_price(float start_price, float volatility, float risk_free, float time_delta);

int main(void)
{
    /* Starting timer */
    clock_t start_time = clock();

    /* Setting random number seed */
    srand(RANDOM_SEED);

    float random;
    /* Generating random floats */ 
    for(int i = 0; i < 100000000; i++)
    {
        // printf("Random number %d is: %lf\n", i, randf_std_norm());
        float start_price = 100;
        float volatility = 0.02;
        float risk_free = 0.001;
        float time_delta = 1;
        // printf("Newly generated price is: %f.\n", generate_asset_price(start_price, volatility, risk_free, time_delta));
        generate_asset_price(start_price, volatility, risk_free, time_delta);
    }

    /* End timer and print elapsed time */
    clock_t end_time = clock();
    double time_taken = ((double)(end_time-start_time)/CLOCKS_PER_SEC);
    printf("Time taken was %lf seconds.\n", time_taken);
    return 0;
}

/* Function that returns a double between 0 and 1 */ 
float randf_uniform(void)
{
    float rnd = (float)rand()/(float)(RAND_MAX);
    return rnd;
}

/* Function that returns a normal distributed random number. Uses the Box-Muller transform */
float randf_std_norm(void)
{
    /* Generating 2 random numbers used to generate a random normal number.
    We check if u1 is 0 such that we can immediately return it before generating u2. */
    /* First random number */
    float u1 = randf_uniform();
    /* If u1 is 0, the natural logarithm of u1 is undefined. Therefore, we return 0. This is not exact but should not affect results by a lot. */
    if (u1 == 0)
    {
        return 0;
    }
    /* Second random number */
    float u2 = randf_uniform();

    /* Generate normal random number */
    float rnd = sqrt(-2 * log(u1)) * cos(2*M_PI*u2);
    /* Return random number */
    return rnd;    
}

/* Function that can generate a new asset price using Geometric Brownian Motion. 
Function has four inputs:
- Current asset price
- Volatility over time period (time period should be the same for all inputs and outputs)
- Risk-free rate of return
- Time delta (number of time steps to take) */
float generate_asset_price(float start_price, float volatility, float risk_free, float time_delta)
{
    /* Create a float to return later */
    float new_price;
    /* The new price is equal to: 
    - Start price multiplied by:
    - E to the power of (part A plus part B) 
    - Where:
        - A) (Risk-free rate - 0.5 * volatility squared) * time delta
        - B) volatility * sqrt(time_delta) * a randomly distributed normal number (standard uniform)
    */ 
    new_price = start_price * exp((risk_free - 0.5 * pow(volatility, 2)) * time_delta 
    + volatility * sqrt(time_delta) * randf_std_norm());

    /*Return new price */
    return new_price; 
}

