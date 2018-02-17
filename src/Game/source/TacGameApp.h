#pragma once
#include "TacSystemApplication.h"
#include <map>

namespace Tac
{
  class Game : public MiniApp
  {
  public:
    enum State
    {
      eLobby,
      eGame,
      eCount
    };
    Game();
    void Update(float dt) override;

  private:
    void (Game::* mEnterFns[eCount])();
    void (Game::* mUpdateFns[eCount])(float);
    
    void EnterLobby();
    void UpdateLobby(float dt);

    void EnterGame();
    void UpdateGame(float dt);

    State mState;
    State mNextState;
  };
}

