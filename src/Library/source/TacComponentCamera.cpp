/*!
\author Nathan Park
*/
#include "TacComponentCamera.h"
#include <TacComponent.h>
#include <TacSystem.h>
#include "TacCore.h"
namespace Tac
{
  void CameraComponent::Load( Loader & serializer )
  {
    serializer.Load(mCamera.mAspectRatio);
    serializer.Load(mCamera.mNear);
    serializer.Load(mCamera.mFar);
    serializer.Load(mCamera.mFieldOfViewYRad);
    serializer.Load(mCamera.mPosition);
    serializer.Load(mCamera.mViewDirection);
    serializer.Load(mCamera.mWorldSpaceUp);
  }

  void CameraComponent::Save( Saver & serializer )
  {
    serializer.SaveFormatted("aspect", mCamera.mAspectRatio);
    serializer.SaveFormatted("near", mCamera.mNear);
    serializer.SaveFormatted("far", mCamera.mFar);
    serializer.SaveFormatted("fovYradians", mCamera.mFieldOfViewYRad);
    serializer.SaveFormatted("position", mCamera.mPosition); // <-- unness
    serializer.SaveFormatted("viewDir", mCamera.mViewDirection);
    serializer.SaveFormatted("worldSpaceUp", mCamera.mWorldSpaceUp);
  }


  bool CameraComponent::Initialize()
  {
    return true;
  }

#ifdef TACTWEAK

  void CameraComponent::AddToTweakBar( TweakBar & bar )
  {
    bar.AddVar("Cam Near", 
      "group=Camera "
      "label='Cam Near' "
      "min=0.001", 
      TW_TYPE_FLOAT, &mCamera.mNear);
    bar.AddVar("Cam Far", 
      "group=Camera "
      "label='Cam Near' "
      "min=1", 
      TW_TYPE_FLOAT, &mCamera.mFar);
    bar.AddVar("Cam FOVYRadians", 
      "group=Camera "
      "label='Cam FOVY radians' "
      "min=1 max=3.14", 
      TW_TYPE_FLOAT, &mCamera.mFieldOfViewYRad);
    bar.AddVar("Cam ViewDir", 
      "group=Camera "
      "label='View Direction'", 
      TW_TYPE_DIR3F, &mCamera.mViewDirection);
    bar.AddVar("Cam Worldspace up", 
      "group=Camera "
      "label='Worldspace Up'", 
      TW_TYPE_DIR3F, &mCamera.mWorldSpaceUp);
  }

  void CameraComponent::RemoveFromTweakBar( TweakBar & bar )
  {
    bar.RemoveVar("Cam Near"); 
    bar.RemoveVar("Cam Far");
    bar.RemoveVar("Cam FOVYRadians"); 
    bar.RemoveVar("Cam ViewDir"); 
    bar.RemoveVar("Cam Worldspace up"); 
  }
#endif

  CameraComponent::~CameraComponent()
  {
    // todo:
    // if it's the current camera, remove it
  }

} // Tac
