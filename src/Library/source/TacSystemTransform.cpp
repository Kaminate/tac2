// Nathan Park
#include "TacSystemTransform.h"
#include "TacCore.h"
#include "TacComponentTransform.h"
#include "TacMacros.h"
namespace Tac
{
  TransformSystem::TransformSystem()
  {
    
  }

  void TransformSystem::Update( float dt )
  {
    UnusedParameter(dt);
    for (SPComp comp : GetComponentSet(Comp::Type::eCompTransform))
    {
      TransformComp * transform = (TransformComp*) comp.get();
      transform->mFakePos = transform->mPos;
      transform->mFakeRot = transform->mRot;
    }
  }

  void TransformSystem::UpdateFakeTime( float dt )
  {
    UnusedParameter(dt);
    for (SPComp comp : GetComponentSet(Comp::Type::eCompTransform))
    {
      TransformComp * transform = (TransformComp*) comp.get();

      transform->mFakeTransform = Matrix4::Transform(
        transform->mSca, 
        transform->mFakeRot, 
        transform->mFakePos);

      transform->mFakeInverseTransform = Matrix4::TransformInv(
        transform->mSca,
        transform->mFakeRot,
        transform->mFakePos);
    }
  }

} // Tac