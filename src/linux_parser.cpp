#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <math.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// An example of how to read data from the filesystem
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

// An example of how to read data from the filesystem
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string memname, unit;
  float memval, memfree, cached;
  float totalusedmem, buffermem, memtotal = 0.0f;
  float shmem = 0.0f, sreclaimable = 0.0f;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  if (!stream.is_open())
    return 0;

  while (std::getline(stream, line))
  {
    std::istringstream linestream(line);
    linestream >> memname >> memval >> unit;

    if (memname == "MemTotal:")
      memtotal = memval;

    if (memname == "MemFree:")
      memfree = memval;

    if (memname == "Buffers:")
      buffermem = memval;

    if (memname == "Cached:")
      cached = memval;

    if (memname == "Shmem:")
      shmem = memval;

    if (memname == "Sreclaimable:")
      sreclaimable = memval;
  }

    totalusedmem = memtotal - memfree;
    cached = cached + sreclaimable - shmem;
    totalusedmem -= buffermem + cached;

  return totalusedmem/memtotal;
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string line;
  long uptime1, uptime2;

  if (!stream.is_open())
    return 0;

  std::getline(stream, line);
  std::istringstream linestream(line);

  linestream >> uptime1 >> uptime2;
 
  return uptime1;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// Read and return CPU utilization
vector<long int> LinuxParser::CpuUtilization()
{ 
  vector <long int> vecOut{};
  string cpuName;
  long int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

  std::ifstream stream(kProcDirectory + kStatFilename);

  if (!stream.is_open())
    return {};

  string line;

  while(std::getline(stream, line))
  {
    std::istringstream linestream(line);

    linestream >> cpuName;

    if (cpuName == "cpu")
    {
      linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
      vecOut.push_back(user);
      vecOut.push_back(nice);
      vecOut.push_back(system);
      vecOut.push_back(idle);
      vecOut.push_back(iowait);
      vecOut.push_back(irq);
      vecOut.push_back(softirq);
      vecOut.push_back(steal);
      vecOut.push_back(guest);
      vecOut.push_back(guest_nice);

      return vecOut; 
    }
  }

  return {}; 
}

vector<float> LinuxParser::CpuUtilization(int pid)
{
  vector<float> cpuVals{};

  float time = 0;
  string line, value;

  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);

  if (stream.is_open())
  {
    while(std::getline(stream, line))
    {
      std::istringstream linestream(line);
      for (int i = 1; i <= 22; ++i)
      {
        linestream >> value;

        if (((i >= 14) && (i <= 17)) || i == 22)
        {
          time = std::stof(value) / sysconf(_SC_CLK_TCK);
          cpuVals.push_back(time);
        }
      }
    }
  }
  return cpuVals;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string procs;
  int numProcs = 0, numProcsDummy;

  std::ifstream stream(kProcDirectory + kStatFilename);
  string line;

  if (!stream.is_open())
    return 0;

  while (std::getline(stream, line))
  {
    std::istringstream linestream(line);
    linestream >> procs >> numProcsDummy;
    if (procs == "processes")
      numProcs = numProcsDummy;
  }

  return numProcs; 
  }

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {  
  string procs;
  int runningProcs = 0, numRunnProcsDummy;

  std::ifstream stream(kProcDirectory + kStatFilename);
  string line;

  if (!stream.is_open())
    return 0;

  while (std::getline(stream, line))
  {
    std::istringstream linestream(line);
    linestream >> procs >> numRunnProcsDummy;
    if (procs == "procs_running")
      runningProcs = numRunnProcsDummy;
  }

  return runningProcs;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{ 
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  string cmd;

  if (!stream.is_open())
    return 0;

  string line;
  while (std::getline(stream, line))
  {
    std::istringstream linestream(line);
    linestream >> cmd;
  }

  return cmd;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) 
{
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  string cmd;
  int sizeKB;

  if (!stream.is_open())
    return 0;

  string line;
  while(std::getline(stream, line))
  {
    std::istringstream linestream(line);
    linestream >> cmd >> sizeKB;
    if (cmd == "vmSize:")
      break;
  }

  float res = (float)sizeKB/1024.0f;

  res = floor(res*100.0f + 0.05f);

  res = res/100.0;

  return std::to_string(res);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) 
{
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  string cmd;
  int uid;

  if (!stream.is_open())
    return 0;

  string line;
  while(std::getline(stream, line))
  {
    std::istringstream linestream(line);
    linestream >> cmd >> uid;
    if (cmd == "Uid:")
      break;
  }

  return std::to_string(uid);
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pids) 
{
  std::ifstream stream(kPasswordPath);
  string cmd, throwaway;
  int uid_target = std::stoi(LinuxParser::Uid(pids)) ;
  int uid_passwd;

  if (!stream.is_open())
    return 0;

  string line, out;
  while(std::getline(stream, line))
  {
    
    std::replace(line.begin(), line.end(), ':', ' ');
    
    std::istringstream linestream(line);

    linestream >> cmd >> throwaway >> uid_passwd;

    if (uid_target == uid_passwd)
    {
      break;
    }
  }

  return cmd;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) 
{
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  string dummy;
  int uptime;

  if (!stream.is_open())
    return 0;

  string line;
  while(std::getline(stream, line))
  {
    std::istringstream linestream(line);
    for (int i = 0; i < 22; ++i)
      linestream >> dummy;
    
    uptime = std::stol(dummy) / sysconf(_SC_CLK_TCK);
  }

  return uptime;
}
