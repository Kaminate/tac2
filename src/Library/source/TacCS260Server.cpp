// Nathan Park
#include "TacCS260Server.h"
#include <thread>
#include <chrono>
namespace Tac
{
  bool ServerApplication::Init()
  {
    std::string filepath("resources/config.txt");
    std::ifstream ifs(filepath);
    if (!ifs.is_open())
    {
      std::cout << "Failed to open " << filepath << std::endl;
      return false;
    }
    std::string ip;
    std::string port;
    ifs >> ip;
    ifs >> port;

    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // ipv4
    hints.ai_socktype = SOCK_STREAM; // connection-oriented
    hints.ai_protocol = IPPROTO_TCP; // tcp
    hints.ai_flags = AI_PASSIVE; // use my ip

    // If the pNodeName parameter contains an empty string, 
    // all registered addresses on the local computer are returned.
    AddrInfo info(nullptr, port.c_str(), &hints);
    if (!info.Ok()){
      if (const char * error = info.GetSpecificError())
        std::cout << error << std::endl;
      else
        PrintWSAError("getaddrinfo failed");
      return false;
    }

    SPSocket tempSock(new Socket(info));

    if (!tempSock->Ok()){
      std::cout << "Socket failed";
      return false;
    }

    if (!tempSock->Bind(info)){
      std::cout << "Bind failed";
      return false;
    }

    if (!tempSock->Listen()){
      std::cout << "Listen failed";
      return false;
    }

    if (!tempSock->SetBlocking(false)){
      std::cout << "NonBlock fail";
      return false;
    }

    mListener = tempSock;
    std::cout << "Server online" << std::endl;
    return true;
  }

  void ServerApplication::Update( float dt )
  {
    AcceptConnections();
    HandleMessages();

  }

  ServerApplication::ServerApplication(AppSystem * sys)
    : MiniApp(sys)
    , mFreeClientIndex(0)
  {

  }

  void ServerApplication::AcceptConnections()
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
        mConnections[++mFreeClientIndex] = newConnection;
        std::cout 
          << "Added connection! Num connections: " 
          << mConnections.size()
          << std::endl;

