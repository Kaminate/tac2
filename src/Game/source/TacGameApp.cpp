#include "TacGameApp.h"
#include "TacCore.h"
#include "TacSystemGraphics.h"
namespace Tac
{
  Game::Game()
    : mState(eCount)
    , mNextState(eGame)
  {
    mEnterFns[eLobby] = &Game::EnterLobby;
    mUpdateFns[eLobby] = &Game::UpdateLobby;

    mEnterFns[eGame] = &Game::EnterGame;
    mUpdateFns[eGame] = &Game::UpdateGame;
  }

  void Game::Update( float dt )
  {
    if( mNextState != mState )
    {
      mState = mNextState;
      ( this->*mEnterFns[ mState ] )();
      ( this->*mUpdateFns[ mState ] )( dt );
    }
  }

  void Game::EnterGame()
  {
    Core * engine = GetOwner()->GetCore();
    engine->ClearEntities();
    engine->Load("resources/TacLevels/geometric.taclevel");
    GraphicsSystem * graphics 
      = (GraphicsSystem*)engine->GetSystem(System::eSystemGraphics);

    graphics->SetCurrentCamera(
      (CameraComponent*)graphics->GetRandomComp(
      Comp::Type::eCompCamera).get());
    graphics->SetCurrentLight(
      (LightComponent*)graphics->GetRandomComp(
      Comp::Type::eCompLight).get());


    // get num players
    int numPlayers = 1;
    for(int i = 0; i < numPlayers; ++i)
    {
      

    }
  }

  void Game::UpdateGame( float dt )
  {
    dt;

  }

  void Game::EnterLobby()
  {

  }

  void Game::UpdateLobby( float dt )
  {
    dt;

  }

}
