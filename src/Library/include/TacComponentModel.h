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
#include "TacGeometry.h"
#include "TacVector4.h"

namespace Tac
{
  class ModelComponent : public Comp
  {
  public:
    TacDLLAPI ModelComponent();
    TacDLLAPI virtual void Load(Loader & serializer);
    TacDLLAPI virtual void Save(Saver & serializer);
    TacDLLAPI virtual bool Initialize();

#ifdef TACTWEAK
    TacDLLAPI virtual void AddToTweakBar(TweakBar & bar);
    TacDLLAPI virtual void RemoveFromTweakBar(TweakBar & bar);
#endif

    TacDLLAPI const Geometry * GetGeometry()const;
    TacDLLAPI void SetMeshFilepath(const std::string & filepath);

    Vector4 mColor;

  private:
    std::string mMeshFilepath;
    SPGeometry mGeom;
  };
  DefineSharedPtr(ModelComponent);

} // Tac
