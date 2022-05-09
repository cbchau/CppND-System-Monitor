#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;


// TODO: Return this process's ID
int Process::Pid() { 
    return pid_; 
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    // Get PID processes
    vector<string> cpu_use = LinuxParser::CpuUtilization(pid_);
    // Get uptime
    long uptime = LinuxParser::UpTime();
    // Assign values
    float utime = std::stof(cpu_use[0]);
    float stime = std::stof(cpu_use[1]);
    float cutime = std::stof(cpu_use[2]);
    float cstime = std::stof(cpu_use[3]);
    float starttime = std::stof(cpu_use[4]);

    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - (starttime/sysconf(_SC_CLK_TCK));

    if (seconds != 0){
        cpu_util_ = ((total_time/sysconf(_SC_CLK_TCK))/seconds);
    }  
    else cpu_util_ = 0.0;

    return cpu_util_; 
}

// TODO: Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command(pid_); 
}

// TODO: Return this process's memory utilization
string Process::Ram() { 
    return LinuxParser::Ram(pid_); 
}

// TODO: Return the user (name) that generated this process
string Process::User() { 
    return LinuxParser::User(pid_); 
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
    return (LinuxParser::UpTime() - LinuxParser::UpTime(pid_)); 
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return (this->cpu_util_ < a.cpu_util_)? true:false; 
}

// Constructor
Process::Process (int process_id){
    pid_ = process_id;
    CpuUtilization();
}