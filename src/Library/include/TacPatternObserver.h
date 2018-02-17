/*!
\author Nathan Park

\par Project: Tac Engine

\brief

*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once
#include <map>
#include <functional>
#include <vector>

namespace Tac
{
    template<typename Event>
    class Subject
    {
    public:
      typedef std::function<void(void)> CallbackFunction;

      template<typename Event>
      void RegisterObserver(Event e, CallbackFunction onEvent)
      {
        mObservers[e].push_back(onEvent);
      }

      template<typename Event>
      void Notify(Event e)
      {
        for (const auto & obs : mObservers[e])
          obs();
      }

    private:
      std::map<Event, std::vector<CallbackFunction> > mObservers;
    };
} // Tac
