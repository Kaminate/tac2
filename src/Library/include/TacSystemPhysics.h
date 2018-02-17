/*!
\author Nathan Park
\brief
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once

#include "TacSystem.h"

namespace Tac
{
  class PhysicsSystem: public System
  {
  public:
    TacDLLAPI PhysicsSystem();
    TacDLLAPI virtual void Update(float dt);
    TacDLLAPI virtual void UpdateFakeTime(float dt);
  private:
  };

} // Tac
