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
    int id;
    double arrivalTime;
    double serviceTime;
    double startTime;

    static Process create(int id, double arrivalTime, double serviceTime, double currentClockTime)
    {
        return {id, arrivalTime, serviceTime, currentClockTime + arrivalTime};
    }
};

struct Event
{
    double arrivalTime;
    EventType type;
    Process process;

    static Event create(double arrivalTime, EventType type, Process process)
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
    std::cout << "./simulation <average arrival> <average CPU service> <average Disk service>\n\n";
    std::cout << "-h | --help   display this help menu\n";
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
            else // in case any other arguments need to be implemented
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

    int processesCompleted = 0;

    double clockTime = 0.0;

    double totalCpuTime = 0.0;
    double totalDiskTime = 0.0;
    double totalTurnaroundTime = 0.0;

    int cpuReadyProcesses = 0;
    int diskReadyProcesses = 0;

    std::priority_queue<Event, std::vector<Event>, EventComparator> eventQueue;

    std::queue<Process> cpuReadyQueue;
    std::queue<Process> diskReadyQueue;

    DeviceState cpuState = DeviceState::IDLE;
    DeviceState diskState = DeviceState::IDLE;

    eventQueue.push(Event::create(
        0.0, EventType::CPU_ARRIVAL,
        Process::create(0, generatePoisson(averageArrivalRate), generateExponential(averageCPUServiceTime), 0)));
    int processesCreated = 1;

    while (processesCompleted < MAXIMUM_PROCESSES)
    {
        Event currentEvent = eventQueue.top();
        eventQueue.pop();

        clockTime = currentEvent.arrivalTime;
        Process currentProcess = currentEvent.process;

        switch (currentEvent.type)
        {
            case EventType::CPU_ARRIVAL:
            {
                if (cpuState == DeviceState::IDLE)
                {
                    cpuState = DeviceState::BUSY;
                    eventQueue.push(Event::create(clockTime + currentProcess.serviceTime, EventType::CPU_DEPARTURE,
                                                  currentProcess));
                }
                else
                {
                    cpuReadyQueue.push(currentProcess);
                }

                Process nextProcess = Process::create(processesCreated, generatePoisson(averageArrivalRate),
                                                      generateExponential(averageCPUServiceTime), clockTime);
                processesCreated++;
                eventQueue.push(
                    Event::create(clockTime + nextProcess.arrivalTime, EventType::CPU_ARRIVAL, nextProcess));

                cpuReadyProcesses += cpuReadyQueue.size();

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
                    Process nextProcess = cpuReadyQueue.front();

                    cpuReadyQueue.pop();
                    eventQueue.push(
                        Event::create(clockTime + nextProcess.serviceTime, EventType::CPU_DEPARTURE, nextProcess));
                }

                if (!willExit())
                {
                    Process diskProcess = currentProcess;
                    diskProcess.serviceTime = generateExponential(averageDiskServiceTime);
                    eventQueue.push(Event::create(clockTime, EventType::DISK_ARRIVAL, diskProcess));

                    break;
                }

                processesCompleted++;
                totalTurnaroundTime += (clockTime - currentProcess.startTime);
                totalCpuTime += currentProcess.serviceTime;

                break;
            }
            case EventType::DISK_ARRIVAL:
            {
                if (diskState == DeviceState::IDLE)
                {
                    diskState = DeviceState::BUSY;
                    eventQueue.push(Event::create(clockTime + currentProcess.serviceTime, EventType::DISK_DEPARTURE,
                                                  currentProcess));
                }
                else
                {
                    diskReadyQueue.push(currentProcess);
                }

                diskReadyProcesses += diskReadyQueue.size();

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
                    Process nextProcess = diskReadyQueue.front();

                    diskReadyQueue.pop();
                    eventQueue.push(
                        Event::create(clockTime + nextProcess.serviceTime, EventType::DISK_DEPARTURE, nextProcess));
                }

                cpuReadyQueue.push(currentProcess);
                totalDiskTime += currentProcess.serviceTime;

                break;
            }
            default: std::cerr << "Invalid event type (this should never happen)\n"; return -1;
        }
    }

    std::cout << "Average Turnaround Time: " << totalTurnaroundTime / processesCompleted << '\n';
    std::cout << "Average Throughput: " << processesCompleted / clockTime << '\n';
    std::cout << "Average CPU Utilization: " << (totalCpuTime / clockTime) * 100 << "%\n";
    std::cout << "Average Disk Utilization: " << (totalDiskTime / clockTime) * 100 << "%\n";
    std::cout << "Average CPU Processes: " << static_cast<double>(cpuReadyProcesses) / processesCompleted << '\n';
    std::cout << "Average Disk Processes: " << static_cast<double>(diskReadyProcesses) / processesCompleted << '\n';

    return 0;
}
