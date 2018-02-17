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
#include "TacCS260FileShareServer.h"
#include "TacTweak.h"
#include "TacVector2.h"

namespace Tac
{
  class FileShareServerError : public State<FileShareServer>
  {
  public:
    FileShareServerError(
      StateMachine<FileShareServer> * sm,
      const std::string & errMsg);
    virtual void Init();
    virtual void Update(float dt);

  private:
    std::string mErrMsg;
    SPTweakBar mErrorBar;
    Vector2 mPos;
    Vector2 mVel;
  };

} // Tac
