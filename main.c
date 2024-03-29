#include <stdio.h> /* Standard IO */
#include <pthread.h> /* Multi Threading */
#include <stdlib.h> /* Included for random numbers */
#include <time.h> /* Included for timing our program */
#include <math.h> /* Included for getting PI and power functions, e powers, etc. */
#include <string.h> /* Strings */
#include "libs/random.h" /* Included own random library header file */

/* DEFINITION OF SYSTEM SPECS */
#define NUM_THREADS 20 /* We use NUM_THREADS threads for now. This can be changed to a higher number later */
#define TIME_DELTA 252 /* We use a time step of TIME_DELTA for now */

/* DEFINITION OF PROGRAM PARAMETERS */
#define RANDOM_SEED 1843397
#define BATCH_SIZE 10000
#define NUM_SAMPLES (long long)pow(10,10) /* Samples is NUM_SAMPLES. Cast to long long. */ 
#define TRADING_DAYS 252
#define RISK_FREE_RATE 0.01
#define START_PRICE 100
#define VOLATILITY 0.02
#define STRIKE_PRICE 100
#define MATURITY 252

/* DEFINITION OF MOCKING PARAMETERS */
#define USE_MOCKED_RANDOM 0 /* If this is set to 1, we use mocked random numbers. If 0, we use real random numbers. */

/* Forward Declarations */
/* Structs */
typedef struct Asset Asset;
typedef struct Market Market;
typedef struct Simulator Simulator;
typedef struct Option Option;
typedef struct Args Args;

/* Functions */
double generate_asset_price(double start_price, double volatility, double risk_free, double time_delta);
double rand_std_norm(void);
double random_mock(double *nums, int count);
void asset_print(Asset *asset);
void market_print(Market *market);
void simulator_print(Simulator *simulator);
void option_print(Option *option);
void print_line();
double maxd(double a, double b);
double mind(double a, double b);
void* simulate(void* args);
double compute_path_payoff(Args *args);
double discountd(double value, double risk_free, double time_step);

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

/* Declaration of global variables */
pthread_mutex_t update_mutex; /* Mutex for locking the updating of the option price and iters */

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
    GOOGL->start_price = START_PRICE;
    GOOGL->volatility = VOLATILITY;
    /* Print asset to see if everything is as expected */
    asset_print(GOOGL);

    /* Market Creation */
    Market *STOCK_MARKET;
    STOCK_MARKET = malloc(sizeof(Market));
    STOCK_MARKET->trading_days = TRADING_DAYS;
    double yearly_risk_free = RISK_FREE_RATE;
    double daily_risk_free;
    daily_risk_free = pow( (double)(yearly_risk_free + 1) , ((double)1/(double)(STOCK_MARKET->trading_days))) - 1;
    STOCK_MARKET->risk_free = daily_risk_free;
    /* Print market to see if everything is as expected */
    market_print(STOCK_MARKET);

    /* Simulator Creation */
    Simulator *SIM;
    SIM = malloc(sizeof(Simulator));
    SIM->time_delta = TIME_DELTA;
    SIM->curr_iter = 0;
    SIM->max_iters = NUM_SAMPLES;
    /* Print simulator */
    simulator_print(SIM);

    /* Create option */
    Option *CallOption;
    CallOption = malloc(sizeof(Option));
    CallOption->type = "Call";
    CallOption->price = 0;
    CallOption->strike = STRIKE_PRICE;
    CallOption->maturity = MATURITY;
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
    /* Creating array of threads, as well initialize the mutex for updating our option struct. */
    pthread_t th[NUM_THREADS];
    pthread_mutex_init(&update_mutex, NULL); 

    /* Spawn threads */
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(th+i, NULL, &simulate, args);
    }

    /* Merge threads */
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(th[i], NULL);
    }
    pthread_mutex_destroy(&update_mutex); 

    /* Finally, adjust the price so that it is exactly correct 
    (it may be the case that too many samples are taken due to multithreading) */
    double correction_factor = (double)args->simulator->max_iters/(double)args->simulator->curr_iter;
    args->option->price *= correction_factor;    

    printf("Option price is %lf.\n", args->option->price);
    printf("Number of samples is %lld \n", args->simulator->curr_iter);

    /* End timer and print elapsed time */
    clock_t end_time = clock();
    double time_taken = ((double)(end_time-start_time)/CLOCKS_PER_SEC);
    printf("Time taken was %lf seconds.\n", time_taken);
    return 0;
}

