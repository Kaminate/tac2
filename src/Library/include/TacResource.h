/*!
\author Nathan Park
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once

#include <time.h>
#include <string>
#include <vector>
#include "TacPatternObserver.h"
#include "TacException.h"

namespace Tac
{
  enum class ResourceEvent { Reload };
  class Resource : public Subject< ResourceEvent >
  {
  public:
    virtual ~Resource();

    void AddPathToWatch(const std::string & path);
    bool IsOutOfDate()const;

    // returns err str
    std::string TryReload();
    virtual std::string ReloadAux() = 0;
    void UpdateFileTimes();

  private:
    time_t GetLastModified(const std::string & path)const throw (...);
    std::vector<std::string> mPaths;
    std::vector<time_t> mLastModified;
  };
} // Tac

