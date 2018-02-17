/*!
\author Nathan Park
\brief
*/

#include "TacCore.h"
#include "TacCS260AsteroidsServerRunning.h"
#include "TacCS260AsteroidsServerError.h"
#include "TacCS260AsteroidsCommon.h"
#include "TacCS260AsteroidsServerGameState.h"
#include "TacRunningAverage.h"

namespace Tac
{
  namespace Asteroids
  {
    class ChatHandler : public Message::Handler
    {
      ServerRunning * mR;
    public:
      ChatHandler(ServerRunning * r) : mR(r){}
      virtual void Handle(SPIncomingMessage m) override
      {
        OutgoingMessage out(*m);
        mR->SendToAll(out);
      }
    };

#ifdef TACTWEAK

    void TW_CALL FuckThisShitServer(void * data)
    {
      ServerRunning * base= (ServerRunning*) data;
      base->GetStateMachine()->SetNextState(
        SPServerState(new ServerError(
        base->GetStateMachine(),
        "todo: ServerRunning::Init()")));
    }
#endif

    ServerRunning::ServerRunning(
      StateMachine<Server> * sm, 
      SPSocket listener)
      : State<Server>(sm)
      , mListener(listener)
      , mNextId(0)
      , mGame(this)
      , mAdder(mHandlerMap)
    {
      mGame.SetNextState(SPServerGameState(new Lobby(&mGame)));

      using namespace Message;

      SPHandler chat = SPHandler(new ChatHandler(this));
      mHandlers.push_back(chat);
      mAdder.Add(eChat, chat.get());

#ifdef TACTWEAK

      Server * owner = this->Get();
      AppSystem * appsys = owner->GetOwner();

      mTweakBar = appsys->GetCore()->GetTweak().Create("Server");
      mTweakBar->SetSize(275, 200);
      mTweakBar->AddButton("", "force quit", FuckThisShitServer, this);
#endif

      std::string configfilepath("config.txt");
      std::ifstream ifs(configfilepath);
      if (ifs.is_open())
      {
      }
      else
      {
        throw(Exception("Cannot open " + configfilepath));
      }

    }

    void ServerRunning::Update( float dt )
    {
      //static RunningAverage mRunning(60);
      //Timer mTimer;
      //mTimer.Start();

      mGame.Update(dt);
      AcceptConnections();
      HandleMessages();

      //mTimer.Stop();
      //mRunning.Push(mTimer.GetElapsedTime());
      //mTimer.Reset();
      //std::cout << mRunning.GetAverage() * 1000 << " ms" << std::endl;
    }

    void ServerRunning::AcceptConnections()
    {
      // listen and accept connections.
      SOCKET accepted = accept(mListener->mSocket, nullptr, nullptr);
      if (accepted == INVALID_SOCKET && WSAGetLastError() != WSAEWOULDBLOCK)
      {
        PrintWSAErrorReturn("accept failed");
      }
      else // the socket is valid
      {
        SPSocket acceptedSocket(new Socket(accepted));
        if (acceptedSocket->SetBlocking(false))
        {
          Connection newConnection;
          newConnection.mSocket = acceptedSocket;

          ID id = GetNextId();
          mConnections[id] = newConnection;

#ifdef TACTWEAK

          mTweakBar->AddComment("", 
            "Added a connection. " 
            + std::to_string(mConnections.size()) 
            + " connections.");
#endif

          using namespace Message;

          OutgoingMessage msg(tostring(Type::eAssignID).c_str());
          msg.AddData(DataPtr(&id));
          SendMsg(acceptedSocket, msg); 

          std::string name = mNameGen.GetName();
          mNames[id] = name;

          OutgoingMessage nameMsg(tostring(Type::eAssignName).c_str());
          nameMsg.AddData(DataPtr(name));
          SendMsg(acceptedSocket, nameMsg);

          for (IDCallback * cb : mOnJoin)
            cb->Execute(id);
        }
      }
    }

    void ServerRunning::HandleMessages()
    {
      const unsigned tcpBufSize = 100;
      char tcpBuffer[tcpBufSize];

      // using this type of loop because we may erase in the middle
      std::map<ID, Connection>::iterator it = mConnections.begin();
      while (it != mConnections.end())
      {
        std::pair<const ID, Connection> & curPair = *it;
        ID curId = curPair.first;
        Connection & curConection= curPair.second;
        SPSocket & curSocket = curConection.mSocket;
        MessageBeingReceived & inTransit = curConection.mInTransit;

        std::vector<SPIncomingMessage> incomingMessages;

        ReceiveMessages(
          curSocket, 
          incomingMessages, 
          inTransit, 
          tcpBuffer, 
          tcpBufSize);
        if (!curSocket)
        {
          incomingMessages.clear();

          ++it;
          HandleDisconnect(curId);
          continue;
        }

        for (SPIncomingMessage & inMsg : incomingMessages)
        {
          Message::Handle(mHandlerMap, inMsg);

          Message::Type msgtype = Message::fromString(inMsg->mName);
          if (msgtype == Message::Type::eReadyToStart)
          {
            for (auto callback : mOnReady)
              callback->Execute(curId);
          }
          else if (msgtype == Message::Type::eUDPInfo)
          {
            std::string UDPip   = inMsg->mData[0].GetString();
            mUDPips[curId] = UDPip;

            std::string UDPport = inMsg->mData[1].GetString();
            mUDPPorts[curId] = UDPport;
          }

        }
        ++it;
      }
    }

    // this removes from the vector, so it may invalidate an interator
    void ServerRunning::SendToAllBut(
      ID index, 
      const OutgoingMessage & msg, 
      std::string & errmsg,
      std::vector<ID> & invalidatedIds)
    {

      // using this type of loop because we may erase in the middle
      for (std::pair<const ID, Connection> & curPair : mConnections)
      {
        ID curId = curPair.first;
        if (curId == index)
          continue;

        Connection & curConection= curPair.second;
        SPSocket & curSocket = curConection.mSocket;

        SendMsg(curSocket, msg);
        if (!curSocket)
        {
          errmsg += GetWSAErrorString(WSAGetLastError());
          errmsg += "\n";
          invalidatedIds.push_back(curId);
        }
      }
    }

    ID ServerRunning::GetNextId()
    {
      if (!mFreeIds.empty())
      {
        ID id = mFreeIds.back();
        mFreeIds.pop_back();
        return id;
      }
      return mNextId++;
    }

    void ServerRunning::HandleDisconnect( ID disconnected )
    {
      // remove
      mConnections.erase(disconnected);
      mUDPPorts.erase(disconnected);
      mNames.erase(disconnected);

      mFreeIds.push_back(disconnected);

#ifdef TACTWEAK

      mTweakBar->AddComment("", 
        "Lost a connection, " 
        + std::to_string(mConnections.size()) 
        + " left");
#endif

      for (auto callback: mOnLeave)
        callback->Execute(disconnected);
    }

    void ServerRunning::SendToAll(const OutgoingMessage & msg)
    {
      // using this type of loop because we may erase in the middle
      for (std::pair<const ID, Connection> & curPair : mConnections)
      {
        ID curId = curPair.first;

        Connection & curConection= curPair.second;
        SPSocket & curSocket = curConection.mSocket;

        SendMsg(curSocket, msg);
        if (!curSocket)
          HandleDisconnect(curId);
      }
    }

    void ServerRunning::RemoveCallback( 
      std::vector<IDCallback*> & vec, 
      IDCallback * cb )
    {
      auto it = std::find(vec.begin(), vec.end(), cb);
      vec.erase(it);
    }

  } //  asteroids

} // Tac
