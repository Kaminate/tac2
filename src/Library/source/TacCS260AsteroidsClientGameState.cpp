/*!
\author Nathan Park
\brief
*/

#include "TacCS260AsteroidsClientGameState.h"
#include "TacCore.h"
#include <TacMessageUtil.h>
#include <TacSystemGraphics.h>
#pragma warning(disable : 4244) // int to ushort

namespace Tac
{
  namespace Asteroids
  {
    class OnIncomingChatCallback : public Message::Handler
    {
      ClientLobby * mLob;
    public:
      OnIncomingChatCallback(ClientLobby * lob) : mLob(lob){}
      virtual void Handle(SPIncomingMessage m) override
      {
#ifdef TACTWEAK

        std::string msg;
        msg += m->mData[0].GetString();
        msg += ": ";
        msg += m->mData[1].GetString();
        mLob->moutput->AddComment("", msg);
#else
        UnusedParameter(m);
#endif
      }
    };

    class OnGameStartCallback : public Message::Handler
    {
      ClientLobby * mLob;
    public:
      OnGameStartCallback(ClientLobby * lob) : mLob(lob){}
      virtual void Handle(SPIncomingMessage m) override
      {
        auto * sm = mLob->GetStateMachine();
        sm->SetNextState(SPClientGameState(new ClientGameplay(sm)));
      }
    };

    class OnGameEndCallback : public  Message::Handler
    {
      ClientGameplay * mG;
    public:
      OnGameEndCallback(ClientGameplay * g) : mG(g){}
      virtual void Handle(SPIncomingMessage m) override
      {
        bool playerVictory = m->mData[0].mBool;
        // switch states to win/lose screen
        StateMachine<ClientRunning> * sm = mG->GetStateMachine();
        sm->SetNextState(SPClientGameState(new ClientScoreScreen(
          sm, 
          playerVictory)));
      }
    };

    ClientLobby::ClientLobby(StateMachine<ClientRunning> * sm)
      : State<ClientRunning>(sm)
      , mAdder(Get()->mHandleMap)
    {
#ifdef TACTWEAK

      ClientRunning * running = Get();
      Client * asteroidsClient = running->Get();
      AppSystem * app = asteroidsClient->GetOwner();
      Core * engine = app->GetCore();

      unsigned chatWidth = 400;
      unsigned chatWindowH = 400;
      unsigned inputH = 50;
      unsigned x = 300;
      unsigned y = 100;

      Tweak & creator = engine->GetTweak();
      moutput = creator.Create("Chat Output");
      moutput->SetPosition(x, y);
      moutput->SetSize(chatWidth, chatWindowH);

      minput = creator.Create("Chat Input");
      minput->SetPosition(x, y + chatWindowH);
      minput->SetSize(chatWidth, inputH);
      minput->AddVar("", "label='Input'", TW_TYPE_STDSTRING, &minputStr);
      minput->AddButton("", "Chat", ClientLobby::OnChat, this);

      mLobbyBar = creator.Create("Lobby");
      mLobbyBar->AddButton("", 
        "Play network game", 
        ClientLobby::OnReady, 
        this);
      mLobbyBar->SetPosition(50, 350);
      mLobbyBar->SetSize(200, 200);
#endif

      using namespace Message;

      SPHandler inmsgHandler (new OnIncomingChatCallback(this));
      mHandlers.push_back(inmsgHandler);
      mAdder.Add(Message::Type::eChat, inmsgHandler.get());

      SPHandler startHandler (new OnGameStartCallback(this));
      mHandlers.push_back(startHandler);
      mAdder.Add(Message::Type::eGameStarted, startHandler.get());
    }

    void ClientLobby::Update( float dt )
    {
      UnusedParameter(dt);
    }
#ifdef TACTWEAK

    void TW_CALL ClientLobby::OnChat(void * data)
    {
      ClientLobby * lob = (ClientLobby*)data;
      ClientRunning * run = lob->Get();

      OutgoingMessage msg(tostring(Message::Type::eChat).c_str());
      msg.AddData(DataPtr(run->mName));
      msg.AddData(DataPtr(lob->minputStr));
      SendMsg(run->mServer.mSocket, msg);
    }

    void TW_CALL ClientLobby::OnReady( void * data )
    {
      ClientLobby * lob = (ClientLobby*)data;
      ClientRunning * run = lob->Get();

      OutgoingMessage msg(tostring(Message::Type::eReadyToStart).c_str());
      SendMsg(run->mServer.mSocket, msg);
    }
#endif

    void ClientGameplay::Update( float dt )
    {
      UnusedParameter(dt);
      RecvGamestate();
      RecvGamestate();
      SendInput();
    }

