/*!
\author Nathan Park
\brief
*/

#include "TacCS260AsteroidsClientRunning.h"
#include "TacCS260AsteroidsClientError.h"
#include "TacCS260AsteroidsCommon.h"
#include "TacCS260AsteroidsClientGameState.h"
#include "TacCore.h"
#include <TacVeryUtil.h>
#include "TacMathUtil.h"
#include "Shlwapi.h"
#include <direct.h>

#include "TacSystemGraphics.h"

namespace Tac
{
  namespace Asteroids
  {

#ifdef TACTWEAK


    void TW_CALL FuckThisShitClient(void * data)
    {
      ClientRunning * base = (ClientRunning*) data;
      StateMachine<Client> * sm = base->GetStateMachine();
      sm->SetNextState(
        SPClientState(new ClientError(sm, "Forced quit")));
    }
#endif

    class AssignIDHandler : public Message::Handler
    {
      ClientRunning * mR;
    public:
      AssignIDHandler(ClientRunning * r) : mR(r){}
      virtual void Handle(SPIncomingMessage m) override
      {
        ID newid = m->mData[0].mInt;
        mR->mId = newid;
#ifdef TACTWEAK

        mR->mTweakBar->AddComment("", "ID: " + std::to_string(newid));
#endif
      }
    };

    class AssignNameHandler : public Message::Handler
    {
      ClientRunning * mR;
    public:
      AssignNameHandler(ClientRunning * r) : mR(r){}
      virtual void Handle(SPIncomingMessage m) override
      {
        std::string name = m->mData[0].GetString();
        mR->mName = name;
#ifdef TACTWEAK

        mR->mTweakBar->AddComment("", "Name: " + name);
#endif
      }
    };

    ClientRunning::ClientRunning(
      StateMachine<Client> * sm,
      SPSocket server)
      : State<Client>(sm)
      , mGame(this)
      , mAdder(mHandleMap)
    {
      using namespace Message;

      mServer.mSocket = server;
      mGame.SetNextState(SPClientGameState(new ClientLobby(&mGame)));

      SPHandler idHandler = SPHandler(new AssignIDHandler(this));
      mHandlers.push_back(idHandler);
      mAdder.Add(Message::Type::eAssignID, idHandler.get());

      SPHandler nameHandler = SPHandler(new AssignNameHandler(this));
      mHandlers.push_back(nameHandler);
      mAdder.Add(Message::Type::eAssignName, nameHandler.get());


#ifdef TACTWEAK

      Client * base = Get();
      AppSystem * app = base->GetOwner();
      Tweak & tw = app->GetCore()->GetTweak();

      mTweakBar = tw.Create("Client");
      mTweakBar->SetSize(300, 250);
      mTweakBar->SetPosition(50,50);
      mTweakBar->AddComment("", "Connected to server");
      mTweakBar->AddButton("", "force quit", FuckThisShitClient, this);
#endif
    }

    void ClientRunning::Update( float dt )
    {
      mGame.Update(dt);

      const unsigned bufSize = 256;
      char buffer[bufSize];
      std::vector<SPIncomingMessage> received;
      ReceiveMessages(
        mServer.mSocket, 
        received, 
        mServer.mInTransit, 
        buffer, 
        bufSize);

      if (!mServer.mSocket)
      {
        Get()->Error("Lost connection with server");
        return;
      }

      for (const SPIncomingMessage & msg : received)
      {
        Message::Handle(mHandleMap, msg);
      }

      
    }
  } // Asteroids
} // Tac
