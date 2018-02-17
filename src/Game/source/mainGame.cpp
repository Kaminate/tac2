#include "TacCore.h"
#include "TacSystemGraphics.h"
#include "TacSystemTransform.h"
#include "TacSystemPhysics.h"
#include "TacSystemApplication.h"
#include "TacSystemInput.h"
#include "TacGameApp.h"
//#include <vld.h>
#include <time.h>
#include <math.h>

// remove console window
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main(int argc, char ** argv)
{
  using namespace Tac;
  using namespace std;

  Core * heart = new Core("Game", 800, 600);
  heart->AddSystem(System::eSystemInput, new InputSystem());
  heart->AddSystem(System::eSystemApp, new AppSystem(new Game()));
  heart->AddSystem(System::eSystemTransform, new TransformSystem());
  heart->AddSystem(System::eSystemPhysics, new PhysicsSystem());
  heart->AddSystem(System::eSystemGraphics, new GraphicsSystem());
  heart->Run();
  delete heart;

  return 0;
}
