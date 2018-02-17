// Nathan Park
#include "TacSystemApplication.h"
#include "TacMacros.h"
namespace Tac
{
  AppSystem::AppSystem()
  {

  }

  AppSystem::AppSystem( MiniApp * mini )
    : mMiniApp(mini)
  {
    mini->SetOwner(this);
  }

  AppSystem::~AppSystem()
  {

  }

  void AppSystem::Update( float dt )
  {
    if (mMiniApp)
      mMiniApp->Update(dt);
  }

  void AppSystem::UpdateFakeTime( float dt )
  {
    UnusedParameter(dt);
  }

  void AppSystem::HandleClientMessages( float dt )
  {
    UnusedParameter(dt);
  }

  void AppSystem::SetMiniApp( SPMiniApp app )
  {
    mMiniApp = app;
  }

  /////////////
  // MiniApp //
  /////////////
  MiniApp::MiniApp(AppSystem * owner) : mOwner(owner)
  {

  }
 
  MiniApp::MiniApp() : mOwner(nullptr)
  {

  }

  AppSystem * MiniApp::GetOwner()
  {
    return mOwner;
  }

  void MiniApp::SetOwner( AppSystem * owner )
  {
    mOwner = owner;
  }

}