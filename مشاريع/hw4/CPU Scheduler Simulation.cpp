#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <iomanip>
#include <algorithm>

using namespace std;

struct Process {
    int id, arrivalTime, burstTime, remainingTime, startTime, finishTime, waitingTime, turnaroundTime;
};

void readProcesses(const string& filename, vector<Process>& processes, int& quantum) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error" << endl;
        exit(EXIT_FAILURE);
    }
    int n;
    file >> n >> quantum;
    processes.resize(n);
    for (int i = 0; i < n; ++i) {
        processes[i].id = i + 1;
        file >> processes[i].arrivalTime >> processes[i].burstTime;
        processes[i].remainingTime = processes[i].burstTime;
    }
    file.close();
}

void printGanttChart(const vector<pair<int, int>>& ganttChart) {
    cout << "\nGantt Chart: ";
    for (auto& entry : ganttChart) {
        cout << "[P" << entry.first << " | " << entry.second << "ms] ";
    }
    cout << endl;
}

void calculateAndPrintResults(const vector<Process>& processes, const vector<pair<int, int>>& ganttChart) {
    int totalWaitingTime = 0, totalTurnaroundTime = 0;
    int finishTime = 0;

    cout << left << setw(10) << "Process" << setw(15) << "end Time" << setw(15) << "Waiting Time" << setw(15) << "Turnaround Time" << endl;

    for (const auto& process : processes) {
        totalWaitingTime += process.waitingTime;
        totalTurnaroundTime += process.turnaroundTime;
        finishTime = max(finishTime, process.finishTime);
        cout << left << "P" << setw(9) << process.id << setw(15) << process.finishTime << setw(15) << process.waitingTime << setw(15) << process.turnaroundTime << endl;
    }

    double cpuUtilization = ((double)(finishTime - totalWaitingTime) / finishTime) * 100;

    cout << "\nAverage Waiting Time: " << (double)totalWaitingTime / processes.size() << " ms" << endl;
    cout << "Average Turnaround Time: " << (double)totalTurnaroundTime / processes.size() << " ms" << endl;
    cout << "CPU Utilization: " << cpuUtilization << "%" << endl;
}

void fcfs(const vector<Process>& inputProcesses) {
    vector<Process> processes = inputProcesses;
    vector<pair<int, int>> ganttChart;
    int currentTime = 0;

    for (auto& process : processes) {
        if (currentTime < process.arrivalTime) {
            currentTime = process.arrivalTime;
        }
        ganttChart.push_back({ process.id, currentTime });
        currentTime += process.burstTime;
        process.finishTime = currentTime;
        process.turnaroundTime = process.finishTime - process.arrivalTime;
        process.waitingTime = process.turnaroundTime - process.burstTime;
    }

    printGanttChart(ganttChart);
    calculateAndPrintResults(processes, ganttChart);
}

void srt(const vector<Process>& inputProcesses) {
    vector<Process> processes = inputProcesses;
    vector<pair<int, int>> ganttChart;
    int currentTime = 0, completed = 0;
    Process* currentProcess = nullptr;

    while (completed < processes.size()) {
        for (auto& process : processes) {
            if (process.arrivalTime <= currentTime && process.remainingTime > 0) {
                if (!currentProcess || process.remainingTime < currentProcess->remainingTime) {
                    if (currentProcess) {
                        ganttChart.push_back({ currentProcess->id, currentTime });
                    }
                    currentProcess = &process;
                }
            }
        }

        if (!currentProcess) {
            ++currentTime;
            continue;
        }

        --currentProcess->remainingTime;
        if (currentProcess->remainingTime == 0) {
            ++completed;
            currentProcess->finishTime = currentTime + 1;
            currentProcess->turnaroundTime = currentProcess->finishTime - currentProcess->arrivalTime;
            currentProcess->waitingTime = currentProcess->turnaroundTime - currentProcess->burstTime;
            ganttChart.push_back({ currentProcess->id, currentTime + 1 });
            currentProcess = nullptr;
        }
        ++currentTime;
    }

    printGanttChart(ganttChart);
    calculateAndPrintResults(processes, ganttChart);
}

void rr(const vector<Process>& inputProcesses, int quantum) {
    vector<Process> processes = inputProcesses;
    vector<pair<int, int>> ganttChart;
    queue<int> readyQueue;
    int currentTime = 0;

    auto addToQueue = [&](int currentTime) {
        for (auto& process : processes) {
            bool alreadyInQueue = false;
            std::queue<int> tempQueue = readyQueue;
            while (!tempQueue.empty()) {
                if (tempQueue.front() == process.id) {
                    alreadyInQueue = true;
                    break;
                }
                tempQueue.pop();
            }

            if (process.arrivalTime <= currentTime && process.remainingTime > 0 && !alreadyInQueue) {
                readyQueue.push(process.id);
            }
        }
    };

    while (true) {
        addToQueue(currentTime);

        if (readyQueue.empty()) {
            break;
        }

        int pid = readyQueue.front();
        readyQueue.pop();
        auto& process = processes[pid - 1];

        if (process.remainingTime > quantum) {
            process.remainingTime -= quantum;
            currentTime += quantum;
            ganttChart.push_back({ process.id, currentTime });
        }
        else {
            currentTime += process.remainingTime;
            process.remainingTime = 0;
            process.finishTime = currentTime;
            process.turnaroundTime = process.finishTime - process.arrivalTime;
            process.waitingTime = process.turnaroundTime - process.burstTime;
            ganttChart.push_back({ process.id, currentTime });
        }

        addToQueue(currentTime);
    }

    printGanttChart(ganttChart);
    calculateAndPrintResults(processes, ganttChart);
}

int main() {
    vector<Process> processes;
    int quantum;
    readProcesses("processes.txt", processes, quantum);
    cout << "------------------------------------------------------------------------" << endl;
    cout << "\nFCFS:" << endl;
    fcfs(processes);
    cout << "\n------------------------------------------------------------------------"<<endl;
    cout << "\nSRT:" << endl;
    srt(processes);
    cout << "\n------------------------------------------------------------------------"<<endl;
    cout << "\nRR:" << endl;
    rr(processes, quantum);
    cout << "\n------------------------------------------------------------------------" << endl;
    return 0;
}
