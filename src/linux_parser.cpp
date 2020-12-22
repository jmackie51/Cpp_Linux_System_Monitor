#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "system.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::replace;
using std::stol;


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
  string os, kernel, junk;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> junk >> kernel;
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 

  string line;
  float freeMemory =0;
  float totMemory =0;
  float buffMemory=0;
  float useMemoryFactor;
  string mem_vals;
  vector<string> memory_file_line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  if (stream.is_open()) {
    while(std::getline(stream, line)){
      memory_file_line.clear();
      std::istringstream linestream(line);

      while(linestream >> mem_vals){
        memory_file_line.push_back(std::move(mem_vals));
      }

      if (memory_file_line[0] == "MemTotal:") totMemory = stof(memory_file_line[1]);
      if (memory_file_line[0] == "MemFree:") freeMemory = stof(memory_file_line[1]);
      if (memory_file_line[0] == "Buffers:") buffMemory = stof(memory_file_line[1]);

      if (freeMemory != 0 and totMemory != 0 and buffMemory != 0) break;

    }
  }

  useMemoryFactor = (float)(freeMemory/(totMemory - buffMemory));
  return (1- useMemoryFactor); 
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  long value;
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> value;
  }
  
  return value; 
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long tot;
  
  vector<string>  totSystemJiffies = LinuxParser::CpuUtilization();
  for(const auto &workPerType: totSystemJiffies){
    if (workPerType != "") tot += std::stol(workPerType);
  }
  
  return tot; 
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  
  vector<string> ActiveSystemJiffies = LinuxParser::CpuUtilization();
  return std::stol(ActiveSystemJiffies[2]); 

}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  
  vector<string> IdleSystemJiffies = LinuxParser::CpuUtilization();
  return std::stol(IdleSystemJiffies[3]); 
  
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line;
  string cpuValues;
  vector<string> cpu_utilization;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    while(linestream >> cpuValues){
      if(cpuValues != "cpu") cpu_utilization.push_back(std::move(cpuValues));
    }
  }

  return cpu_utilization; 
  
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key, value;
  int numProcesses;
  string line_vals;
  vector<string> line_Vector;
  std::ifstream stream(kProcDirectory + kStatFilename);

  numProcesses = 0;
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      line_Vector.clear();
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") numProcesses = stoi(value);

      if (numProcesses != 0) break;

    }
  }

  return numProcesses; 
  
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  
  string line, key, value;
  int runningProcesses;
  string line_vals;
  vector<string> line_Vector;
  std::ifstream stream(kProcDirectory + kStatFilename);

  runningProcesses = 0;
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      line_Vector.clear();
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") runningProcesses = stoi(value);

      if (runningProcesses != 0 ) break;

    } 
  }
 
  return runningProcesses; 
 
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string cmdLine; 
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
  } 

  return line; 
  
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line, key, value;
  string Ram;
  string line_vals;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);

  Ram = "";
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") Ram = value;
      if (Ram != "") break;
    }
  }
  return Ram; //returns ram in kB
  
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {

  string line, key, value;
  string Uid;
  string line_vals;

  Uid = "";
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") Uid = value;

      if (Uid != "" ) break;

    } 
  } 
  
  return Uid; 
  }

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string userName; 
  string Uid = LinuxParser::Uid(pid);
  string line, key, nonSense, value;
  string line_vals;
  std::ifstream stream(kPasswordPath);

  userName = "";
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      replace(line.begin(), line.end(), ':',' ');
      std::istringstream linestream(line);
      linestream >> value >> nonSense >> key;
      if (key == Uid) userName = value;

      if (userName != "" ) break;
    } 
  } 
  return userName; 
  }

// Read and return the required metrics from the linux folder system needed to calculate process CPU usage
std::vector<long> LinuxParser::ProcStats(int pid) { 
  
  string os, values;
  string line;
  vector<string> line_Vector;
  vector<long> retVec;

  line_Vector.clear();
  retVec.clear();
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    //convert line into a vector containing all the information
    while(linestream >> values){
      line_Vector.push_back(std::move(values));
    }

    /*retVec positions 0. #14 utime  1. #15 stime  2. #16 cutime  3. #17 cstime  4. #22 startrime 5. system uptime*/
    retVec.push_back(std::move(stol(line_Vector[13])));
    retVec.push_back(std::move(stol(line_Vector[14])));
    retVec.push_back(std::move(stol(line_Vector[15])));
    retVec.push_back(std::move(stol(line_Vector[16])));
    retVec.push_back(std::move(stol(line_Vector[21])));
    retVec.push_back(std::move(LinuxParser::UpTime()));

  }

  return retVec; 
  }