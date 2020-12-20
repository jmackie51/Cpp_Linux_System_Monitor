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

//define constructor
Process::Process(int procID){ 
    procID_ = procID;
    userName_ = LinuxParser::User(procID);
    ram_ = to_string(stol(LinuxParser::Ram(procID)) / 1000); //convert Kb to Mb
    
    }

// Return this process's ID
int Process::Pid() { return procID_; }

// Return this process's CPU utilization
float Process::CpuUtilization(){ 
    vector<long> cpuStats = LinuxParser::ProcStats(procID_); // get all required metrics from the linux folder system.
    long utime = cpuStats[0];
    long stime = cpuStats[1];
    long cutime = cpuStats[2];
    long cstime = cpuStats[3];
    long starttime = cpuStats[4];
    long sysUpTimeSec = cpuStats[5];
    std::string str;

    //determine total time spent or the process
    long total_time  = utime + stime;
    //add children time to process time
    total_time = total_time + cutime + cstime;

    //get total elapsed time in seconds since the process started
    long seconds = sysUpTimeSec  - (starttime / (sysconf(_SC_CLK_TCK)) ); // (sysconf(_SC_CLK_TCK)) returns clock frequency
    procUpTime_ = seconds; //save process uptime for later use in the "Time+" field

    //calculation the CPU usage percentage
    float cpuUsage = (((float)total_time /(sysconf(_SC_CLK_TCK))) / (float)seconds);

    return cpuUsage; 
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(procID_); }

// Return this process's memory utilization
string Process::Ram() { return ram_; }

// Return the user (name) that generated this process
string Process::User() { return userName_; }

// Return the age of this process (in seconds)
long int Process::UpTime() { return procUpTime_; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process &a) { 
    return a.CpuUtilization() < this->CpuUtilization(); 
}