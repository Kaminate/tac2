// Nathan Park
#include "TacCS260Client.h"
#include "TacCore.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include "TacMathUtil.h"
namespace Tac
{
#define UnassignedId -1

  ClientApplication::ClientApplication(bool randomName)
    // different so Login::OnEnter() is called
    : mState(DoNothing)
    , mNextState(Login)
    , mId(UnassignedId)
    // mCloseThread initialized in Lobby::OnEnter
  {
    if (randomName)
    {
      srand((unsigned)time(0));
      int num = rand() % 10000;
      char buf[10];
      _itoa_s(num, buf, 10);
      mUsername = std::string(buf);
    }
  }

  TacDLLAPI ClientApplication::~ClientApplication()
  {
    if (mBrowserInputThread.joinable())
    {
      mBrowserThreadShouldClose = true;
      mBrowserInputThread.detach();
      //mBrowserInputThread.join();
    }

    if (mGameInputThread.joinable())
    {
      mGameThreadShouldClose = true;
      mGameInputThread.detach();
      //mGameInputThread.join();
    }

  }

  void ClientApplication::HandleOnEnter()
  {
    if (mNextState != mState)
    {
      mState = mNextState;
      switch (mState)
      {
      case Tac::ClientApplication::Login:
        StateLoginOnEnter();
        break;
      case Tac::ClientApplication::ServerBrower:
        StateServerBrowserOnEnter();
        break;
      case Tac::ClientApplication::GameLobby:
        StateGameLobbyOnEnter();
        break;
      case Tac::ClientApplication::DoNothing:
        break;
      case Tac::ClientApplication::Count:
      default:
        ErrorIf(true, "shouldnt be here");
        break;
      }
    }
  }

  void ClientApplication::HandleUpdate()
  {
    switch (mState)
    {
    case Tac::ClientApplication::Login:
      StateLoginUpdate();
      break;
    case Tac::ClientApplication::ServerBrower:
      StateServerBrowserUpdate();
      break;
    case Tac::ClientApplication::GameLobby:
      StateGameLobbyUpdate();
      break;
    case Tac::ClientApplication::DoNothing:
      break;
    case Tac::ClientApplication::Count:
    default:
      ErrorIf(true, "shouldnt be here");
      break;
    }
  }

  void ClientApplication::Update( float dt )
  {
    HandleOnEnter();
    HandleUpdate();
  }

  TacDLLAPI bool ClientApplication::Init()
  {
    return true;
  }

  bool ClientApplication::IsOurTurn()
  {
    bool ourTurn 
      = mWarboats.mGame.mCurrentPlayerTurn == GetOurPlayerIndex();

    return ourTurn;
  }

  unsigned ClientApplication::GetOurPlayerIndex()
  {
    unsigned ourIndex = mWarboats.mPlayers[0] == mId ? 0 : 1;
    return ourIndex;
  }

  // Login

