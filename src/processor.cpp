#include "processor.h"
#include <vector>
#include "linux_parser.h"
#include <iostream>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    std::vector<long int> read_values;
    read_values = LinuxParser::CpuUtilization();

    long int user = read_values[0];
    long int nice = read_values[1];
    long int system = read_values[2];
    long int idle = read_values[3];
    long int iowait = read_values[4];
    long int irq = read_values[5];
    long int softirq = read_values[6];
    long int steal = read_values[7];

    long int Idle = idle + iowait;
    long int NonIdle = user + nice + system + irq + softirq + steal;
    long int Total = Idle + NonIdle;

    long int totald = Total - PrevTotal;
    long int idled = Idle - PrevIdle;

    PrevTotal = Total;
    PrevIdle = Idle;

    float CPU_Percentage = (float)(totald - idled)/totald;

    return CPU_Percentage;
}