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

#include "TacSystemApplication.h"
#include "TacTweak.h"
#include "TacMessageUtil.h"
#include "TacStateMachine.h"

#include <thread>
#include <mutex>
#include <list>

namespace Tac
{
  namespace Asteroids
  {
    class Server : public MiniApp
    {
    public:
      TacDLLAPI Server(AppSystem * sys);
      TacDLLAPI virtual void Update(float dt);

      void Error(const std::string & errorMessage);
      void Connect();

    private:
      StateMachine<Server> mStateMachine;
    };

    typedef State<Server> ServerState;
    DefineSharedPtr(ServerState);
  }
} // Tac
