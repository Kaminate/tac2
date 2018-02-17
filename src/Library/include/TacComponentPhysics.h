/*!
\author Nathan Park
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once

#include "TacComponent.h"
#include "TacVector3.h"

namespace Tac
{
  class PhysicsComp : public Comp
  {
  public:
    PhysicsComp();
    virtual void Load(Loader & serializer);
    virtual void Save(Saver & serializer);
    virtual bool Initialize();
#ifdef TACTWEAK

    virtual void AddToTweakBar(TweakBar & bar);
    virtual void RemoveFromTweakBar(TweakBar & bar);
#endif

    Vector3 mVel;
    Vector3 mAngularVel;
  };
  DefineSharedPtr(PhysicsComp);

} // Tac
