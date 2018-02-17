/*!

\author Nathan Park

\file RunningAverage.h
\par Project: Tac Engine
\par "All content c 2014 DigiPen (USA) Corporation, all rights reserved.

\brief
   You push a bunch of values into it, and it tells you the average. Fast.

*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once

#include <vector>
#include <ostream>

namespace Tac
{
  // Great for keepipng track of fps 
  class RunningAverage
  {
      float mAverage;
      std::vector<float> mValues;
      unsigned mPushCount;
    public:
      TacDLLAPI RunningAverage(unsigned size); // number of values to keep
      TacDLLAPI float GetAverage() const;
      TacDLLAPI float & GetAverageRef(); // for ant tweak bar
      TacDLLAPI void Push(float val);
      std::ostream & Write(std::ostream & os) const;
  };

  class DifferentRunningAverage
  {
  public:
    DifferentRunningAverage() : mAverage(0), mPushCount(0) {}
    TacDLLAPI void Push (float val)
    {
      mAverage = (mPushCount * mAverage + val) / (1 + mPushCount);
      mPushCount++;
    }
    float GetAverage() const { return mAverage; }
  private:
    float mAverage;
    unsigned mPushCount;
  };

  TacDLLAPI std::ostream & operator << (std::ostream & lhs, const RunningAverage & rhs);

} // Tac