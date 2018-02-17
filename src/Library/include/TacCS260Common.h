/*!
\author Nathan Park
\brief
*/

#ifndef __TAC_CS260_COMMON_H_
#define __TAC_CS260_COMMON_H_

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#include "TacMessage.h"
#include "TacMessageUtil.h"
#include "TacNetwork.h"
#include "TacErrorIf.h"
#include "TacCS260Warboats.h"
#include <string>
#include <vector>

namespace Tac
{
  typedef int ClientIndex;

  struct Game
  {
    Game(unsigned width = 10, unsigned height = 10) 
      : mGame(width, height){}

    // index in vector corresponds to index in game
    std::vector<ClientIndex> mPlayers;

    //std::vector<ClientIndex> mObservers;
    Warboats mGame;
  };


  void GetInput(std::string & input);

  void Say(const std::string & msg);

  bool UserAnswer(const std::string & question);

  namespace Message
  {
    enum Type
    {
      eChat,
      eAssignId,
      eListUsers,
      eChallenge,
      eGameStart,
      eOpponentDisconnect,
      eFire,
      eExit,
      eCount,
    };
    std::string GetName(Type messageType);

    // returns Type::Count if invalid
    Type GetType(const std::string & message);

    struct Fire
    {
      Fire(const SPIncomingMessage & msg)
      {
        mRow = msg->mData[0].mInt;
        mCol = msg->mData[1].mInt;
      }
      Fire(int row, int col) : mRow(row), mCol(col)
      {
      }
      OutgoingMessage ToMsg()
      {
        std::string msgType = GetName(Message::Type::eFire);
        OutgoingMessage out(msgType.c_str());
        out.AddData(DataPtr(Data::Type::eInt, &mRow, sizeof(int)));
        out.AddData(DataPtr(Data::Type::eInt, &mCol, sizeof(int)));
        return out;
      }
      int mRow;
      int mCol;
    };

    struct OpponentDisconnect
    {
      OpponentDisconnect(const SPIncomingMessage & msg)
      {
      }
      OpponentDisconnect()
      {
      }
      OutgoingMessage ToMsg()
      {
        std::string msgType = GetName(Message::Type::eOpponentDisconnect);
        OutgoingMessage out(msgType.c_str());
        return out;
      }
    };

    struct AssignId
    {
      AssignId(const SPIncomingMessage & msg)
      {
        mIndex = msg->mData[0].mInt;
      }
      AssignId(ClientIndex index) : mIndex(index)
      {
      }
      OutgoingMessage ToMsg()
      {
        std::string msgType = GetName(Message::Type::eAssignId);
        OutgoingMessage out(msgType.c_str());
        out.AddData(DataPtr(Data::Type::eInt, &mIndex, sizeof(int)));
        return out;
      }
      ClientIndex mIndex;
    };

    // sent from the server, contains game
    struct GameStart
    {
      // todo:
      GameStart(const SPIncomingMessage & msg)
      {
        /*
        int numPlayers = msg->mData[0].mInt;
        for (int i = 0; i < numPlayers; ++i)
        {
          ClientIndex player = msg->mData[i + 1].mInt;
          mWarboats.mPlayers.push_back(player);
        }
        */

        int index = -1;
        mWarboats.mGame.mWidth = msg->mData[++index].mInt;
        mWarboats.mGame.mHeight = msg->mData[++index].mInt;
        mWarboats.mGame.mCurrentPlayerTurn = msg->mData[++index].mInt;
        int numPlayers = msg->mData[++index].mInt;
        for (int i = 0; i < numPlayers; ++i)
        {
          ClientIndex curIndex = msg->mData[++index].mInt;
          mWarboats.mPlayers.push_back(curIndex);
          int numBoats = msg->mData[++index].mInt;
          Player p;
          for (int i = 0; i < numBoats; ++i)
          {
            bool horiz = msg->mData[++index].mBool;
            int len = msg->mData[++index].mInt;

            std::string name(msg->mData[++index].GetString());
            float x = msg->mData[++index].mFloat;
            float y = msg->mData[++index].mFloat;
            Boat warBoat(name, Vector2(x,y), len, horiz);
            p.mBoats.push_back(warBoat);
          }
          mWarboats.mGame.mPlayers.push_back(p);
        }
        // end
      }
      GameStart(const Game & game) : mWarboats(game)
      {

      }
      OutgoingMessage ToMsg(
        int * numPlayers, 
        int * numBoats) // array of 2
      {
        std::string msgType = GetName(Message::Type::eGameStart);
        OutgoingMessage out(msgType.c_str());
        
        // width
        out.AddData(DataPtr(Data::Type::eInt, 
          &mWarboats.mGame.mWidth, sizeof(int)));
        // Height
        out.AddData(DataPtr(Data::Type::eInt, 
          &mWarboats.mGame.mHeight, sizeof(int)));
        // current player turn
        out.AddData(DataPtr(Data::Type::eInt, 
          &mWarboats.mGame.mCurrentPlayerTurn, sizeof(int)));

        // num players
        out.AddData(DataPtr(Data::Type::eInt, numPlayers, sizeof(int)));
        for (int i = 0; i < *numPlayers; ++i)
        {
          // each player's clientIndex
          out.AddData(DataPtr(Data::Type::eInt, &mWarboats.mPlayers[i], 
            sizeof(int)));

          Player & curPlayer = mWarboats.mGame.mPlayers[i];
          out.AddData(DataPtr(Data::Type::eInt, numBoats + i,sizeof(int)));
          for (Boat & iBoat : curPlayer.mBoats)
          {
            out.AddData(DataPtr(Data::Type::eBool, 
              &iBoat.mHorizontal, sizeof(bool)));
            out.AddData(DataPtr(Data::Type::eInt, 
              &iBoat.mLength, sizeof(int)));
            out.AddData(DataPtr(Data::Type::eCString, 
              iBoat.mName.c_str(), iBoat.mName.size()));
            out.AddData(DataPtr(Data::Type::eFloat, 
              &iBoat.mTopLeftPosition.x, sizeof(float)));
            out.AddData(DataPtr(Data::Type::eFloat, 
              &iBoat.mTopLeftPosition.y, sizeof(float)));
          }
        }
        return out;
      }
      Game mWarboats;
    };

