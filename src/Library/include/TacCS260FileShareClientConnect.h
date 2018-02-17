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
  class FileShareClientConnect : public State<FileShareClient>
  {
  public:
    FileShareClientConnect();
    virtual void Init();
    virtual void Update(float dt);
    
  private:
    SPSocket mConnectSocket;
    std::shared_ptr<AddrInfo> mAddrInfo;
    SPTweakBar mOutputBar;
    Vector2 mPos;
    Vector2 mVel;
    float mMaxSecondsToWait;
    float mSecondsRemaining;
  };

} // Tac
