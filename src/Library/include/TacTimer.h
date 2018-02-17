/*
  @file     TacTimer.h
  @author   Nathan Park
*/

#ifndef __TAC_TIMER_H
#define __TAC_TIMER_H

//#include <time.h> // clock_t
#include <chrono>

namespace Tac
{
  class Timer
  {
  public:
    Timer();
    float GetElapsedTime();
    void Reset(); 
  private:
    std::chrono::high_resolution_clock::time_point mLastTimePoint;
  };
}

#endif
