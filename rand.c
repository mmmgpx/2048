#include "rand.h"
int rander(void) {
     srand(time(NULL)); 
    return rand() % 4;
     // Generate a random number between 0 and 4
}