#include <string>
#include <vector>
#include "processor.h"
#include "linux_parser.h"

using std::string;
using std::vector;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    vector<string> cpu_use = LinuxParser::CpuUtilization();
    // Assign values
    float user = std::stof(cpu_use[0]);
    float nice = std::stof(cpu_use[1]);
    float system = std::stof(cpu_use[2]);
    float idle = std::stof(cpu_use[3]);
    float iowait = std::stof(cpu_use[4]);
    float irq = std::stof(cpu_use[5]);
    float softirq = std::stof(cpu_use[6]);
    float steal = std::stof(cpu_use[7]);
    float guest = std::stof(cpu_use[8]);
    float guest_nice = std::stof(cpu_use[9]);

    // Algorithm
    float cpu_prev_idle = previdle + previowait;
    float cpu_idle = idle + iowait;

    float cpu_prev_nonidle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
    float cpu_nonidle = user + nice + system + irq + softirq + steal;

    float prev_total = cpu_prev_idle + cpu_prev_nonidle;
    float total = cpu_idle + cpu_nonidle;

    float totald = total - prev_total;
    float idled = cpu_idle - cpu_prev_idle;

    float cpu_util;
    try{
        cpu_util = ((totald - idled)/totald);
    } catch(...){
        cpu_util = 0.0;
    }
    
    // Set previous values
    prevuser = user;
    prevnice = nice;
    prevsystem = system;
    previdle = idle;
    previowait = iowait;
    previrq = irq;
    prevsoftirq = softirq;
    prevsteal = steal;
    prevguest = guest;
    prevguest_nice = guest_nice;

    return cpu_util; 
}