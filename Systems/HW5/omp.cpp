#include <chrono>
#include <iostream>
#include <omp.h>
#include <random>
#include <vector>

const int ARRAY_SIZE = 1000000;

int main()
{
    using namespace std::chrono;

    // omp_set_num_threads(1);
    // omp_set_num_threads(2);
    // omp_set_num_threads(4);
    // omp_set_num_threads(8);
    omp_set_num_threads(16);

    std::vector<int> array;

    std::random_device device;
    std::mt19937 engine{device()};
    std::uniform_int_distribution<int> randomInt{};

    // initializing array
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        array.push_back(randomInt(engine));
    }

    std::cout << "Entering parallel section..." << '\n';

    high_resolution_clock::time_point t1 = high_resolution_clock::now();

#pragma omp parallel for
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        for (int j = 1; j <= 1000; j++)
        {
            int result = array[i] % j;
        }
    }

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto timespan = duration_cast<duration<double>>(t2 - t1);

    std::cout << "Exiting parallel section..." << '\n';
    std::cout << "Program took " << timespan.count() << " seconds to execute.\n";

    return 0;
}