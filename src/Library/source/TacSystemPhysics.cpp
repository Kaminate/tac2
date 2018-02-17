// Nathan Park
#include "TacSystemPhysics.h"
#include "TacCore.h"
#include "TacComponentPhysics.h"
#include "TacComponentTransform.h"
#include "TacMathUtil.h"
namespace Tac
{
  PhysicsSystem::PhysicsSystem()
  {

  }

  void PhysicsSystem::Update( float dt )
  {
    for (SPComp comp : GetComponentSet(Comp::Type::eCompPhysics))
    {
      PhysicsComp * physics = (PhysicsComp*) comp.get();
      TransformComp * transform = (TransformComp*)
        physics->GetSibling(Comp::eCompTransform);

      if (transform)
      {
        // update pos, rot
        //Vector3 vel(0,0,0);
        //vel.z -= 1;
        //vel.x = cos(dt * 8 + transform->mPos.y);
        //vel.y = cos(dt * 6 + transform->mPos.x);

        //vel.x += sin(transform->mPos.z);
        transform->mPos += physics->mVel * dt;
        //float str = sin(transform->mPos.Length()) / transform->mPos.Length();
        //transform->mPos += -transform->mPos * 1.0f / transform->mPos.LengthSq();// * str;
        transform->mRot += physics->mAngularVel * dt;
      }
    }
  }

  void PhysicsSystem::UpdateFakeTime( float dt )
  {
    for (SPComp comp : GetComponentSet(Comp::Type::eCompPhysics))
    {
      PhysicsComp * physics = (PhysicsComp*) comp.get();
      TransformComp * transform = (TransformComp*)
        physics->GetSibling(Comp::eCompTransform);

      if (transform)
      {
        // update fakepos, fakerot
        // update pos, rot
        //Vector3 vel(0,0,0);
        //vel.z -= 1;
        //vel.x = cos(dt * 8 + transform->mPos.y);
        //vel.y = cos(dt * 6 + transform->mPos.x);
        transform->mFakePos += physics->mVel * dt;
        transform->mFakeRot += physics->mAngularVel * dt;
      }
    }
  }

} // Tac