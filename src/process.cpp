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

//   void ComputeCpuUsage();
//   void ExtractUser();
//   void ExtractCommand();
//   void ComputeRam();
//   void ComputeUpTime();

Process::Process(int pid) : pid_(pid)
{
    Process::ComputeCpuUsage();
    Process::ExtractUser();
    Process::ExtractCommand();
    Process::ComputeRam();
    Process::ComputeUptime();
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return cpuUtilization_;}

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return ram_;  }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return  uptime_;}

// // TODO: Overload the "less than" comparison operator for Process objects
// // REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return a.cpuUtilization_ < Process::cpuUtilization_; }

void Process::ComputeCpuUsage()
{
    long uptime = LinuxParser::UpTime();
    vector<float> cpuVals = LinuxParser::CpuUtilization(Pid());

    if (cpuVals.size() == 5)
    {
        float totalTime = cpuVals[0] + cpuVals[1] + cpuVals[2] + cpuVals[3];
        float seconds = uptime - cpuVals[4];

        cpuUtilization_ = totalTime/seconds;
    }
    else
    {
        cpuUtilization_ = 0.0f;
    }    
}

void Process::ExtractUser()
{
    user_ = LinuxParser::User(Process::Pid());
}

void Process::ExtractCommand()
{
    command_ = LinuxParser::Command(Process::Pid());
}

void Process::ComputeRam() 
{
    ram_ = LinuxParser::Ram(Pid()).substr(0, 5);
}

void Process::ComputeUptime()
{
    uptime_ = LinuxParser::UpTime(Pid());
}
