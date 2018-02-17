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

#include "TacStateMachine.h"
#include "TacMessageUtil.h"
#include "TacCS260AsteroidsServer.h"
#include "TacCS260AsteroidsCommon.h"
#include "TacCS260AsteroidsServerRunning.h"
#include "TacVector2.h"
#include "TacVector3.h"

namespace Tac
{
  namespace Asteroids
  {
    class Lobby : public State <ServerRunning>
    {
      class OnLeaveCallback : public IDCallback
      {
        Lobby * mL;
      public:
        OnLeaveCallback(Lobby * l);
        virtual void Execute(ID user) override;
      };
      class OnReadyCallback : public IDCallback
      {
        Lobby * mL;
      public:
        OnReadyCallback(Lobby * l);
        virtual void Execute(ID user) override;
      };

    public:
      Lobby(StateMachine<ServerRunning>*sm);
      ~Lobby();
      virtual void Update(float dt) override
      {
        UnusedParameter(dt);
      }
    //private:
      //Message::Adder mAdder;
      //std::vector<Message::SPHandler> mCallbacks;
      
      OnLeaveCallback mLeave;
      OnReadyCallback mReady;

      std::set<ID> mReadyID;
#ifdef TACTWEAK

      SPTweakBar mLobbyBar;
#endif
      void UpdateReadyText();
    };
    
    // throws
    class Gameplay : public State <ServerRunning>
    {
      class OnJoinCallback : public IDCallback
      {
        Gameplay * mG;
      public:
        OnJoinCallback(Gameplay * g);
        virtual void Execute(ID user) override;
      };
      class OnLeaveCallback : public IDCallback
      {
        Gameplay * mG;
      public:
        OnLeaveCallback(Gameplay * g);
        virtual void Execute(ID user) override;
      };

    public:
      Gameplay(StateMachine<ServerRunning>*sm);
      ~Gameplay();
      virtual void Update(float dt) override;

      void RecvInput();
      void SendGamestate();

      void OnGameOver();

      OnJoinCallback mOnJoin;
      OnLeaveCallback mOnLeave;
      Game mAsteroids;
#ifdef TACTWEAK

      SPTweakBar mGameplayBar;
#endif
      SPSocket mUDPSock;
      std::string mUDPPort;
      float mSendWaitTime;
      float mTimeSinceLastSentGamestate;
    };
  } // asteroids
} // Tac
