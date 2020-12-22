#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// Template for parsing a key value file
template <typename K, typename V>
std::vector<V> ProcFileParser(std::string path, K k) {
  std::string line;
  K key;
  V value;
  std::vector<V> container;
  std::ifstream fstream(path);
  if (fstream.is_open()) {
    while (std::getline(fstream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == k) {
          container.push_back(value);
          while (linestream >> value) container.push_back(value);
        }
      }
    }
  } else {
    throw std::runtime_error("File " + path + "cant be opened");
  } 
  // remove warning
  return container;
}

// Template for parsing a matrix file
template <typename T>
std::vector<std::vector<T>> ProcFileParser(std::string path) {
  std::string line;
  T value;
  std::vector<std::vector<T>> matrix;
  std::ifstream fstream(path);
  if (fstream.is_open()) {
    while (std::getline(fstream, line)) {
      std::vector<T> list;
      std::istringstream linestream(line);
      while (linestream >> value) {
        list.push_back(value);
      }
      matrix.push_back(list);
    }
  } else {
    throw std::runtime_error("File " + path + "cant be opened");
  } 
  return matrix;
}

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
std::vector<std::string> CpuUtilization();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
int Uid(int pid);
std::string User(int pid);
long int TotalTime(int pid);
long int StartTime(int pid);
long int UpTime(int pid);
};  // namespace LinuxParser

#endif