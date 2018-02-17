// Nathan Park
#include "TacComponentPhysics.h"
#include "TacSerializer.h"
#include "TacCore.h"
namespace Tac
{
  void PhysicsComp::Load( Loader & serializer )
  {
    serializer.Load(mVel);
    serializer.Load(mAngularVel);
  }

  void PhysicsComp::Save( Saver & serializer )
  {
    serializer.SaveFormatted("velocity", mVel);
    serializer.SaveFormatted("angularVel", mAngularVel);
  }

  bool PhysicsComp::Initialize()
  {
    //mAngularVel.x = fmod((float)rand(), 0.3f);
    //mAngularVel.y = fmod((float)rand(), 0.3f);
    //mAngularVel.z = fmod((float)rand(), 0.3f);
    return true;
  }

  PhysicsComp::PhysicsComp()
    : mVel(0,0,0)
    , mAngularVel(0,0,0)
  {


  }
#ifdef TACTWEAK

  void PhysicsComp::AddToTweakBar( TweakBar & bar )
  {
    Tweak & tactweak = GetSystem()->GetCore()->GetTweak();
    TwType twvec3 = tactweak.GetTypeVector3();
    bar.AddVar("PhysicsVelocity", 
      "group=Physics "
      "label='Velocity' ",
      twvec3, &mVel);
    bar.AddVar("PhysicsAngularVelocity", 
      "group=Physics "
      "label='Angular Vel' ",
      twvec3, &mAngularVel);
  }
  void PhysicsComp::RemoveFromTweakBar( TweakBar & bar )
  {
    bar.RemoveVar("PhysicsVelocity");
  }
#endif
} // Tac