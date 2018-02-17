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

namespace Tac
{
  namespace Asteroids
  {
    class IDCallback 
    {
    public:
      virtual void Execute(ID user) = 0;
    };
    DefineSharedPtr(IDCallback);

    class ServerRunning : public ServerState
    {
    public:
      // throws Asteroids::Exception
      ServerRunning(
        StateMachine<Server> * sm, 
        SPSocket listener);
      virtual void Update(float dt);
      void AcceptConnections();
      void HandleMessages();

      void HandleDisconnect(ID disconnected);

      void SendToAll(const OutgoingMessage & m);

      // hooks
      void RemoveCallback(
        std::vector<IDCallback*> & vec, 
        IDCallback * cb);
      std::vector<IDCallback*> mOnJoin;
      std::vector<IDCallback*> mOnLeave;
      std::vector<IDCallback*> mOnReady;

      StateMachine<ServerRunning> mGame;
      SpaceNames mNameGen;
      std::vector<Message::SPHandler> mHandlers;

      Message::HandlerMap mHandlerMap;
      Message::Adder mAdder;

      std::vector<ID> mFreeIds;
      ID mNextId;
      ID GetNextId();

      std::map<ID, Connection> mConnections;
      std::map<ID, std::string> mNames;
      std::map<ID, std::string> mUDPPorts;
      std::map<ID, std::string> mUDPips;

      // may invalidate
      void SendToAllBut(
        ID index, 
        const OutgoingMessage & msg, 
        std::string & errmsg,
        std::vector<ID> & invalidatedIds);

      SPSocket mListener;
#ifdef TACTWEAK

      SPTweakBar mTweakBar;
#endif
    };
    typedef State<ServerRunning> ServerGameState;
    DefineSharedPtr(ServerGameState);
  } // asteroids
} // Tac