        SendMsg(acceptedSocket, 
          Message::AssignId(mFreeClientIndex).ToMsg());
      }
    }
  }

  void ServerApplication::HandleMessages()
  {
    // using this type of loop because we may erase in the middle
    std::map<ClientIndex, Connection>::iterator it = mConnections.begin();
    while(it != mConnections.end())
    {
      std::pair<const ClientIndex, Connection> & curPair = *it;
      ClientIndex index = curPair.first;
      Connection & curConection= curPair.second;
      SPSocket & curSocket = curConection.mSocket;
      MessageBeingReceived & inTransit = curConection.mInTransit;

      std::vector<SPIncomingMessage> incomingMessages;

      ReceiveMessages(
        curSocket, 
        incomingMessages, 
        inTransit, 
        mBuffer, 
        sBufferSize);
      if (!curSocket)
      {
        incomingMessages.clear();

        // erase from connection
        it = mConnections.erase(it);

        RemoveClientChallenges(index);

        unsigned gameIndex = GetGame(index);
        if (gameIndex != mGames.size())
        {
          Game & curGame = mGames[gameIndex];
          unsigned playerIndex = curGame.mPlayers[0] == index? 0:1;
          unsigned opponentIndex = 1 - playerIndex;
          ClientIndex opponentClient = curGame.mPlayers[opponentIndex];

          // remove game
          mGames.erase(mGames.begin() + gameIndex);

          Connection & opponentConnection = mConnections[opponentClient];

          // msg opponent
          SendMsg(opponentConnection.mSocket, 
            Message::OpponentDisconnect().ToMsg());

          // check if opponent disconnected
          if (!opponentConnection.mSocket)
          {
            RemoveClientChallenges(opponentClient);
            auto opponentIt = mConnections.find(opponentClient);
            // should be valid
            mConnections.erase(opponentIt);
            // since this invalidates the map, just return.
            return;
          }
        }

        continue;
      }

      for (std::shared_ptr<IncomingMessage> & msg : incomingMessages)
      {
        switch (Message::GetType(msg->mName))
        {
        case Message::eAssignId:
          break;
        case Message::eChat:
          if (mConnections.size() == 1)
          {
            SendMsg(curSocket, 
              Message::Chat("SERVER", 
              "Your message echoes in silence...").ToMsg());
          }
          else
          {
            SendToAllBut(OutgoingMessage(*msg.get()), index);
          }
          break;
        case Message::eExit:
          break;
        case Message::eChallenge:
          {
            Message::Challenge outmsg(msg);

            bool opponentExists 
              = mConnections.find(outmsg.mOpponent) 
              != mConnections.end();

            // if the opponent is in a game, tell challenger that
            unsigned gameIndex = GetGame(outmsg.mOpponent);
            bool opponentInGame = gameIndex != mGames.size();

            if (!opponentExists)
            {
              SendMsg(curSocket, 
                Message::Chat("SERVER", 
                "Error: Opponent not found").ToMsg());
            }
            else if (bool opponentIsUs = index == outmsg.mOpponent)
            {
              SendMsg(curSocket, 
                Message::Chat("SERVER", 
                "Error: You cannot challenge yourself").ToMsg());
            }
            else if (opponentInGame)
            {
              SendMsg(curSocket, 
                Message::Chat("SERVER", 
                "Error: Opponent in game").ToMsg());
            }
            else 
            {

              // add the opponent to our set of challenges
              mWhoIsChallengingWho[index].insert(outmsg.mOpponent);

              bool opponentChallengingUs 
                = mWhoIsChallengingWho[outmsg.mOpponent].find(index) 
                != mWhoIsChallengingWho[outmsg.mOpponent].end();

              if (opponentChallengingUs)
              {
                // we are challenging them and they're challenging us,
                // so start the game
                Game newGame;
                newGame.mPlayers.push_back(index);
                newGame.mPlayers.push_back(outmsg.mOpponent);


                // temp
                /*
                Player p0;
                p0.mBoats.push_back(Boat(Boat::PatrolBoat,Vector2(0,0), true));
                Player p1;
                p1.mBoats.push_back(Boat(Boat::Destroyer,Vector2(5,5), false));
                newGame.mGame.mPlayers.push_back(p0);
                newGame.mGame.mPlayers.push_back(p1);
                */
                newGame.mGame.CreateSampleGame();

                mGames.push_back(newGame);

                // send message that the game is actually starting
                Message::GameStart startMsg(newGame);
                int numPlayers = newGame.mPlayers.size();
                int boatCount[2];
                for(unsigned i = 0; i < 2; ++i)
                  boatCount[i] = newGame.mGame.mPlayers[i].mBoats.size();
                SPSocket oppSocket 
                  = mConnections[outmsg.mOpponent].mSocket;
                SendMsg(curSocket, startMsg.ToMsg(&numPlayers, boatCount));
                SendMsg(oppSocket, startMsg.ToMsg(&numPlayers, boatCount));

                // for good measure
                mWhoIsChallengingWho[index].clear();
                mWhoIsChallengingWho[outmsg.mOpponent].clear();
              }
              else
              {
                std::string challengerID = std::to_string(index);
                std::string challengeMsg
                  ( "You have been challenged by player " 
                  + challengerID
                  + ". To accept, type \"Challenge " 
                  + challengerID 
                  + "\"");
                SendMsg(mConnections[outmsg.mOpponent].mSocket,
                  Message::Chat("Server", challengeMsg).ToMsg());
              }
            }
            outmsg.mOpponent;
          }
          break;
        case Message::eListUsers:
          {
            std::vector<std::string> userNames;
            std::vector<ClientIndex> userIndexes;
            for (auto it = mConnections.begin(); 
              it != mConnections.end(); ++it)
            {
              auto pair = *it;
              userNames.push_back("[todo: names]");
              userIndexes.push_back(pair.first);
            }
            Message::ListUsers outMsg(userNames, userIndexes);
            SendMsg(curSocket, outMsg.ToMsg());
          }
          break;
        case Message::eFire:
          {
            unsigned gameIndex = GetGame(index);
            bool inGame = gameIndex != mGames.size();
            // error if not in game
            if (!inGame)
            {
              std::cout 
                << "error: player " 
                << index 
                << "firing when not in game"
                << std::endl;
              break;
            }

            Game & playerGame = mGames[gameIndex];
            unsigned playerIndex = playerGame.mPlayers[0] == index ? 0 : 1;
            // error if not his turn
            if (playerIndex != playerGame.mGame.mCurrentPlayerTurn)
            {
              std::cout 
                << "error: player " 
                << index 
                << "firing when not his turn"
                << std::endl;
              break;
            }

            unsigned opponentIndex = 1 - playerIndex;
            ClientIndex opponentId = playerGame.mPlayers[opponentIndex];
            // send message to both players
            SendMsg(mConnections[index].mSocket, OutgoingMessage(*msg));
            SendMsg(mConnections[opponentId].mSocket, OutgoingMessage(*msg));
            // todo: handle disconnect
            // todo: update board, handle victory
            Message::Fire fireMsg(msg);
            Vector2 shot((float)fireMsg.mRow, (float)fireMsg.mCol);
            playerGame.mGame.Update(shot);
            if (playerGame.mGame.PlayerWin(playerIndex))
            {
              // that player won
              mGames.erase(mGames.begin() + gameIndex);
              std::cout 
                << index 
                << " has beaten " 
                << opponentId 
                << std::endl;;
            }
            break;
          }
        case Message::eCount:
        default:
          break;
        }
      }
      ++it;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  }

  void ServerApplication::SendToAllBut( 
    const OutgoingMessage & msg, ClientIndex dontSendToThisIndex )
  {
    for (auto it = mConnections.begin(); it != mConnections.end(); ++it)
    {
      auto pair = *it;
      ClientIndex index = pair.first;

      if (index == dontSendToThisIndex)
        continue;

      Connection & curConection= pair.second;
      SPSocket & curSocket = curConection.mSocket;

      SendMsg(curSocket, msg);
    }
  }

  void ServerApplication::RemoveClientChallenges( ClientIndex index )
  {
    mWhoIsChallengingWho.erase(index);

    // remove self from everybody's challenges
    for (std::pair<const ClientIndex, std::set<ClientIndex> > & pair 
      : mWhoIsChallengingWho)
    {
      pair.second.erase(index);
    }
  }

} // Tac