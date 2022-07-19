#include <stdio.h> /* Standard IO */
#include <string.h> /* Included for strings */
#include <stdlib.h> /* Malloc */
#include <Windows.h> /* Sleep Function */

#define NUM_HOUSES 3 /* We have 3 houses on our street */

/* Forward Declarations */
/* Structs */
typedef struct House House;

/* Functions */
void house_print(House *house);
void house_create(House *house, int id, int length, int width, int height, char* street);

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
    /* We first create a pointer for a single house */
    struct House *my_house_0 = (struct House *)malloc(sizeof(struct House));
    /* We test if it is the appropriate size */
    printf("The size of a House * pointer should be %d\n", sizeof(House*));
    printf("The size of my_house_0 pointer as defined is %d\n", sizeof(my_house_0));
    printf("The size of the my_house_0 object is %d\n", sizeof(*my_house_0));
    printf("\n\n");

    /* We create two more houses in the same manner */
    struct House *my_house_1 = (struct House *)malloc(sizeof(struct House));
    struct House *my_house_2 = (struct House *)malloc(sizeof(struct House));

    /* We now create a pointer that points to pointers of houses. This pointer is our street pointer.
    This pointer is a void pointer. */
    void **streetPtr = malloc(sizeof(House*) * NUM_HOUSES);
    streetPtr[0] = (void*) &my_house_0;
    streetPtr[1] = (void*) &my_house_1;
    streetPtr[2] = (void*) &my_house_2;

    /* We build the houses */
    house_create((House*)streetPtr, 0, 3, 4, 2, "Baker St.");
    house_create((House*)streetPtr+1, 1, 5, 5, 3, "Baker St.");
    house_create((House*)streetPtr+2, 2, 4, 2, 1, "Baker St.");

    /* We now display the houses */
    house_print((House*)streetPtr);
    house_print((House*)streetPtr+1);
    house_print((House*)streetPtr+2);

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