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

  public:
    Process(double arrivalTime, double serviceTime, double startTime = 0.0) {
        this->arrivalTime = arrivalTime;
        this->serviceTime = serviceTime;
        this->startTime = startTime;
    }

    double getArrivalTime() const { return this->arrivalTime; }
    double getServiceTime() const { return this->serviceTime; }
    double getStartTime() const { return this->startTime; }
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

// Generates a new process with arrival and service times according to
// random distribution
Process generateRandomProcess(double pLambda, double eLambda, double startTime = 0.0) {
    double pValue = generatePoissonValue(pLambda);
    double eValue = generateExponentialValue(eLambda);

    return Process{pValue, eValue, startTime};
}

int main(int argc, char** args) {
    if (argc != 3) {
        std::cerr << "Invalid arguments" << '\n';
        return -1;
    }

    double averageArrivalTime = atof(args[1]);
    double averageServiceTime = atof(args[2]);

    double oldClock = 0.0;
    double clock = 0.0;

    int processesCompleted = 0;

    int iterations = 0;
    float totalTurnaroundTime = 0.0;
    float totalCPUTime = 0;
    int totalReadyQueueProcesses = 0;

    CPUState cpuState = CPUState::IDLE;

    std::priority_queue<Event, std::vector<Event>, EventComparator> eventQueue;
    std::queue<Process> readyQueue;

    Process firstProcess = generateRandomProcess(averageArrivalTime, averageServiceTime);
    eventQueue.push({EventType::ARRIVAL, firstProcess, firstProcess.getArrivalTime()});

    while (processesCompleted < 10000) {
        Event event = eventQueue.top();
        eventQueue.pop();
        clock = event.getTime();

        switch (event.getType()) {
        case EventType::ARRIVAL: {
            if (cpuState == CPUState::IDLE) {
                cpuState = CPUState::BUSY;
                Event newDeparture{EventType::DEPARTURE, event.getProcess(),
                                   clock + event.getProcess().getServiceTime()};
                eventQueue.push(newDeparture);
            } else {
                readyQueue.push(event.getProcess());
                totalCPUTime += clock - oldClock;
            }

            Process newProcess = generateRandomProcess(averageArrivalTime, averageServiceTime, clock);
            Event newArrival{EventType::ARRIVAL, newProcess, clock + newProcess.getArrivalTime()};
            eventQueue.push(newArrival);
            break;
        }
        case EventType::DEPARTURE: {
            if (readyQueue.empty()) {
                cpuState = CPUState::IDLE;
            } else {
                Process readyProcess = readyQueue.front();
                Event newDeparture{EventType::DEPARTURE, readyProcess, clock + readyProcess.getServiceTime()};
                eventQueue.push(newDeparture);
                readyQueue.pop();
                totalCPUTime += clock - oldClock;
            }

            totalTurnaroundTime += clock - event.getProcess().getStartTime();
            processesCompleted++;
            break;
        }
        default:
            std::cout << "Invalid event type" << '\n';
            return -2;
        }

        if (cpuState == CPUState::BUSY) {
        }

        totalReadyQueueProcesses += readyQueue.size();
        iterations++;

        oldClock = clock;
    }

    std::cout << std::setprecision(3);
    std::cout << "Average turnaround:      " << totalTurnaroundTime / 10000 << " seconds" << '\n';
    std::cout << "Total throughput:        " << 10000 / clock << " processes/second\n";
    std::cout << "CPU Utilization:         " << (totalCPUTime / clock) * 100.0 << "%\n";
    std::cout << std::setprecision(4);
    std::cout << "Average ready processes: " << totalReadyQueueProcesses / (float)iterations << '\n';

    return 0;
}
