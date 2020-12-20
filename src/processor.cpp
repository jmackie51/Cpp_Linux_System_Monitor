#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>

// Return the aggregate CPU utilization
float Processor::Utilization() { 
    long totalSytemJiffies = LinuxParser::Jiffies();
    long activeSystemJiffies = LinuxParser::ActiveJiffies();
    long idleSystemJiffies = LinuxParser::IdleJiffies();

    float cpu_utilization = ((float)activeSystemJiffies / (float)totalSytemJiffies);

    return cpu_utilization; 
    
    }