  void ClientApplication::StateLoginOnEnter()
  {
    bool promptToTryAgain = false;
    do
    {
      if (promptToTryAgain)
      {
        promptToTryAgain = false;
        if(!UserAnswer("Try again?"))
        {
          this->GetOwner()->GetCore()->StopRunning();
          mNextState = DoNothing;
          return;
        }
      }
      if (mUsername.empty())
      {

        std::cout << "Enter Username: ";
        std::getline(std::cin, mUsername);
        const unsigned maxCharLen = 15;
        const unsigned minCharLen = 3;
        if (mUsername == "mark" || mUsername == "Mark")
        {
          std::cout << "Fuck you mark" << std::endl;
          mUsername.clear();
          promptToTryAgain = true;
          continue;
        }
        else if (mUsername.length() > maxCharLen)
        {
          std::cout 
            << "Username too long (max " 
            << maxCharLen 
            << " chars)" 
            << std::endl;
          promptToTryAgain = true;
          continue;
        }
        else if (mUsername.length() < minCharLen)
        {
          std::cout 
            << "Username too short (min " 
            << minCharLen 
            << " chars)" 
            << std::endl;
          promptToTryAgain = true;
          continue;
        }
        else
        {
          std::cout << "Authentication successful. " << std::endl;
        }
      }


      // connect to the server
      std::string filepath("resources/config.txt");
      std::ifstream ifs(filepath);
      if (!ifs.is_open())
      {
        std::cout << "Failed to open " << filepath << std::endl;
        promptToTryAgain = true;
        continue;
      }
      std::string ip;
      std::string port;
      ifs >> ip;
      ifs >> port;

      addrinfo hints;
      ZeroMemory(&hints, sizeof(hints));
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;
      Tac::AddrInfo info(ip.c_str(), port.c_str(), &hints);
      if (!info.Ok())
      {
        std::cout << info.GetSpecificError() << std::endl;
        promptToTryAgain = true;
        continue;
      }

      std::shared_ptr<Socket> connectSocket(new Socket(info));

      if (!connectSocket->Ok())
      {
        std::cout << "Failed to create a socket" << std::endl;
        promptToTryAgain = true;
        continue;
      }

      bool connected = false;
      std::cout << "Connecting to server, please wait..." << std::endl;
      // loop through each address in the linked list
      for (addrinfo * node = info.GetInfo(); !connected && node; node = node->ai_next)
      {
        if (connect(
          connectSocket->mSocket, 
          node->ai_addr, 
          (int) node->ai_addrlen) == SOCKET_ERROR)
        {
          PrintWSAError("Connect failed");
        }
        else
        {
          connected = true;
          break;
        }
      }

      if (!connected)
      {
        std::cout << "Unable to connect to server!\n";
        promptToTryAgain = true;
        continue;
      }
      else
      {
        std::cout << "Connected to server" << std::endl;
      }

      if (!connectSocket->SetBlocking(false))
      {
        std::cout << "Unable to set socket to non-blocking mode!\n";
        promptToTryAgain = true;
        continue;
      }

      mConnection.mSocket = connectSocket;
    } while (promptToTryAgain);

    mNextState = ServerBrower;
  }

  void ClientApplication::StateLoginUpdate()
  {
  }

  // Server Browser

  void ClientApplication::StateServerBrowserOnEnter()
  {
    mBrowserThreadShouldClose = false;
    mBrowserInputThread 
      = std::thread(&ClientApplication::StateServerBrowserInputThread, this);
    std::cout << "Entering lobby" << std::endl;
    SendMsg(mConnection.mSocket, OutgoingMessage(
      Message::GetName(Message::eListUsers).c_str()));
  }

  void ClientApplication::StateServerBrowserUpdate()
  {
    if (!mUserMsgs.empty())
    {
      std::string msg;
      mBrowserInputMutex.lock();
      msg = mUserMsgs.back();
      mUserMsgs.pop_back();
      mBrowserInputMutex.unlock();

      std::stringstream ss;
      ss.str(msg);
      std::string action;
      ss >> action;
      action[0] = toupper(action[0]);

      switch (Message::Type t = Message::GetType(action))
      {
      case Message::eChat:
        {
          // remove action from msg
          msg.erase(msg.find(action), action.length());
          Message::Chat chatMsg(mUsername, msg);
          SendMsg(mConnection.mSocket, chatMsg.ToMsg());
          break;
        }
      case Message::eListUsers:
        {
          SendMsg(mConnection.mSocket, OutgoingMessage(action.c_str()));
          break;
        }
      case Message::eExit:
        {
          GetOwner()->GetCore()->StopRunning();
          mNextState = DoNothing;
          // todo: use an actual state machine so 
          // i dont have to keep typing this
          mBrowserThreadShouldClose = true;
          mBrowserInputThread.join();
          return;
        }
      case Message::eChallenge:
        {
          if (mId == UnassignedId)
          {
            std::cout 
              << "Still awaiting id, cannot yet challenge players"
              << std::endl;
          }
          else
          {
            ClientIndex index;
            ss >> index;
            SendMsg(mConnection.mSocket, Message::Challenge(index).ToMsg());
          }
        }
        break;
      default:
        std::cout << "Invalid action. Available actions:" << std::endl;
        std::vector<Message::Type> availableActions;
        availableActions.push_back(Message::eChat);
        availableActions.push_back(Message::eListUsers);
        availableActions.push_back(Message::eChallenge);
        availableActions.push_back(Message::eExit);
        for (Message::Type msgType : availableActions)
          std::cout << Message::GetName(msgType) << std::endl;
        break;
      }
    }

    if (!mConnection.mSocket)
    {
      GetOwner()->GetCore()->StopRunning();
      std::cout << "Disonnected from server" << std::endl;;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      mNextState = DoNothing;
      // todo: use an actual state machine so 
      // i dont have to keep typing this
      mBrowserThreadShouldClose = true;
      mBrowserInputThread.join();
      return;
    }

    const unsigned bufSize = 256;
    char buffer[bufSize];
    std::vector<SPIncomingMessage> received;
    ReceiveMessages(
      mConnection.mSocket, 
      received, 
      mConnection.mInTransit, 
      buffer, 
      bufSize);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // todo: have a better way of handling disconnect 
    // after a send or receive
    if (!mConnection.mSocket)
    {
      GetOwner()->GetCore()->StopRunning();
      std::cout << "Disonnected from server" << std::endl;;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      mNextState = DoNothing;
      // todo: use an actual state machine so 
      // i dont have to keep typing this
      mBrowserThreadShouldClose = true;
      mBrowserInputThread.join();
      return;
    }



    for (SPIncomingMessage msg : received)
    {
      Message::Type msgType = Message::GetType(msg->mName);
      switch (msgType)
      {
      case Tac::Message::eChat:
        {
          Message::Chat chatMsg(msg);
          chatMsg.Print();
        }
        break;
      case Tac::Message::eAssignId:
        {
          Message::AssignId inMsg(msg);
          mId = inMsg.mIndex;
          std::cout << "Your id is: " << mId << std::endl;
        }
        break;
      case Tac::Message::eExit:
        break;
      case Tac::Message::eChallenge:
        break;
      case Tac::Message::eGameStart:
        {
          std::cout << "Found an opponent!" << std::endl;
          Message::GameStart inMsg(msg);
          mWarboats = inMsg.mWarboats;
          mNextState = GameLobby;
          mBrowserThreadShouldClose = true;
          mBrowserInputThread.join();
          //mBrowserInputThread.detach();
          break;
        }
      case Tac::Message::eListUsers:
        {
          Message::ListUsers inMsg(msg);
          inMsg.ListOnlineUsers();
          break;
        }
      case Tac::Message::eCount:
        break;
      default:
        break;
      }
    }

  }

