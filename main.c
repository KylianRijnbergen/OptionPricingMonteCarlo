#include <stdio.h> /* Standard IO */
#include <pthread.h> /* Multi Threading */
#include <stdlib.h> /* Included for random numbers */
#include <time.h> /* Included for timing our program */
#include <math.h> /* Included for getting PI and power functions, e powers, etc. */
#include <string.h> /* Strings */

/* DEFINITION OF SYSTEM SPECS */
#define NUM_THREADS 2 /* We use NUM_THREADS threads for now. This can be changed to a higher number later */

/* DEFINITION OF PROGRAM PARAMETERS */
#define RANDOM_SEED 1843397
#define BATCH_SIZE 1
#define NUM_SAMPLES (long long)pow(10,7) /* Samples is 10 ** 7. Cast to long long. */ 

/* Forward Declarations */
/* Structs */
typedef struct Asset Asset;
typedef struct Market Market;
typedef struct Simulator Simulator;
typedef struct Option Option;
typedef struct Args Args;

/* Functions */
float randf_uniform(void);
float randf_std_norm(void);
float generate_asset_price(float start_price, float volatility, float risk_free, float time_delta);
// Temporary for development purposes
void* thread_running_routine(void* x);
void asset_print(Asset *asset);
void market_print(Market *market);
void simulator_print(Simulator *simulator);
void option_print(Option *option);
void print_line();
float maxf(float a, float b);
float minf(float a, float b);

/* Declaration of structs */
/* Asset */
typedef struct Asset 
{
    char* name;
    float start_price;
    float volatility;
} Asset;

/* Market */
typedef struct Market
{
    int trading_days;
    double risk_free;
} Market;

/* Simulator */
typedef struct Simulator
{
    int time_delta;
    long long curr_iter;
    long long max_iters;
} Simulator;

/* Option */
typedef struct Option 
{
    char* type;
    float price;
    float strike;
    int maturity;
    Asset* underlying;
} Option;

/* Struct that can be passed to our threads that run the simulation. 
Has attibutes Market, Simulator, Option. Note that Asset is not an attibute of Args. 
This is done because Asset is already an attribute of Option (option.underlying), which is an argument of Args. */
typedef struct Args
{
    Market* market;
    Simulator* simulator;
    Option* option;
} Args;

/* Main Function */
int main(void)
{
    /* Starting timer */
    clock_t start_time = clock();

    /* Setting random number seed */
    srand(RANDOM_SEED);

    /* Create the Asset, Market, Simulator, Option, such that we can combine them into Args which can be passed to our threads. */
    /* Asset Creation */
    Asset *GOOGL;
    GOOGL = malloc(sizeof(Asset));
    GOOGL->name = "Google";
    GOOGL->start_price = 2897.04;
    GOOGL->volatility = 0.020241938215008266;
    /* Print asset to see if everything is as expected*/
    asset_print(GOOGL);

    /* Market Creation */
    Market *STOCK_MARKET;
    STOCK_MARKET = malloc(sizeof(Market));
    STOCK_MARKET->trading_days = 252;
    double yearly_risk_free = 1.97;
    double daily_risk_free;
    daily_risk_free = pow( (double)((yearly_risk_free / 100) + 1) , ((double)1/(double)(STOCK_MARKET->trading_days))) - 1;
    STOCK_MARKET->risk_free = daily_risk_free;
    /* Print market to see if everything is as expected */
    market_print(STOCK_MARKET);

    /* Simulator Creation */
    Simulator *SIM;
    SIM = malloc(sizeof(Simulator));
    SIM->time_delta = 1;
    SIM->curr_iter = 0;
    SIM->max_iters = NUM_SAMPLES;
    /* Print simulator */
    simulator_print(SIM);

    /* Create option */
    Option *CallOption;
    CallOption = malloc(sizeof(Option));
    CallOption->type = "Call";
    CallOption->price = 0;
    CallOption->strike = 2900;
    CallOption->maturity = 252;
    CallOption->underlying = GOOGL;
    /* Print Option */
    option_print(CallOption);

    /* Create Args */
    Args *args;
    args = malloc(sizeof(Args));
    args->market = STOCK_MARKET;
    args->simulator = SIM;
    args->option = CallOption;

    /* MULTITHREADING PART */
    /* Creating array of threads */
    pthread_t th[NUM_THREADS];

    /* Spawn threads */
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(th+i, NULL, &thread_running_routine, NULL);
    }

    /* Merge threads */
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(th[i], NULL);
    }

    printf("Number of samples is %lld \n", NUM_SAMPLES);


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

// Function for development purposes 
void* thread_running_routine(void* x)
{   
    randf_std_norm();
    return NULL;
}

/* Function that can print an asset */
void asset_print(Asset *asset)
{
    printf("Asset name is %s.\n", asset->name);
    printf("Start price of asset is %.2lf.\n", asset->start_price);
    printf("Volatility of asset is %.2lf.\n", asset->volatility);
    printf("\n");
    return NULL;
}

/* Function that prints the market */
void market_print(Market *market)
{
    printf("Trading days in market are %d.\n", market->trading_days);
    printf("Daily risk-free rate of return is %.10lf.\n", market->risk_free);
    printf("\n");
    return NULL;
}

/* Function that prints our simulator */
void simulator_print(Simulator *simulator)
{
    printf("Simulator has time step of %d.\n", simulator->time_delta);
    printf("Simulator is currently at iteration %lld.\n", simulator->curr_iter);
    printf("Simulator will stop after %lld iterations.\n", simulator->max_iters);
    printf("\n");
    return NULL;
}

/* Function that prints our option */
void option_print(Option *option)
{
    print_line();
    printf("This is an option on the stock of %s.\n\n", option->underlying->name);
    printf("Contract parameters are: \n");
    printf("- Type: %s.\n", option->type);
    printf("- Strike: %.2lf.\n", option->strike);
    printf("- Time to maturity: %d.\n", option->maturity);
    printf("\n");
    printf("The current price of Google stock is %.2lf.\n", option->underlying->start_price);
    print_line();
    printf("\n\n");
}

/* Function that prints a line to the console */
void print_line()
{
    for (int i = 0; i < 100; i++)
    {
        printf("_");
    }
    printf("\n\n");
    return NULL;
}

/* Max function for floats */
float maxf(float a, float b)
{
    if(a >= b)
    {
        return a;
    }
    return b;
}

/* Min function for floats */
float minf(float a, float b)
{
    if (a <= b)
    {
        return a;
    }
    return b;
}