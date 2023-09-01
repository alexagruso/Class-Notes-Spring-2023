#include "stdio.h"
#include "stdlib.h"
#include "time.h"

// returns an integer from [min,max]
int rand_int(int min, int max) {
    return rand() % max + min;
}

// returns a float from [min,max]
float rand_float(float min, float max) {
    float value = ((float) rand() / RAND_MAX);
    return value * (max - min) + min;
}

// return special random number
void rand_special() {
    int diceRoll = rand_int(1, 10);

    if (diceRoll <= 5) {
        printf("%d\n", 1);
    } else if (diceRoll <= 7) {
        printf("%d\n", 2);
    } else {
        printf("%f\n", rand_float(3.0, 4.0));
    }
}

int main() {
    time_t t;
    srand((unsigned) time(&t));

    printf("100 random integers between 0 and 99\n\n");
    for (int i = 0; i < 100; i++) {
        printf("%d\n", rand_int(0, 99));
    }

    printf("\n100 random numbers between 0.25 and 0.5\n\n");
    for (int i = 0; i < 100; i++) {
        printf("%f\n", rand_float(0.25, 0.5));
    }

    printf("\n100 random numbers according to special distribution\n\n");
    for (int i = 0; i < 100; i++) {
        rand_special();
    }

    return EXIT_SUCCESS;
}
