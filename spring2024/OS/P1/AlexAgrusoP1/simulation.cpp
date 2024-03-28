#include <chrono>
#include <cmath>
#include <iostream>
#include <queue>
#include <random>
#include <string>

const int MAXIMUM_PROCESSES = 10000;

std::mt19937 randomEngine{
    static_cast<std::mt19937::result_type>(std::chrono::steady_clock::now().time_since_epoch().count())};

std::uniform_real_distribution compactUnit{0.0, 1.0};
std::uniform_int_distribution diskUniform{1, 5};

enum class DeviceState
{
    IDLE,
    BUSY
};

enum class EventType
{
    CPU_ARRIVAL,
    CPU_DEPARTURE,
    DISK_ARRIVAL,
    DISK_DEPARTURE,
};

struct Process
{
    double arrivalTime;
    double serviceTime;
    double startTime;

    bool hadToUseDisk = false;
    bool hadToWaitCpu = false;
    bool hadToWaitDisk = false;

    static Process* create(double arrivalTime, double serviceTime, double createTime)
    {
        return new Process{arrivalTime, serviceTime, createTime + arrivalTime};
    }

    static void drop(Process* process)
    {
        delete process;
    }
};

struct Event
{
    double arrivalTime;
    EventType type;
    Process* process;

    static Event create(double arrivalTime, EventType type, Process* process)
    {
        return {arrivalTime, type, process};
    }
};

// Comparator for priority queue, sooner arrival time takes higher priority
struct EventComparator
{
    bool operator()(const Event& lhs, const Event& rhs)
    {
        return lhs.arrivalTime > rhs.arrivalTime;
    }
};

void printHelp()
{
    std::cout << "Usage:\n";
    std::cout << "./simulation <average arrival> <average CPU service> <average Disk service> <FLAGS...>\n\n";
    std::cout << "  -h | --help   display this help menu\n";
    std::cout << "  -o | --out    streamlined results output in .csv format\n";
}

double generateCompactUnit()
{
    return ::compactUnit(::randomEngine);
}

double generatePoisson(double lambda)
{
    return (-1.0 / lambda) * std::log(1.0 - generateCompactUnit());
}

double generateExponential(double lambda)
{
    return -lambda * std::log(1.0 - generateCompactUnit());
}

bool willExit()
{
    // 0.6 is a 3/5 chance, so generate integer from 1 to 5 and test if <= 3.
    return ::diskUniform(::randomEngine) <= 3;
}

