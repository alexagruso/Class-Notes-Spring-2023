// Alexander Agruso
// CS 3360

#include <cmath>
#include <iomanip>
#include <iostream>
#include <queue>
#include <random>
#include <vector>

// Holds process information
class Process {
  private:
    double arrivalTime;
    double serviceTime;
    double startTime;

    int cpu;

  public:
    Process(double arrivalTime, double serviceTime, double startTime = 0.0, int cpu = -1) {
        this->arrivalTime = arrivalTime;
        this->serviceTime = serviceTime;
        this->startTime = startTime;

        this->cpu = cpu;
    }

    double getArrivalTime() const { return this->arrivalTime; }
    double getServiceTime() const { return this->serviceTime; }
    double getStartTime() const { return this->startTime; }

    int getCPU() const { return this->cpu; }
    void setCPU(int cpu) { this->cpu = cpu; }
};

// ARRIVAL   - Process arrives, is either processed immediately or sent to the ready queue
// DEPARTURE - Process has finished and is removed from the ready queue
enum class EventType { ARRIVAL, DEPARTURE };

// Holds the event type, process, and time
class Event {
  private:
    EventType type;
    Process process{0, 0};
    double time;

  public:
    Event(EventType type, Process process, double time) {
        this->type = type;
        this->process = process;
        this->time = time;
    }

    EventType getType() const { return this->type; }
    Process getProcess() const { return this->process; }
    double getTime() const { return this->time; }
};

// For the priority queue
class EventComparator {
  public:
    bool operator()(const Event& lhs, const Event& rhs) { return lhs.getTime() > rhs.getTime(); }
};

// Self explanatory
enum class CPUState { IDLE, BUSY };

// ReadyQueue is only used in scenario 1
struct CPU {
    std::queue<Process> readyQueue;
    CPUState state;
};

// Mersenne Twister random number engine (rand() sucks)
std::random_device randomDevice;
std::mt19937 engine{randomDevice()};
std::uniform_real_distribution<double> compactUnit{0.0, 1.0}; // compact unit is the set [0, 1]

// Generates double from [min, max]
double generateDoubleInRange(double min, double max) { return compactUnit(engine) * (max - min) + min; }

// Generates properly distributed random poisson value
double generatePoissonValue(double lambda) { return -(1 / lambda) * std::log(1 - compactUnit(engine)); }

// Generates properly distributed random exponential value
double generateExponentialValue(double lambda) { return -lambda * std::log(1 - compactUnit(engine)); }

// Chooses a random cpu 1-4 (0 indexed)
int chooseCPU(int cpuCount) {
    std::uniform_int_distribution<int> cpuRange{0, cpuCount - 1};
    return cpuRange(engine);
}

// Generates a new process with arrival and service times according to
// random distribution
Process generateRandomProcess(double pLambda, double eLambda, double startTime = 0.0, int cpu = 0) {
    double pValue = generatePoissonValue(pLambda);
    double eValue = generateExponentialValue(eLambda);

    return Process{pValue, eValue, startTime, cpu};
}