  void ClientApplication::StateServerBrowserInputThread()
  {
    while(!mBrowserThreadShouldClose)
    {
      std::string msg;
      std::getline(std::cin, msg);
      mBrowserInputMutex.lock();
      mUserMsgs.push_back(msg);
      mBrowserInputMutex.unlock();
    }
  }

  // Game Lobby

  void ClientApplication::StateGameLobbyOnEnter()
  {
    unsigned ourPlayerIndex = GetOurPlayerIndex();
    std::cout 
      << "Started game against"
      << mWarboats.mPlayers[1 - ourPlayerIndex]
    << std::endl;

    /*
    // temp, (duplicated in servercode)
    Player p0;
    p0.mBoats.push_back(Boat(Boat::PatrolBoat,Vector2(0,0), true));
    Player p1;
    p1.mBoats.push_back(Boat(Boat::Destroyer,Vector2(5,5), false));
    mWarboats.mGame.mPlayers.push_back(p0);
    mWarboats.mGame.mPlayers.push_back(p1);
    */

    mWarboats.mGame.RenderForPlayer(ourPlayerIndex);
    std::cout << std::endl;
    std::cout << "Enemy Boats:" << std::endl;
    mWarboats.mGame.ListBoatsOfPlayer(1 - ourPlayerIndex);
    std::cout << std::endl;
    std::cout << "Your boats:" << std::endl;
    mWarboats.mGame.ListBoatsOfPlayer(ourPlayerIndex);
    std::cout << std::endl;
    PrintWhoseTurnItIs();

    mGameThreadShouldClose = false;
    mGameInputThread 
      = std::thread(&ClientApplication::StateGameLobbyInputThread, this);
  }

