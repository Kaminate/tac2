#include "TacCore.h"
#include "TacSystemTransform.h"
#include "TacSystemPhysics.h"
#include "TacSystemApplication.h"
#include "TacSystemGraphics.h"
#include "TacCS260AsteroidsServer.h"

// remove console window
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

// Editor Main
int main(int argc, char ** argv)
{
  using namespace Tac;
  using namespace std;

  Core * heart = new Core("Server", 800, 600);
  heart->AddSystem(System::eSystemTransform, new TransformSystem());
  heart->AddSystem(System::eSystemPhysics, new PhysicsSystem());
  heart->AddSystem(System::eSystemApp, new AppSystem());
  heart->AddSystem(System::eSystemGraphics, new GraphicsSystem());
  AppSystem * app = (AppSystem*)heart->GetSystem(System::eSystemApp);
  app->SetMiniApp(SPMiniApp(new Asteroids::Server(app)));

  heart->Load("resources/asteroids.taclevel");

  GraphicsSystem * graphics
    =  (GraphicsSystem*) heart->GetSystem(System::eSystemGraphics);

  graphics->SetCurrentCamera(
    (CameraComponent*)graphics->GetRandomComp(
    Comp::Type::eCompCamera).get());

  graphics->SetCurrentLight(
    (LightComponent*)graphics->GetRandomComp(
    Comp::Type::eCompLight).get());

  heart->Run();
  delete heart;

  return 0;
}