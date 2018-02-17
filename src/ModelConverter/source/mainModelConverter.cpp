////////////////////////////
// mainModelConverter.cpp //
////////////////////////////

#include "TacCore.h"
#include "TacSystemGraphics.h"
#include "TacSystemTransform.h"
#include "TacSystemPhysics.h"
#include "TacSystemInput.h"
#include "TacSystemApplication.h"
#include "TacComponentTransform.h"
#include <TacComponentModel.h>
#include "TacObjInfo.h"

// remove console window
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "TacGuicon.h"


#include "modelConverterMiniApp.h"

int main(int argc, char ** argv)
{
  using namespace Tac;

  Core * heart = new Core("Editor", 800, 600);
  heart->AddSystem(System::eSystemInput, new InputSystem());
  heart->AddSystem(System::eSystemTransform, new TransformSystem());
  heart->AddSystem(System::eSystemPhysics, new PhysicsSystem());
  heart->AddSystem(System::eSystemGraphics, new GraphicsSystem());
  AppSystem * appSys = new AppSystem();
  heart->AddSystem(System::eSystemApp, appSys);
  appSys->SetMiniApp(SPMiniApp(new converter(appSys)));

  heart->Run();

  delete heart;

  return 0;
}