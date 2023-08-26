#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"

// returns a float from [min,max]
float rand_float(float min, float max) {
    float value = ((float) rand() / RAND_MAX);
    return value * (max - min) + min;
}

int main() {
    time_t t;
    srand((unsigned) time(&t));

    // legend
    printf(" process id | arrival time | service time\n");
    printf(" 1 - 1000   | in seconds   | in seconds\n");
    printf("------------------------------------------\n");

    float totalArrivalRate = 0.0f;
    float totalServiceTime = 0.0f;

    for (int i = 0; i < 1000; i++) {
        float arrival = rand_float(0.0, 1.0); // generates random arrival time between 0s and 1s (average arrival 0.5s)
        totalArrivalRate += arrival;

        float service = rand_float(0.0, 2.0); // generates random service time between 0s and 2s (average service 1s)
        totalServiceTime += service;

        printf(" %-10d | %3.10f | %3.10f\n", i + 1, totalArrivalRate, service);
    }

    printf("\nActual average arrival rate: %f/s\n", totalArrivalRate / 1000);
    printf("Actual average service time: %fs\n", totalServiceTime / 1000);

    return EXIT_SUCCESS;
}
