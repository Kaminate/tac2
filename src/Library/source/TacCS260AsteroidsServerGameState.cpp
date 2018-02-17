/*!
\author Nathan Park
\brief
*/

#include "TacCS260AsteroidsServerGameState.h"
#include "TacCore.h"
#include <algorithm>
#include "TacSystemGraphics.h"
#pragma warning(disable : 4244) // int to ushort

namespace Tac
{
  namespace Asteroids
  {

    Lobby::Lobby( StateMachine<ServerRunning>*sm )
      : State<ServerRunning>(sm)
      , mReady(this)
      , mLeave(this)
      //, mAdder(Get()->mHandlerMap)
    {
      ServerRunning * run = Get();
      run->mOnReady.push_back(&mReady);
      run->mOnLeave.push_back(&mLeave);
#ifdef TACTWEAK

      Server * serv = run->Get();
      AppSystem * app = serv->GetOwner();
      Tweak & tw = app->GetCore()->GetTweak();

      mLobbyBar = tw.Create("Lobby");
      mLobbyBar->SetPosition(50, 350);
      mLobbyBar->SetSize(200, 200);
#endif
    }

    Lobby::~Lobby()
    {
      ServerRunning * run = Get();
      run->RemoveCallback(run->mOnReady, &mReady);
      run->RemoveCallback(run->mOnLeave, &mLeave);
    }

    void Lobby::UpdateReadyText()
    {
#ifdef TACTWEAK

      unsigned readyCount = mReadyID.size();
      unsigned userCount = Get()->mConnections.size();

      mLobbyBar->AddComment
        ( ""
        , std::to_string(readyCount) 
        + " / "
        + std::to_string(userCount) 
        + " ready");
#endif
    }

    Lobby::OnLeaveCallback::OnLeaveCallback( Lobby * l ) : mL(l)
    {

    }

    void Lobby::OnLeaveCallback::Execute( ID user )
    {
      mL->mReadyID.erase(user);
      mL->UpdateReadyText();
    }

    Lobby::OnReadyCallback::OnReadyCallback( Lobby * l ) : mL(l)
    {

    }

    void Lobby::OnReadyCallback::Execute( ID user )
    {
      mL->mReadyID.insert(user);
      mL->UpdateReadyText();

      unsigned playersNeededToStart = 1;
      if (mL->mReadyID.size() == playersNeededToStart)
      {
        // start the game
        StateMachine<ServerRunning> * sm = mL->GetStateMachine();
        sm->SetNextState(SPServerGameState(new Gameplay(sm)));;
        mL->mReadyID.clear();
      }
    }

    // gameplay
    class ServerOnObjectDestroy : public OnDestroy
    {
    public:
      void Execute(Object * obj)override
      {
        UnusedParameter(obj);
        // do nothing
      }
    };
    class ServerOnGameOver : public Callback
    {
      Gameplay* mS;
    public:
      ServerOnGameOver(Gameplay* s) : mS(s){}
      virtual void Execute() override
      {
        mS->OnGameOver();
      }
    };

    Gameplay::Gameplay( StateMachine<ServerRunning>*sm )
      : State(sm)
      , mOnJoin(this)
      , mOnLeave(this)
      , mSendWaitTime(0.0f)
      , mTimeSinceLastSentGamestate(0)
      , mAsteroids(
      SPOnDestroy(new ServerOnObjectDestroy()),
      SPCallback(new ServerOnGameOver(this)))
    {
      ServerRunning * run = Get();
      run->mOnJoin.push_back(&mOnJoin);
      run->mOnLeave.push_back(&mOnLeave);

      for (auto pair : run->mConnections)
      {
        ID clientId = pair.first;
        mOnJoin.Execute(clientId);
      }

#ifdef TACTWEAK
      Server * serv = run->Get();

      AppSystem * app = serv->GetOwner();
      Core * engine = app->GetCore();
      Tweak & tw = engine->GetTweak();

      mGameplayBar = tw.Create("Gameplay");
      mGameplayBar->SetPosition(50, 300);
      mGameplayBar->AddVar("",
        "label='send gamestate waitime' "
        "step=0.01666", 
        TW_TYPE_FLOAT, 
        &mSendWaitTime);
#endif

      std::ifstream ifs(gConfigFilepath);
      if (ifs.is_open())
      {
        std::string serverip;
        std::string serverport;
        ifs >> serverip;
        ifs >> serverport;
        ifs.close();

        mUDPPort = serverport;
      }

      // create the udp socket

      addrinfo hints;
      ZeroMemory(&hints, sizeof(hints));
      hints.ai_family = AF_INET; // ipv4
      hints.ai_socktype = SOCK_DGRAM; 
      hints.ai_protocol = IPPROTO_UDP; 
      hints.ai_flags = AI_PASSIVE;

      std::shared_ptr<AddrInfo> errInfo;
      // If the pNodeName parameter contains an empty string, 
      // all registered addresses on the local computer are returned.
      errInfo = std::shared_ptr<AddrInfo>(new AddrInfo(
        nullptr,
        mUDPPort.c_str(), 
        &hints));

      if (!errInfo->Ok())
        throw Exception(errInfo->GetSpecificError());

      mUDPSock = SPSocket(new Socket(*errInfo));

      if (!mUDPSock->Ok())
      {
        std::string errStr;
        errStr += "socket failed ";
        errStr += GetWSAErrorString(WSAGetLastError());
        throw(Exception(errStr));
      }

      if (!mUDPSock->Bind(*errInfo))
      {
        throw(Exception("bind failed"));
      }

      if (!mUDPSock->SetBlocking(false))
      {
        std::string errStr;
        errStr += "nonblock failed ";
        errStr += GetWSAErrorString(WSAGetLastError());
        throw Exception(errStr);
      }
    }

