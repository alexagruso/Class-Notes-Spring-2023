#include "stdio.h"
#include "stdlib.h"

// returns an integer from [min,max]
int rand_int(int min, int max) {
    return rand() % max + min;
}

int main() {
    float mtbf = 500.0;

    int totalS1Failures = 0;
    int totalS2Failures = 0;

    printf("      | server 1 rates | server 2 rates\n");
    printf(" year | (failure rate, | (failure rate,\n");
    printf("      |  restore time) |  restore time\n");
    printf("----------------------------------------------\n");
    for (int i = 0; i < 20; i++) {
        // (int) (value + 0.5) rounds to nearest integer
        int averageFailuresPerYear = (int) (8760.0 / mtbf + 0.5);

        // generate number of failures, average is 8760h/500h
        int s1failures = rand_int(0, averageFailuresPerYear);
        totalS1Failures += s1failures;

        int s2failures = rand_int(0, averageFailuresPerYear);
        totalS2Failures += s2failures;

        printf(" %-4d | (%-5d,%-6d) | (%-5d,%-6d) \n", i,
            s1failures, s1failures * 10,
            s2failures, s2failures * 10);
    }

    return EXIT_SUCCESS;
}
