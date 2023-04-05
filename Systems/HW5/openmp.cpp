#include <iostream>
#include <random>

std::random_device device;
std::mt19937 engine{device()};
std::uniform_int_distribution<int> randomInt{0};

int main()
{
    for (int i = 0; i < 1000000; i++)
    {
    }

    return 0;
}