    Gameplay::~Gameplay()
    {
      ServerRunning * run = Get();
      run->RemoveCallback(run->mOnJoin, &mOnJoin);
      run->RemoveCallback(run->mOnLeave, &mOnLeave);
    }

    void Gameplay::Update( float dt )
    {
      mAsteroids.Update(dt);

      RecvInput();

      mTimeSinceLastSentGamestate += dt;
      if (mTimeSinceLastSentGamestate >= mSendWaitTime)
      {
        SendGamestate();
        mTimeSinceLastSentGamestate = 0;
      }
    }

    void Gameplay::SendGamestate()
    {
      ServerRunning * serv = Get();
      // send a message to all clients
      for (std::pair<const ID, std::string> & pair : serv->mUDPips)
      {
        ID curID = pair.first;
        const std::string & curIP = pair.second;
        const std::string & curPort = serv->mUDPPorts[curID];
        if (curIP.empty() || curPort.empty())
          continue;
        int port = atoi(curPort.c_str());
        sockaddr_in RecvAddr;
        RecvAddr.sin_family = AF_INET;
        RecvAddr.sin_port = (u_short)htons(port);
        RecvAddr.sin_addr.s_addr 
          = inet_addr(curIP.c_str());

        int iresult = sendto(
          mUDPSock->mSocket,
          gBuffer,
          sizeof(ObjList),
          0,
          (sockaddr *) &RecvAddr, 
          sizeof(RecvAddr));

        if (iresult == SOCKET_ERROR)
        {
          int err = WSAGetLastError();
          switch (err)
          {
          case WSAEWOULDBLOCK:
            // ignore
            break;
          case WSAECONNRESET:
            Get()->Get()->Error(GetWSAErrorString(err));
            break;
          default:
            Get()->Get()->Error(GetWSAErrorString(err));
            break;
          }
        }
      }
    }

    void Gameplay::RecvInput()
    {
      // +1 for good measure?
      for (unsigned i = 0; i < Get()->mUDPips.size() + 1; ++i)
      {
        int iresult = recvfrom(
          mUDPSock->mSocket,
          gBuffer,
          sizeof(Inpput),
          0,
          nullptr,
          nullptr);

        if (iresult == SOCKET_ERROR)
        {
          int lastError = WSAGetLastError();
          switch (lastError)
          {
          case WSAEWOULDBLOCK:
            // no data to recv
            return;
          case WSAECONNRESET:
            // just let tcp handle d/c?
            break;
          default:
            {
              std::string err = GetWSAErrorString(lastError);
              Get()->Get()->Error(err);
              break;
            }
          }
        }
        else
        {
          Inpput * input = reinterpret_cast<Inpput*>(gBuffer);
          ID clientID = (ID) input->idAsChar;
          Object * playerShip = mAsteroids.GetShip(clientID);
          if (playerShip)
          {
            float accel = 1;
            if (input->mKeyFlags & FlagUP)
            {
              playerShip->mVelY += accel;
            }
            if (input->mKeyFlags & FlagDOWN)
            {
              playerShip->mVelY -= accel;
            }
            if (input->mKeyFlags & FlagRIGHT)
            {
              playerShip->mVelX += accel;
            }
            if (input->mKeyFlags & FlagLEFT)
            {
              playerShip->mVelX -= accel;
            }
            if (input->mKeyFlags & FlagSPACE)
            {
              mAsteroids.AttemptFire(clientID);
            }
          }
        }
      }
    }

    void Gameplay::OnGameOver()
    {
      // todo:
      // - switch states back to lobby
      // - send a gameover message to the clients
      bool playerVictory = mAsteroids.GetNum(Object::Type::eShip) != 0;

      // send message to tell them the game is over
      OutgoingMessage msg(tostring(Message::Type::eGameEnded).c_str());
      msg.AddData(DataPtr(&playerVictory));


      ServerRunning * serv = Get();
      for (std::pair<const ID, Connection> & pair : serv->mConnections)
      {
        Connection & curConnection = pair.second;

        SendMsg(curConnection.mSocket, msg);
      }

      // switch state back to lobby
      StateMachine<ServerRunning> * sm = GetStateMachine();
      sm->SetNextState(SPServerGameState(new Lobby(sm)));
    }

    Gameplay::OnJoinCallback::OnJoinCallback( Gameplay * g ) : mG(g)
    {

    }

    void Gameplay::OnJoinCallback::Execute( ID user )
    {
      // create the ship
      mG->mAsteroids.AddPlayer(user);

      // send message to tell them the game has started
      Connection & curConnection = mG->Get()->mConnections[user];
      OutgoingMessage msg(tostring(Message::Type::eGameStarted).c_str());
      SendMsg(curConnection.mSocket, msg);
    }

    Gameplay::OnLeaveCallback::OnLeaveCallback( Gameplay * g ) : mG(g)
    {
    }

    void Gameplay::OnLeaveCallback::Execute( ID user )
    {
      mG->mAsteroids.RemPlayer(user);
    }

  } //asteroids

} // Tac
