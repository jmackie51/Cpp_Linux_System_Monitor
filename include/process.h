#ifndef PROCESS_H
#define PROCESS_H

#include <string>

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid();                   
  std::string User();                    
  std::string Command();                  
  float CpuUtilization();                 
  std::string Ram();                      
  long int UpTime();                       
  bool operator<(Process &a) ;  
  Process(int procID);   // Declare a constructor

  // Declare private members
 private:
 int procID_;
 std::string userName_;
 std::string ram_;
 long procUpTime_;
};

#endif