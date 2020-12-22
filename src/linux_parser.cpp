#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <numeric>
#include <string>
#include <vector>

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
  string os, version, kernel;
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

/*
Read and return the system memory utilization
Used the first formula for the total memory from
https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
*/
float LinuxParser::MemoryUtilization() {
  unsigned long int memtotal{1}, memfree{1};
  memtotal = ProcFileParser<string, unsigned int>(
      kProcDirectory + kMeminfoFilename, "MemTotal:")[0];
  memfree = ProcFileParser<string, unsigned int>(
      kProcDirectory + kMeminfoFilename, "MemFree:")[0];

  return (float)(memtotal - memfree) / memtotal;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  vector<vector<float>> up_times =
      ProcFileParser<float>(kProcDirectory + kUptimeFilename);
  // return (long) std::accumulate(up_times.begin(), up_times.end(), 0);
  return (long)up_times[0][0];
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<long> jiffies =
      ProcFileParser<string, long>(kProcDirectory + kStatFilename, "cpu");
  unsigned long int usertime = jiffies[0] - jiffies[8];
  unsigned long int nicetime = jiffies[1] - jiffies[9];
  unsigned long int idlealltime = jiffies[3] + jiffies[4];
  unsigned long int systemalltime = jiffies[2] + jiffies[5] + jiffies[6];
  unsigned long int virtalltime = jiffies[8] + jiffies[9];
  unsigned long int totalime = usertime + nicetime + systemalltime +
                               idlealltime + jiffies[7] + virtalltime;
  return totalime;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return ActiveJiffies() - IdleJiffies(); }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<long> jiffies =
      ProcFileParser<string, long>(kProcDirectory + kStatFilename, "cpu");
  // idlealltime = idletime + ioWait
  return jiffies[3] + jiffies[4];
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return ProcFileParser<string, unsigned int>(kProcDirectory + kStatFilename,
                                              "processes")[0];
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return ProcFileParser<string, unsigned int>(kProcDirectory + kStatFilename,
                                              "procs_running")[0];
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  try {
    string full_command = "";
    vector<string> line = ProcFileParser<string>(kProcDirectory + to_string(pid) +
                                               kCmdlineFilename)[0];
    for (const string& word : line) full_command += word + " ";
    return full_command;
  } catch(...) {
    return string();
  }
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  try {
    int ram_size = ProcFileParser<string, int>(kProcDirectory + to_string(pid) + kStatusFilename, "VmSize:")[0];
    return to_string(ram_size / 1000);
  } catch (...) {
    return string();
  }
  
}

// Read and return the user ID associated with a process
int LinuxParser::Uid(int pid) {
  try {
    return ProcFileParser<string, int>(
      kProcDirectory + to_string(pid) + kStatusFilename, "Uid:")[0];
  } catch (...) {
    return 0;
  }
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  try {
    int uid = Uid(pid);
    vector<vector<string>> users = ProcFileParser<string>(kPasswordPath);

    for (vector<string> list : users) {
      string line = list[0];
      std::replace(line.begin(), line.end(), ':', ' ');
      string username, pass;
      int user_id;
      std::istringstream linestream(line);
      linestream >> username >> pass >> user_id;
      if (user_id == uid) 
        return username;
    }
    return string();
  } catch(...) {
    return string();
  }
}

// Read and return the total time of a process in seconds
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
long LinuxParser::TotalTime(int pid) {
  long total_time = 0;
  try {
    vector<string> stats =
      ProcFileParser<string>(kProcDirectory + to_string(pid) + kStatFilename)[0];
    total_time = std::stol(stats[13]) + std::stol(stats[14]) + std::stol(stats[15]) + std::stol(stats[16]);
  } catch(...) {
    return 0;
  }
  
  return total_time;
}

long LinuxParser::StartTime(int pid) {
  try {
    vector<string> stats =
      ProcFileParser<string>(kProcDirectory + to_string(pid) + kStatFilename)[0];
    return std::stoi(stats[21]);
  } catch(...) {
    return 0;
  }
}

long LinuxParser::UpTime(int pid) {
  return UpTime() - (StartTime(pid) / sysconf(_SC_CLK_TCK));
}