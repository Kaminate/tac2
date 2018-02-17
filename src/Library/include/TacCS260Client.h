/*!
\author Nathan Park
\brief
*/

#pragma once

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#include "TacSystemApplication.h"
#include "TacCS260Common.h"

#include <thread>
#include <mutex>

namespace Tac
{
  class ClientApplication : public MiniApp
  {
  public:

    enum State
    {
      Login,
      ServerBrower, // not in a game, can see other players
      ChallengeAccepted, // waiting for the server to start the game
      GameLobby, // playing a match
      DoNothing,

      Count,
    };
    TacDLLAPI ClientApplication(bool randomName = false);
    TacDLLAPI ~ClientApplication();
    TacDLLAPI bool Init();
    TacDLLAPI void HandleOnEnter();
    TacDLLAPI void HandleUpdate();
    TacDLLAPI virtual void Update(float dt);

    void StateLoginOnEnter();
    void StateLoginUpdate();

    void StateServerBrowserOnEnter();
    void StateServerBrowserUpdate();
    void StateServerBrowserInputThread();

    void StateGameLobbyOnEnter();
    void StateGameLobbyUpdate();
    void StateGameLobbyInputThread();

    State mState;
    State mNextState;
    std::string mUsername;
    ClientIndex mId;
    Connection mConnection;
    std::vector<std::string> mUserMsgs;

    bool mBrowserThreadShouldClose;
    std::thread mBrowserInputThread;
    std::mutex mBrowserInputMutex;

    bool mGameThreadShouldClose;
    std::thread mGameInputThread;
    std::mutex mGameInputMutex;

    Game mWarboats;
    unsigned GetOurPlayerIndex();
    bool IsOurTurn();
    void PrintWhoseTurnItIs();
  };

} // Tac
