/*
  @file     TacTimer.cpp
  @author   Nathan Park
*/

#include "TacTimer.h"
namespace Tac
{
  Timer::Timer() 
  {
    Reset();
  }

  void Timer::Reset()
  {
    mLastTimePoint = std::chrono::high_resolution_clock::now();
  }

  float Timer::GetElapsedTime()
  {
    using namespace std::chrono;
    high_resolution_clock::time_point now = high_resolution_clock::now();

    duration<float> timeDiff 
      = duration_cast<duration<float> >(now - mLastTimePoint);

    return timeDiff.count();
  }

} // Tac
