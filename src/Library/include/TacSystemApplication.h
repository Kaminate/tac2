/*!
\author Nathan Park
\brief
*/

#ifndef __TAC_SYSTEM_APPLICATION_H_
#define __TAC_SYSTEM_APPLICATION_H_

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#include "TacSystem.h"
#include "TacErrorIf.h"
#include "TacNetwork.h"
#include "TacMessageUtil.h"

namespace Tac
{
  class AppSystem;
  class MiniApp
  {
  public:
    TacDLLAPI MiniApp(AppSystem * owner);
    TacDLLAPI MiniApp();
    TacDLLAPI virtual void Update(float dt) = 0;
    TacDLLAPI AppSystem * GetOwner();
    TacDLLAPI void SetOwner(AppSystem * owner);
  private:
    AppSystem * mOwner;
  };

  DefineSharedPtr(MiniApp);

  class AppSystem : public System
  {
  public:
    TacDLLAPI AppSystem();
    TacDLLAPI AppSystem(MiniApp * mini);
    TacDLLAPI virtual ~AppSystem();
    TacDLLAPI virtual void Update(float dt);
    TacDLLAPI virtual void UpdateFakeTime(float dt);
    TacDLLAPI void HandleClientMessages(float dt);
    TacDLLAPI void SetMiniApp(SPMiniApp app);

  private:
    SPMiniApp mMiniApp;
  };
} // Tac

#endif
