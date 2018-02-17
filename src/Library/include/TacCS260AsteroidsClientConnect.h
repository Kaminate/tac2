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

#include "TacStateMachine.h"
#include "TacCS260AsteroidsClient.h"
#include "TacTweak.h"
#include "TacVector2.h"

namespace Tac
{
  namespace Asteroids
  {

    class ClientConnect : public State<Client>
    {
    public:
      // throws Asteroids::Exception
      ClientConnect(StateMachine<Client>*sm);
      virtual void Update(float dt);

    private:
      SPSocket mConnectSocket;
      std::shared_ptr<AddrInfo> mAddrInfo;
#ifdef TACTWEAK

      SPTweakBar mOutputBar;
#endif
      Vector2 mPos;
      Vector2 mVel;
      float mMaxSecondsToWait;
      float mSecondsRemaining;
    };
  }

} // Tac