  void ClientApplication::StateGameLobbyUpdate()
  {
    if (!mUserMsgs.empty())
    {
      std::string msg;
      mGameInputMutex.lock();
      msg = mUserMsgs.back();
      mUserMsgs.pop_back();
      mGameInputMutex.unlock();

      std::stringstream ss;
      ss.str(msg);
      std::string action;
      ss >> action;
      action[0] = toupper(action[0]);

      switch (Message::Type t = Message::GetType(action))
      {
      case Message::eExit:
        {
          GetOwner()->GetCore()->StopRunning();
          mNextState = DoNothing;
          return;
        }
      case Message::eFire:
        {
          if (IsOurTurn())
          {
            int row, col;
            ss >> row;
            ss >> col;
            Clamp(row, 0, (int)mWarboats.mGame.mHeight - 1);
            Clamp(col, 0, (int)mWarboats.mGame.mWidth - 1);
            SendMsg(mConnection.mSocket, Message::Fire(row, col).ToMsg());
          }
          else
          {
            std::cout << "It's not your turn!" << std::endl;
          }
          break;
        }
      default:
        std::cout << "Invalid action. Available actions:" << std::endl;
        std::vector<Message::Type> availableActions;
        availableActions.push_back(Message::eExit);
        // todo: only if its your turn
        availableActions.push_back(Message::eFire);
        for (Message::Type msgType : availableActions)
          std::cout << Message::GetName(msgType) << std::endl;
        break;
      }
    }

    // todo: have a better way of handling disconnect 
    // after a send or receive
    if (!mConnection.mSocket)
    {
      std::cout << "Disconnected from server" << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      GetOwner()->GetCore()->StopRunning();
      mNextState = DoNothing;
      // todo: use an actual state machine so 
      // i dont have to keep typing this
      mGameThreadShouldClose = true;
      mGameInputThread.join();
      return;
    }


    const unsigned bufSize = 256;
    char buffer[bufSize];
    std::vector<SPIncomingMessage> received;
    ReceiveMessages(
      mConnection.mSocket, 
      received, 
      mConnection.mInTransit, 
      buffer, 
      bufSize);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // todo: have a better way of handling disconnect 
    // after a send or receive
    if (!mConnection.mSocket)
    {
      std::cout << "Disconnected from server" << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      GetOwner()->GetCore()->StopRunning();
      mNextState = DoNothing;
      // todo: use an actual state machine so 
      // i dont have to keep typing this
      mGameThreadShouldClose = true;
      mGameInputThread.join();
      return;
    }


    for (SPIncomingMessage msg : received)
    {
      Message::Type msgType = Message::GetType(msg->mName);
      switch (msgType)
      {
      case Tac::Message::eCount:
        break;
      case Tac::Message::eFire:
        {
          Message::Fire inMsg(msg);
          Vector2 shot((float)inMsg.mRow, (float)inMsg.mCol);
          mWarboats.mGame.Update(shot);
          mWarboats.mGame.RenderForPlayer(GetOurPlayerIndex());
          std::cout << std::endl;
          std::cout << "Enemy Boats:" << std::endl;
          mWarboats.mGame.ListBoatsOfPlayer(1 - GetOurPlayerIndex());
          std::cout << std::endl;
          std::cout << "Your boats:" << std::endl;
          mWarboats.mGame.ListBoatsOfPlayer(GetOurPlayerIndex());
          std::cout << std::endl;

          bool gameOver = false;

          if (mWarboats.mGame.PlayerWin(GetOurPlayerIndex()))
          {
            std::cout << "You win!" << std::endl;
            gameOver = true;

          }
          else if (mWarboats.mGame.PlayerWin(1 - GetOurPlayerIndex()))
          {
            std::cout << "You lose." << std::endl;
            gameOver = true;
          }

          if (gameOver)
          {
            mNextState = ServerBrower;
            mGameThreadShouldClose = true;
            mGameInputThread.join();
          }
          else
          {
            PrintWhoseTurnItIs();
          }

          break;
        }
      case Tac::Message::eOpponentDisconnect:
        {
          Message::OpponentDisconnect inMsg(msg);
          std::cout 
            << "Opponent disconnected. You win, I guess."
            << std::endl;
          mNextState = ServerBrower;
          mGameThreadShouldClose = true;
          mGameInputThread.join();
          break;
        }
      default:
        break;
      }
    }
  }

  void ClientApplication::StateGameLobbyInputThread()
  {
    while(!mGameThreadShouldClose)
    {
      std::string msg;
      std::getline(std::cin, msg);
      mGameInputMutex.lock();
      mUserMsgs.push_back(msg);
      mGameInputMutex.unlock();
    }
  }

  void ClientApplication::PrintWhoseTurnItIs()
  {
    if (IsOurTurn())
      std::cout << "It is YOUR turn" << std::endl;
    else
      std::cout << "It is YOUR OPPONENT's turn" << std::endl;
  }

} // Tac
