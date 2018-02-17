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
#include <thread>
#include <mutex>
#include "TacTweak.h"
#include "TacStateMachine.h"

namespace Tac
{
  namespace Asteroids
  {
    class Client : public MiniApp
    {
    public:
      TacDLLAPI Client(AppSystem * spp);
      TacDLLAPI virtual void Update(float dt);
      void Error(const std::string & errorMessage);
      void Connect();

    private:
      StateMachine<Client> mStateMachine;

    };
    typedef State<Client> ClientState;
    DefineSharedPtr(ClientState);
  }

} // Tac