int main(int argc, char* args[])
{
    if (argc < 2)
    {
        printHelp();
        return -1;
    }

    double averageArrivalRate;
    double averageCPUServiceTime;
    double averageDiskServiceTime;

    bool fileOutput = false;

    for (int i = 1; i < argc; i++)
    {
        std::string currentArgument = static_cast<std::string>(args[i]);

        if (currentArgument[0] == '-')
        {
            if (static_cast<std::string>(args[i]) == "-h" || static_cast<std::string>(args[i]) == "--help")
            {
                printHelp();
                return -1;
            }
            else if (static_cast<std::string>(args[i]) == "-o" || static_cast<std::string>(args[i]) == "--out")
            {
                fileOutput = true;
                continue;
            }
            else
            {
                printHelp();
                return -1;
            }
        }

        double parameter = std::stod(currentArgument);

        switch (i)
        {
            case 1: averageArrivalRate = parameter; break;
            case 2: averageCPUServiceTime = parameter; break;
            case 3: averageDiskServiceTime = parameter; break;
            default: printHelp(); return -1;
        }
    }

    if (averageArrivalRate <= 0)
    {
        std::cerr << "Arrival rate must be greater than zero\n";
        return -1;
    }

    if (averageCPUServiceTime <= 0)
    {
        std::cerr << "Average cpu service time must be greater than zero\n";
        return -1;
    }

    if (averageDiskServiceTime <= 0)
    {
        std::cerr << "Average disk service time must be greater than zero\n";
        return -1;
    }

    int processesCompleted = 0;
    int diskProcesses = 0;

    double clockTime = 0.0;

    double totalCpuTime = 0.0;
    double totalDiskTime = 0.0;
    double totalTurnaroundTime = 0.0;

    int cpuWaitProcesses = 0;
    int diskWaitProcesses = 0;

    std::priority_queue<Event, std::vector<Event>, EventComparator> eventQueue;

    std::queue<Process*> cpuReadyQueue;
    std::queue<Process*> diskReadyQueue;

    DeviceState cpuState = DeviceState::IDLE;
    DeviceState diskState = DeviceState::IDLE;

    eventQueue.push(Event::create(
        0.0, EventType::CPU_ARRIVAL,
        Process::create(generatePoisson(averageArrivalRate), generateExponential(averageCPUServiceTime), 0)));

    while (processesCompleted < MAXIMUM_PROCESSES)
    {
        Event currentEvent = eventQueue.top();
        eventQueue.pop();

        clockTime = currentEvent.arrivalTime;
        Process* currentProcess = currentEvent.process;

        switch (currentEvent.type)
        {
            case EventType::CPU_ARRIVAL:
            {
                if (cpuState == DeviceState::IDLE)
                {
                    cpuState = DeviceState::BUSY;
                    eventQueue.push(Event::create(clockTime + currentProcess->serviceTime, EventType::CPU_DEPARTURE,
                                                  currentProcess));
                }
                else
                {
                    cpuReadyQueue.push(currentProcess);
                    currentProcess->hadToWaitCpu = true;
                }

                Process* nextProcess = Process::create(generatePoisson(averageArrivalRate),
                                                       generateExponential(averageCPUServiceTime), clockTime);
                eventQueue.push(
                    Event::create(clockTime + nextProcess->arrivalTime, EventType::CPU_ARRIVAL, nextProcess));

                break;
            }
            case EventType::CPU_DEPARTURE:
            {
                if (cpuReadyQueue.empty())
                {
                    cpuState = DeviceState::IDLE;
                }
                else
                {
                    Process* nextProcess = cpuReadyQueue.front();
                    cpuReadyQueue.pop();
                    eventQueue.push(
                        Event::create(clockTime + nextProcess->serviceTime, EventType::CPU_DEPARTURE, nextProcess));
                }

                totalCpuTime += currentProcess->serviceTime;

                if (!willExit())
                {
                    currentProcess->serviceTime = generateExponential(averageDiskServiceTime);
                    eventQueue.push(Event::create(clockTime, EventType::DISK_ARRIVAL, currentProcess));

                    if (!currentProcess->hadToUseDisk)
                    {
                        currentProcess->hadToUseDisk = true;
                        diskProcesses++;
                    }

                    break;
                }

                processesCompleted++;
                totalTurnaroundTime += (clockTime - currentProcess->startTime);

                if (currentProcess->hadToWaitCpu)
                {
                    cpuWaitProcesses++;
                }

                if (currentProcess->hadToWaitDisk)
                {
                    diskWaitProcesses++;
                }

                Process::drop(currentProcess);

                break;
            }
            case EventType::DISK_ARRIVAL:
            {
                if (diskState == DeviceState::IDLE)
                {
                    diskState = DeviceState::BUSY;
                    eventQueue.push(Event::create(clockTime + currentProcess->serviceTime, EventType::DISK_DEPARTURE,
                                                  currentProcess));
                }
                else
                {
                    diskReadyQueue.push(currentProcess);
                    currentProcess->hadToWaitDisk = true;
                }

                break;
            }
            case EventType::DISK_DEPARTURE:
            {
                if (diskReadyQueue.empty())
                {
                    diskState = DeviceState::IDLE;
                }
                else
                {
                    Process* nextProcess = diskReadyQueue.front();

                    diskReadyQueue.pop();
                    eventQueue.push(
                        Event::create(clockTime + nextProcess->serviceTime, EventType::DISK_DEPARTURE, nextProcess));
                }

                totalDiskTime += currentProcess->serviceTime;

                currentProcess->serviceTime = generateExponential(averageCPUServiceTime);

                if (cpuState == DeviceState::IDLE)
                {
                    cpuState = DeviceState::BUSY;
                    eventQueue.push(Event::create(clockTime + currentProcess->serviceTime, EventType::CPU_DEPARTURE,
                                                  currentProcess));
                }
                else
                {
                    cpuReadyQueue.push(currentProcess);
                    currentProcess->hadToWaitCpu = true;
                }

                break;
            }
            default: std::cerr << "Invalid event type (this should never happen)\n"; return -1;
        }
    }

    double averageTurnaroundTime = totalTurnaroundTime / processesCompleted;
    double averageThroughput = processesCompleted / clockTime;
    double averageCpuUtilization = (totalCpuTime / clockTime) * 100;
    double averageDiskUtilization = (totalDiskTime / clockTime) * 100;
    double averageCpuWaitProcesses = static_cast<double>(cpuWaitProcesses) / processesCompleted;
    double averageDiskWaitProcesses = static_cast<double>(diskWaitProcesses) / diskProcesses;

    if (fileOutput)
    {
        std::cout << averageArrivalRate << "," << averageTurnaroundTime << "," << averageThroughput << ","
                  << averageCpuUtilization << "," << averageDiskUtilization << "," << averageCpuWaitProcesses << ","
                  << averageDiskWaitProcesses << ",\n";

        return 0;
    }

    std::cout << "Average Turnaround Time: " << averageTurnaroundTime << '\n';
    std::cout << "Average Throughput: " << averageThroughput << '\n';
    std::cout << "Average CPU Utilization: " << averageCpuUtilization << "%\n";
    std::cout << "Average Disk Utilization: " << averageDiskUtilization << "%\n";
    std::cout << "Average CPU Waiting Processes: " << averageCpuWaitProcesses << '\n';
    std::cout << "Average Disk Waiting Processes: " << averageDiskWaitProcesses << '\n';

    return 0;
}