/* Function that can generate a new asset price using Geometric Brownian Motion. 
Function has four inputs:
- Current asset price
- Volatility over time period (time period should be the same for all inputs and outputs)
- Risk-free rate of return
- Time delta (number of time steps to take) */
double generate_asset_price(double start_price, double volatility, double risk_free, double time_delta)
{
    /* Create a float to return later */
    double new_price;
    /* The new price is equal to: 
    - Start price multiplied by:
    - E to the power of (part A plus part B) 
    - Where:
        - A) (Risk-free rate - 0.5 * volatility squared) * time delta
        - B) volatility * sqrt(time_delta) * a randomly distributed normal number (standard uniform)
    */ 
    new_price = start_price * exp((risk_free - 0.5 * pow(volatility, 2)) * time_delta 
    + volatility * sqrt(time_delta) * rand_std_norm());

    /*Return new price */
    return new_price; 
}

/* Function that is responsible for generationg random std normal numbers. Can use either random or mocked random. */
double rand_std_norm(void)
{
    if (USE_MOCKED_RANDOM)
    {
        double numbers[] = {1.1, 0.9, 1, 0.8, 1.2};
        int count = 5;
        return random_mock(numbers, count);
    }
    else
    {
        return randd_std_norm();
    }
}

/* Function that mocks random numbers */
double random_mock(double *nums, int count)
{
    static int i = 0;
    double ret_val = nums[i];
    i++;
    printf("I is %d\n", i);
    if (i == count)
    {
        i = 0;
    }
    return ret_val;
}

/* Function that can print an asset */
void asset_print(Asset *asset)
{
    printf("Asset name is %s.\n", asset->name);
    printf("Start price of asset is %.2lf.\n", asset->start_price);
    printf("Volatility of asset is %.2lf.\n", asset->volatility);
    printf("\n");
}

/* Function that prints the market */
void market_print(Market *market)
{
    printf("Trading days in market are %d.\n", market->trading_days);
    printf("Daily risk-free rate of return is %.10lf.\n", market->risk_free);
    printf("\n");
}

/* Function that prints our simulator */
void simulator_print(Simulator *simulator)
{
    printf("Simulator has time step of %d.\n", simulator->time_delta);
    printf("Simulator is currently at iteration %lld.\n", simulator->curr_iter);
    printf("Simulator will stop after %lld iterations.\n", simulator->max_iters);
    printf("\n");
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
}

/* Max function for doubles */
double maxd(double a, double b)
{
    if(a >= b)
    {
        return a;
    }
    return b;
}

/* Min function for doubles */
double mind(double a, double b)
{
    if (a <= b)
    {
        return a;
    }
    return b;
}

/* Function that is capable of performing multiple iterations of the simulation for a single thread */ 
void* simulate(void* inp_args)
{
    Args *args = (Args *)inp_args;
    while (args->simulator->curr_iter < args->simulator->max_iters)
    {
        double price_update = 0;
        for (int i = 0; i < BATCH_SIZE; i++)
        {
            price_update += discountd((compute_path_payoff(args)), (double)args->market->risk_free, (double)args->option->maturity);
        }
        pthread_mutex_lock(&update_mutex);
    
        args->option->price += price_update/NUM_SAMPLES;
        args->simulator->curr_iter += BATCH_SIZE;

        pthread_mutex_unlock(&update_mutex);
    }
}

/* Function that computes the payoff for a single path (random walk). */
double compute_path_payoff(Args *args)
{
    double strike = (double)args->option->strike;
    double s_0 = (double)args->option->underlying->start_price;
    double volatility = (double)args->option->underlying->volatility;
    double risk_free = (double)args->market->risk_free;
    double time_delta = (double)TIME_DELTA;
    double new_price = s_0;
    int i;

    /* MULTI-STEP APPROACH */
    for (i = 0; i + (int)time_delta < (int)args->option->maturity; i += (int)time_delta) /* Perform the first couple of steps such that we do not go over the maturity, but we use the TIME_DELTA as specified */
    {
        new_price = generate_asset_price(new_price, volatility, risk_free, time_delta);
    }
    /* Use time step of the differene between i and actual maturity as time delta */ 
    if (i != (int)args->option->maturity)
    {
        new_price = generate_asset_price(new_price, volatility, risk_free, ((int)args->option->maturity - i));
    }
    return maxd(0, new_price-strike); /* Return the positive difference between new price and strike. This is the payoff for a single path for a call option */
}

/* Function that can discount doubles. Floats can be cast to doubles to make the function work with floats. */
double discountd(double value, double risk_free, double time_step)
{
    return value * exp(-1*risk_free*time_step);
}