int main(int argc, char** args) {
    if (argc != 5) {
        std::cerr << "Invalid arguments" << '\n';
        return -1;
    }

    double averageArrivalTime = atof(args[1]);
    double averageServiceTime = atof(args[2]);
    int scenario = atoi(args[3]);
    int cpuCount = atoi(args[4]);

    if (scenario != 1 && scenario != 2) {
        std::cerr << "Invalid scenario, choose 1 or 2\n";
        return -3;
    }

    if (cpuCount < 1) {
        std::cerr << "Invalid number of CPUs, must be at least 1\n";
        return -4;
    }

    double clock = 0.0;

    int processesCompleted = 0;
    std::vector<int> processesPerCPU;

    double totalWaitTime = 0.0;
    double totalServiceTime = 0.0;

    std::vector<double> serviceTimes;

    for (int i = 0; i < cpuCount; i++) {
        processesPerCPU.push_back(0);
        serviceTimes.push_back(0.0);
    }

    std::priority_queue<Event, std::vector<Event>, EventComparator> eventQueue;
    std::vector<CPU> cpus{};

    for (int i = 0; i < cpuCount; i++) {
        cpus.push_back({std::queue<Process>{}, CPUState::IDLE});
    }

    if (scenario == 1) {
        // n CPUs, n ready queues
        Process firstProcess = generateRandomProcess(averageArrivalTime, averageServiceTime, 0.0f, chooseCPU(cpuCount));
        eventQueue.push({EventType::ARRIVAL, firstProcess, firstProcess.getArrivalTime()});

        while (processesCompleted < 10000) {
            Event event = eventQueue.top();
            eventQueue.pop();
            clock = event.getTime();

            switch (event.getType()) {
            case EventType::ARRIVAL: {
                int cpu = event.getProcess().getCPU();

                if (cpus.at(cpu).state == CPUState::IDLE) {
                    cpus.at(cpu).state = CPUState::BUSY;
                    Event newDeparture{EventType::DEPARTURE, event.getProcess(),
                                       clock + event.getProcess().getServiceTime()};
                    eventQueue.push(newDeparture);
                } else {
                    cpus.at(cpu).readyQueue.push(event.getProcess());
                }

                Process newProcess =
                    generateRandomProcess(averageArrivalTime, averageServiceTime, clock, chooseCPU(cpuCount));
                Event newArrival{EventType::ARRIVAL, newProcess, clock + newProcess.getArrivalTime()};
                eventQueue.push(newArrival);

                break;
            }
            case EventType::DEPARTURE: {
                int cpu = event.getProcess().getCPU();

                if (cpus.at(cpu).readyQueue.empty()) {
                    cpus.at(cpu).state = CPUState::IDLE;
                } else {
                    Process readyProcess = cpus.at(cpu).readyQueue.front();
                    Event newDeparture{EventType::DEPARTURE, readyProcess, clock + readyProcess.getServiceTime()};
                    eventQueue.push(newDeparture);
                    cpus.at(cpu).readyQueue.pop();
                }

                processesCompleted++;
                processesPerCPU.at(event.getProcess().getCPU())++;

                totalWaitTime += (clock - event.getProcess().getStartTime() - event.getProcess().getServiceTime());
                totalServiceTime += event.getProcess().getServiceTime();

                serviceTimes[event.getProcess().getCPU()] += event.getProcess().getServiceTime();

                break;
            }
            default:
                std::cerr << "Invalid event type" << '\n';
                return -2;
            }
        }
    } else {
        // n CPUs, 1 ready queue
        std::queue<Process> readyQueue;

        Process firstProcess = generateRandomProcess(averageArrivalTime, averageServiceTime, 0.0f, -1);
        eventQueue.push({EventType::ARRIVAL, firstProcess, firstProcess.getArrivalTime()});

        while (processesCompleted < 10000) {
            Event event = eventQueue.top();
            eventQueue.pop();
            clock = event.getTime();

            switch (event.getType()) {
            case EventType::ARRIVAL: {
                bool foundCPU = false;

                for (int i = 0; i < cpuCount; i++) {
                    if (!foundCPU && cpus.at(i).state == CPUState::IDLE) {
                        foundCPU = true;
                        cpus.at(i).state = CPUState::BUSY;
                        Process newProcess = {event.getProcess().getArrivalTime(), event.getProcess().getServiceTime(),
                                              clock, i};
                        Event newDeparture{EventType::DEPARTURE, newProcess, clock + newProcess.getServiceTime()};
                        eventQueue.push(newDeparture);
                    }
                }

                if (!foundCPU) {
                    readyQueue.push(event.getProcess());
                }

                Process newProcess = generateRandomProcess(averageArrivalTime, averageServiceTime, clock, -1);
                Event newArrival{EventType::ARRIVAL, newProcess, clock + newProcess.getArrivalTime()};
                eventQueue.push(newArrival);

                break;
            }
            case EventType::DEPARTURE: {
                if (readyQueue.empty()) {
                    cpus.at(event.getProcess().getCPU()).state = CPUState::IDLE;
                } else {
                    Process readyProcess = readyQueue.front();
                    readyQueue.pop();
                    readyProcess.setCPU(event.getProcess().getCPU());
                    Event newDeparture{EventType::DEPARTURE, readyProcess, clock + readyProcess.getServiceTime()};
                    eventQueue.push(newDeparture);
                }

                processesCompleted++;
                processesPerCPU.at(event.getProcess().getCPU())++;

                totalWaitTime += (clock - event.getProcess().getStartTime() - event.getProcess().getServiceTime());
                totalServiceTime += event.getProcess().getServiceTime();

                serviceTimes[event.getProcess().getCPU()] += event.getProcess().getServiceTime();

                break;
            }
            default:
                std::cerr << "Invalid event type" << '\n';
                return -2;
            }
        }
    }

    double actualServiceAverage = (totalServiceTime / 10000);
    double actualWaitAverage = (totalWaitTime / 10000);

    double throughput = processesCompleted / clock;

    std::cout << "Average Turnaround:   " << actualServiceAverage + actualWaitAverage << " seconds\n";
    std::cout << "Throughput:           " << processesCompleted / clock << " processes/second\n";

    std::cout << std::setprecision(3);
    std::cout << "Utilization:\n";
    for (int i = 0; i < cpuCount; i++) {
        std::cout << "  CPU " << i << ":              " << (processesPerCPU[i] / (clock)) * 2 << "%\n";
    }

    std::cout << "Average Queue Length: " << actualWaitAverage * averageArrivalTime << " processes\n";

    return 0;
}
