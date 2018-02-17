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
#include "TacCS260FileShareClient.h"
#include "TacTweak.h"
#include "TacVector2.h"

namespace Tac
{
  class FileShareClientError : public State<FileShareClient>
  {
  public:
    FileShareClientError(const std::string & errMsg);
    virtual void Init();
    virtual void Update(float dt);
  private:
    std::string mErrMsg;
    SPTweakBar mErrorBar;
    Vector2 mPos;
    Vector2 mVel;
  };

} // Tac
