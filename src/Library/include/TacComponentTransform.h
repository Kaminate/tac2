/*!
\author Nathan Park

\par Project: Tac Engine

\brief

*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once

#include "TacComponent.h"
#include "TacVector3.h"
#include "TacMatrix4.h"

namespace Tac
{
  class TransformComp : public Comp
  {
  public:
    TransformComp();
    Vector3 mPos;
    Vector3 mRot;
    Vector3 mSca;
    virtual void Load(Loader & serializer);
    virtual void Save(Saver & serializer);
    virtual bool Initialize();
#ifdef TACTWEAK

    virtual void AddToTweakBar(TweakBar & bar);
    virtual void RemoveFromTweakBar(TweakBar & bar);
#endif

    // physics update sets pos
    // physics update sets rot
    // transform update sets mfakepos = pos
    // transform update sets mfakerot = rot

    // physics fakeupdate fakepos += vel*dt
    // physics fakeupdate fakerot += angvel*dt
    // transform fakeupdate sets transform
    Vector3 mFakePos;
    Vector3 mFakeRot;
    Matrix4 mFakeTransform;
    Matrix4 mFakeInverseTransform;
  };
  DefineSharedPtr(TransformComp);

} // Tac
