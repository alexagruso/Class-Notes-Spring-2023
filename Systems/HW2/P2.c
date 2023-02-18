#include "stdio.h"
#include "stdlib.h"
#include "time.h"

// returns an integer from [min,max)
int rand_int(int min, int max) {
    return rand() % max + min;
}

float rand_float(float min, float max) {

}

int main() {
    time_t t;

    srand((unsigned) time(&t));

    // print 100 random integers between 0 and 99
    for (int i = 0; i < 100; i++) {
        printf("%d\n", rand_int(0, 99));
    }

    // print 100 random numbers between 0.25 and 0.75
    // for (int i = 0; i < 100; i++) {
    //     float value = ((float) rand()) / ((float) RAND_MAX);
    //     value = (value / 4.0) + (1.0 / 4.0);
    //     printf("%f\n", value);
    // }

    return EXIT_SUCCESS;
}
