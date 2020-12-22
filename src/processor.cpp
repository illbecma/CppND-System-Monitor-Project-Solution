#include "processor.h"

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  unsigned long int total_jiffies = LinuxParser::Jiffies();
  unsigned long int idle_jiffies = LinuxParser::IdleJiffies();
  unsigned long int totald = total_jiffies - prev_total_;
  unsigned long int idled = idle_jiffies - prev_idle_;
  prev_total_ = total_jiffies;
  prev_idle_ = idle_jiffies;

  return (float)(totald - idled) / totald;
}