    ClientGameplay::ClientGameplay(StateMachine<ClientRunning>*sm )
      : State(sm)
      , mAdder(Get()->mHandleMap)
    {
      ClientRunning * running = Get();
      // listen for game over
      using namespace Message;
      SPHandler gameoverHandler(new OnGameEndCallback(this));
      mHandlers.push_back(gameoverHandler);
      mAdder.Add(Message::Type::eGameEnded, gameoverHandler.get());

#ifdef TACTWEAK

      Client * asteroidsClient = running->Get();
      AppSystem * app = asteroidsClient->GetOwner();
      Core * engine = app->GetCore();

      unsigned barW = 200;
      unsigned barH = 200;
      unsigned x = 400;
      unsigned y = 100;

      Tweak & creator = engine->GetTweak();
      mGameplayBar = creator.Create("Gamplay");
      mGameplayBar->SetPosition(x, y);
      mGameplayBar->SetSize(barW, barH);
#endif


      using namespace Message;

      std::ifstream ifs(gConfigFilepath);
      if (ifs.is_open())
      {
        ifs >> mServerIP;
        ifs >> mServerUDPPort;
        ifs.close();
        unsigned serverPortStr = atoi(mServerUDPPort.c_str());
        mUDPPort = std::to_string(serverPortStr + 1);
      }

      // create the udp socket

      addrinfo hints;
      ZeroMemory(&hints, sizeof(hints));
      hints.ai_family = AF_INET; // ipv4
      hints.ai_socktype = SOCK_DGRAM; 
      hints.ai_protocol = IPPROTO_UDP; 
      hints.ai_flags = AI_PASSIVE;

      std::shared_ptr<AddrInfo> errInfo;
      bool bindSucceeded = false;
      while(!bindSucceeded)
      {
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
          int port = atoi(mUDPPort.c_str());
          ++port;
          mUDPPort = std::to_string(port);
        }
        else
          bindSucceeded = true;
      }

      if (!mUDPSock->SetBlocking(false))
      {
        std::string errStr;
        errStr += "nonblock failed ";
        errStr += GetWSAErrorString(WSAGetLastError());
        throw Exception(errStr);
      }

      // get our ip
      if (hostent * localhost = gethostbyname(""))
      {
        if (char* localIP = inet_ntoa(*(in_addr*)*localhost->h_addr_list))
        {
          mUDPIpv4Addr = std::string(localIP);
        }
        else
        {
          std::string err;
          err += "inet_ntoa failed ";
          err += GetWSAErrorString(WSAGetLastError());
          throw Exception(err);
        }
      }
      else
      {
        std::string err;
        err += "gethostbyname failed ";
        err += GetWSAErrorString(WSAGetLastError());
        throw Exception(err);
      }

      // send a message to tell the server our udpport
      OutgoingMessage msg(tostring(Message::Type::eUDPInfo).c_str());
      msg.AddData(DataPtr(mUDPIpv4Addr));
      msg.AddData(DataPtr(mUDPPort));
      SendMsg(running->mServer.mSocket, msg);
    } // ClientGameplay()

    void ClientGameplay::RecvGamestate()
    {
      int iresult = recvfrom(
        mUDPSock->mSocket,
        gBuffer, 
        sizeof(ObjList), 
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
          break;
        case WSAECONNRESET:
          // remove from incoming
          // mSharedWithUs.erase()
          // too lazy to get their ip, lets just timeout
          break;
        default:
          {
            std::string err = GetWSAErrorString(lastError);
            Get()->Get()->Error(err);
            break;
          }
        }
      }
    }

    void ClientGameplay::SendInput()
    {
      Window & wind = Get()->Get()->GetOwner()->GetCore()->GetWindow();
      if (!wind.IsFocused())
        return;

      int serverport = atoi(mServerUDPPort.c_str());
      sockaddr_in RecvAddr;
      RecvAddr.sin_family = AF_INET;
      RecvAddr.sin_port = htons(serverport);
      RecvAddr.sin_addr.s_addr 
        = inet_addr(mServerIP.c_str());

      Inpput * inputMsg = reinterpret_cast<Inpput*>(gBuffer);
      inputMsg->idAsChar = (char) Get()->mId;
      unsigned & flags = inputMsg->mKeyFlags;
      flags = 0;
      if (GetAsyncKeyState(VK_DOWN) !=0) flags |= FlagDOWN;
      if (GetAsyncKeyState(VK_LEFT) !=0) flags |= FlagLEFT;
      if (GetAsyncKeyState(VK_RIGHT)!=0) flags |= FlagRIGHT;
      if (GetAsyncKeyState(VK_UP)   !=0) flags |= FlagUP;
      if (GetAsyncKeyState(VK_SPACE)!=0) flags |= FlagSPACE;
      int iresult = sendto(mUDPSock->mSocket,
        gBuffer,
        sizeof(Inpput),
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

    ClientScoreScreen::ClientScoreScreen( 
      StateMachine<ClientRunning>*sm ,
      bool playerVictory)
      : State(sm)
    {
#ifdef TACTWEAK

      ClientRunning * running = Get();
      Client * asteroidsClient = running->Get();
      AppSystem * app = asteroidsClient->GetOwner();
      Core * engine = app->GetCore();

      unsigned w,h,x,y;
      GetStateMachineTweakVars(w,h,x,y);

      Tweak & creator = engine->GetTweak();

      std::string barname;
      std::string comment;

      if (playerVictory)
      {
        barname = "Victory";
        comment = "You win";
      }
      else
      {
        barname = "Defeat";
        comment = "You got owned";
      }
      mScoreScreenBar= creator.Create(barname);
      mScoreScreenBar->AddComment("",comment);
      mScoreScreenBar->SetPosition(x, y);
      mScoreScreenBar->SetSize(w, h);
      mScoreScreenBar->AddButton("", "Play again", OnPlayAgain, this);
#else
      UnusedParameter(playerVictory);
#endif
    }

    void ClientScoreScreen::Update( float dt )
    {
      UnusedParameter(dt);
    }
#ifdef TACTWEAK

    void TW_CALL ClientScoreScreen::OnPlayAgain( void * data )
    {
      ClientScoreScreen * score = (ClientScoreScreen*)data;
      StateMachine<ClientRunning> * sm = score->GetStateMachine();
      sm->SetNextState(SPClientGameState(new ClientLobby(sm)));
    }
#endif

  } //asteroids



} // Tac
