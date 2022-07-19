#include <stdio.h> /* Standard IO */
#include <string.h> /* Included for strings */
#include <stdlib.h> /* Malloc */
#include <Windows.h> /* Sleep Function */
#include <pthread.h> /* Multi Threading */

#define NUM_HOUSES 10 /* We have 3 houses on our street */

/* Forward Declarations */
/* Structs */
typedef struct House House;

/* Functions */
void house_print(House *house);
void house_create(House *house, int id, int length, int width, int height, char* street);
void* thread_running_routine(void* housePtr);

/* Declaration of Structs */
typedef struct House
{
    int id;
    int length;
    int width;
    int height;
    char* street; 
} House;

/* Main Function */
int main(void)
{
    /* We create a void pointer for our street */
    void **streetPtr = malloc(sizeof(House*) * NUM_HOUSES);

    /* Create an array for our threads */
    pthread_t th[NUM_HOUSES];
    
    /* Create a pointer for each house in our street. */
    for (int i = 0; i < NUM_HOUSES; i++)
    {
        House *house = (struct House *)malloc(sizeof(struct House));
        streetPtr[i] = (void*) &house;
    }

    for (long long j = 0; j < NUM_HOUSES; j++)
    {
        pthread_create(th+j, NULL, thread_running_routine, streetPtr[j]);
    }

    /* Merge Threads */
    for (int k = 0; k < NUM_HOUSES; k++)
    {
        pthread_join(th[k], NULL);
    }

    return 0;
}

/* House Creation */
void house_create(House *house, int id, int length, int width, int height, char* street)
{
    printf("Building house.\n\n\n");
    Sleep(3000);
    house->id = id;
    house->length = length;
    house->width = width;
    house->height = height;
    house->street = street;
}

/* Display House */
void house_print(House *house)
{
    printf("House has id %d\n", house->id);
    printf("House has length %d\n", house->length);
    printf("House has width %d\n", house->width);
    printf("House has height %d\n", house->height);
    printf("House is on Street: %s\n", house->street);
    printf("\n\n");
}

// Function for development purposes 
void* thread_running_routine(void* housePtr)
{   
    House *house = (House *)housePtr;
    printf("Spawned thread\n");
    house_create(house, 1, 2, 3, 4, "Baker St.");
    printf("Thread finished execution\n");
    return NULL;
}