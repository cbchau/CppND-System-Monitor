#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float total_mem, free_mem, value;
  string key;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal") {
        total_mem = value;
      }
      if (key == "MemFree") {
        free_mem = value;
      }
    }   
  }
  return ((total_mem - free_mem)/total_mem); 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long system_time;
  string line, value;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()){
    // Read up time line
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    system_time = std::stol(value);
  }
  return system_time; 
}

// TODO: Read and return the number of jiffies for the system
// Not in use
long LinuxParser::Jiffies() { 
  long uptime = UpTime();
  return uptime/sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  vector<string> tokens;
  long jiffies = 0;
  string token;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> token){
      tokens.push_back(token);
    }
  }
  for (int i = 13; i <= 16; i++){
    jiffies += std::stol(tokens[i]);
  }
  return jiffies/sysconf(_SC_CLK_TCK); 
}

// TODO: Read and return the number of active jiffies for the system
// Not in use
long LinuxParser::ActiveJiffies() { 
  vector<string> tokens;
  long active_jiffies = 0;
  string token;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> token){
      tokens.push_back(token);
    }
  }
  tokens.erase(tokens.begin());
  for (int i = 0; i < int(tokens.size()); i++){
    active_jiffies += std::stol(tokens[i]);
  } 
  return active_jiffies/sysconf(_SC_CLK_TCK); 
}

// TODO: Read and return the number of idle jiffies for the system
// Not in use
long LinuxParser::IdleJiffies() { 
  vector<string> tokens;
  string token;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> token){
      tokens.push_back(token);
    }
  }
  return (std::stol(tokens[3])+ std::stol(tokens[4]))/sysconf(_SC_CLK_TCK); 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpu_util;
  string usage, key;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    if (key == "cpu"){
      while (linestream >> usage){
        cpu_util.push_back(usage);
      }
    }
  }
  return cpu_util; 
}

// Overload of CpuUtilization function
vector <string> LinuxParser::CpuUtilization(int pid) {
  vector<string> cpu_util;
  string usage;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++){
      linestream >> usage;
      if (i == 13) cpu_util.push_back(usage);
      if (i == 14) cpu_util.push_back(usage);
      if (i == 15) cpu_util.push_back(usage);
      if (i == 16) cpu_util.push_back(usage);
      if (i == 21) cpu_util.push_back(usage);
    }
  }
  return cpu_util;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  int value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "proccesses") break;
    }
  }
  return value;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  int value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") break;
    }
  }
  return value;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line, command;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> command;
  }
  return command; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line;
  string key; 
  int value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize"){
        break;
      }
    }
  }
  return (std::to_string(value/1000));
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line, key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while (std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid"){
        break;
      }
    }
  }
  return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line;
  string user, password, uid;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> password >> uid;
      if (uid == LinuxParser::Uid(pid)){
        break;
      }
    }
  }
  return user; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string uptime;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()){
    // Read up time line
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++){
      linestream >> uptime;
    }
  }
  return std::stol(uptime)/sysconf(_SC_CLK_TCK); 
}
