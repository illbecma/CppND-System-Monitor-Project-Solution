#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() {
  long total_time = LinuxParser::TotalTime(Pid());
  long seconds = LinuxParser::UpTime(Pid());
  CpuUsage((float)(total_time / sysconf(_SC_CLK_TCK)) / seconds);
  return CpuUsage();
}

// Return the command that generated this process
string Process::Command() const { return command_; }

// Accessor for CPU usage
float Process::CpuUsage() const { return cpu_usage_; }

// Mutator for CPU usage
void Process::CpuUsage(float c) {
  cpu_usage_ = c;
}

// Return this process's memory utilization
string Process::Ram() { 
  string ram = LinuxParser::Ram(Pid());
  return ram;
}

// Return the user (name) that generated this process
string Process::User() const { return user_; }

// Return the age of this process (in seconds)
long int Process::UpTime() {
  return LinuxParser::UpTime(Pid());
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return CpuUsage() < a.CpuUsage();
}