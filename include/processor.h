#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor() {PrevIdle = 0; PrevTotal = 0;}
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
 unsigned long int PrevIdle;
 unsigned long int PrevTotal;

};

#endif