    struct Challenge
    {
      Challenge(const SPIncomingMessage & msg)
      {
        std::string msgType(msg->mName);
        std::string chatType = Message::GetName(eChallenge);
        ErrorIf(msgType.compare(chatType) != 0, "Message type mismatch");
        mOpponent = msg->mData[0].mInt;
      }

      Challenge(ClientIndex opponent) : mOpponent(opponent)
      {
      }

      OutgoingMessage ToMsg()
      {
        std::string msgType = GetName(Message::Type::eChallenge);
        OutgoingMessage out(msgType.c_str());
        out.AddData(DataPtr(Data::Type::eInt, &mOpponent, sizeof(int)));
        return out;
      }
      ClientIndex mOpponent;
    };

    struct Chat
    {
      Chat(const SPIncomingMessage & msg)
      {
        std::string msgType(msg->mName);
        std::string chatType = Message::GetName(eChat);
        ErrorIf(msgType.compare(chatType) != 0, "Message type mismatch");

        mUsername = msg->mData[0].GetString();
        mMsg = msg->mData[1].GetString();
      }

      Chat(const std::string & username, const std::string & msg)
        : mUsername(username)
        , mMsg(msg)
      {
      }

      OutgoingMessage ToMsg()
      {
        std::string msgType = GetName(Message::Type::eChat);
        OutgoingMessage out(msgType.c_str());
        out.AddData(DataPtr(
          Data::Type::eCString, mUsername.c_str(), mUsername.length()));
        out.AddData(DataPtr(
          Data::Type::eCString, mMsg.c_str(), mMsg.length()));
        return out;
      }

      void Print()
      {
        std::cout << mUsername << ": " << mMsg << std::endl;
      }

      std::string mUsername;
      std::string mMsg;
    };

    struct ListUsers
    {
      ListUsers(const SPIncomingMessage & msg)
      {
        bool readName = true;
        for (const Data & msgData : msg->mData)
        {
          if (readName)
          {
            mUserNames.push_back(msgData.GetString());
          }
          else
          {
            mUserIndexes.push_back(msgData.mInt);
          }
          readName = !readName;
        }
      }
      ListUsers(
        const std::vector<std::string> & userNames,
        const std::vector<ClientIndex> & userIndexes)
        : mUserNames(userNames)
        , mUserIndexes(userIndexes)
      {
        ErrorIf(userNames.size() != userIndexes.size(), "wtf");
      }
      OutgoingMessage ToMsg()
      {
        std::string msgType = GetName(Message::Type::eListUsers);
        OutgoingMessage out(msgType.c_str());
        for (unsigned i = 0; i < mUserNames.size(); ++i)
        {
          out.AddData(DataPtr(
            Data::Type::eCString, 
            mUserNames[i].c_str(), 
            mUserNames[i].length()));
          out.AddData(DataPtr(
            Data::Type::eInt, 
            &mUserIndexes[i],
            sizeof(int)));
        }
        return out;
      }
      void ListOnlineUsers()
      {
        std::cout << mUserNames.size() << " players online" << std::endl;

        for (unsigned i = 0; i < mUserNames.size(); ++i)
        {
          std::cout 
            << mUserNames[i] << " (id: " 
            << mUserIndexes[i] << ")" << std::endl;
        }
      }
      std::vector<std::string> mUserNames;
      std::vector<ClientIndex> mUserIndexes;
    };
  }


} // Tac

#endif
