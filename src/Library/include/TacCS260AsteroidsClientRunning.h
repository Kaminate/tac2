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

#include "TacCS260AsteroidsClient.h"
#include "TacTweak.h"
#include "TacCS260AsteroidsCommon.h"

namespace Tac
{
  namespace Asteroids
  {

    
    class ClientRunning : public State<Client>
    {
    public:
      // throws
      ClientRunning(StateMachine<Client> * sm, SPSocket server);
      virtual void Update(float dt);

    public:
      std::vector<Message::SPHandler> mHandlers;
      Message::HandlerMap mHandleMap;
      Message::Adder mAdder;


      StateMachine<ClientRunning> mGame;
#ifdef TACTWEAK

      SPTweakBar mTweakBar;
#endif
      ID mId;
      std::string mName;
      Connection mServer; // tcp
    };
    typedef State<ClientRunning> ClientGameState;
    DefineSharedPtr(ClientGameState);
  }
} // Tac
