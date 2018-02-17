#include "TacCore.h"
#include "TacSystemGraphics.h"
#include "TacSystemTransform.h"
#include "TacSystemPhysics.h"
#include "TacSystemEditor.h"
#include "TacSystemInput.h"
//#include <vld.h>
#include "TacComponentTransform.h"
#include <TacComponentModel.h>

// remove console window
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "TacGuicon.h"

#include "TacOctree.h"

// Editor Main
#include "TacGraphics\TacPasses\TacNormalPass.h"

int main(int argc, char ** argv)
{
  using namespace Tac;

  Core * heart = new Core("Editor", 800, 600);
  heart->AddSystem(System::eSystemInput, new InputSystem());
  heart->AddSystem(System::eSystemTransform, new TransformSystem());
  heart->AddSystem(System::eSystemPhysics, new PhysicsSystem());
  GraphicsSystem * graphicsSys = new GraphicsSystem();
  heart->AddSystem(System::eSystemGraphics, graphicsSys);
  EditorSystem * editor = new EditorSystem();
  heart->AddSystem(System::eSystemEditor, editor);

  heart->Init();
  AddNormalPass(graphicsSys);

  //editor->OpenLevel("resources/TacLevels/level.taclevel");
  editor->OpenLevel("resources/TacLevels/level.taclevel");
  
  heart->Run();


  delete heart;

  return 0;
}