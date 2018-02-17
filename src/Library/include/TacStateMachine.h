/*!
\author Nathan Park
\brief
*/

#pragma once

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#include <memory> // shared_ptr

namespace Tac
{
  template<class T>
  class StateMachine;

  template<class T>
  class State
  {
  public:
    State(StateMachine<T> * sm) : mStateMachine(sm) {}
    virtual ~State(){}
    virtual void Update(float dt) = 0;
    StateMachine<T> * GetStateMachine()
    {
      return mStateMachine;
    }
    T * Get()
    {
      return mStateMachine->Get();
    }
  private:
    StateMachine<T> * mStateMachine;
  };

  template<class T>
  class StateMachine
  {
  public:
    StateMachine(T * tee) : mT(tee) {}
    void Update(float dt)
    {
      if (mNext)
      {
        mCurrent = mNext;
        mNext.reset();
      }
      if (mCurrent)
        mCurrent->Update(dt);
    }
    void SetNextState(std::shared_ptr<State<T> > next)
    {
      mNext = next;
    }
    T * Get()
    {
      return mT;
    }

  private:
    T * mT;
    std::shared_ptr<State<T> > mCurrent;
    std::shared_ptr<State<T> > mNext;
  };
} // Tac
