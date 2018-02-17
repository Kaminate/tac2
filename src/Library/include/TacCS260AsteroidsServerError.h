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
#include "TacCS260AsteroidsServer.h"
#include "TacTweak.h"
#include "TacVector2.h"

namespace Tac
{
  namespace Asteroids
  {
    class ServerError : public State<Server>
    {
    public:
      ServerError(StateMachine<Server> * sm, const std::string & errMsg);
      virtual void Update(float dt);

    private:
      std::string mErrMsg;
#ifdef TACTWEAK

      SPTweakBar mErrorBar;
      static void TW_CALL ServerConnect(void * data);
#endif
      Vector2 mPos;
      Vector2 mVel;

    };
  }

} // Tac
