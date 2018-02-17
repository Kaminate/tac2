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
    class ClientError : public State<Client>
    {
    public:
      ClientError(
        StateMachine<Client>*sm,
        const std::string & errMsg);
      virtual void Update(float dt);
    private:
      std::string mErrMsg;
#ifdef TACTWEAK

      SPTweakBar mErrorBar;
#endif
      Vector2 mPos;
      Vector2 mVel;
    };
  }
} // Tac
