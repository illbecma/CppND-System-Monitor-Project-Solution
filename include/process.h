#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int p, std::string u, std::string c)
      : pid_(p), user_(u), command_(c) {}
  int Pid() const;                         
  std::string User() const;                
  std::string Command() const;             
  float CpuUtilization();                  
  std::string Ram();                       
  long int UpTime();                       
  bool operator<(Process const& a) const; 
  float CpuUsage() const;
  void CpuUsage(float c);

  // TODO: Declare any necessary private members
 private:
  int pid_ = 0;
  float cpu_usage_ = 0.0;
  std::string user_ = "";
  std::string command_ = "";
};

#endif