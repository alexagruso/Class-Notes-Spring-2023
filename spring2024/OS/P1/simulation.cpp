#include <iostream>
#include <string>

struct Event
{
};

void printHelp()
{
    std::cout << "Usage:\n";
    std::cout << "./simulation <average arrival> <average CPU service> <average Disk service>\n\n";
    std::cout << "-h | --help   display this help menu\n";
}

int main(int argc, char* args[])
{
    int averageArrivalRate;
    int averageCPUServiceTime;
    int averageDiskServiceTime;

    for (int i = 1; i < argc; i++)
    {
        if (static_cast<std::string>(args[i]) == "-h" || static_cast<std::string>(args[i]) == "--help")
        {
            printHelp();
            return -1;
        }

        switch (i)
        {
            case 1: averageArrivalRate = atoi(args[1]); break;
            case 2: averageCPUServiceTime = atoi(args[2]); break;
            case 3: averageDiskServiceTime = atoi(args[3]); break;
            default: printHelp(); return -1;
        }
    }

    return 0;
}
