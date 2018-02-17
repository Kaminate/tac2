/*!
\author Nathan Park
\brief
*/

#ifndef __TAC_CS260_SERVER_H_
#define __TAC_CS260_SERVER_H_

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#include "TacSystemApplication.h"
#include "TacCS260Common.h"
#include "TacCS260Warboats.h"

namespace Tac
{
  
  class ServerApplication : public MiniApp
  {
  public:
    TacDLLAPI ServerApplication(AppSystem * sys);
    virtual bool Init();
    void AcceptConnections();
    void HandleMessages();
    virtual void Update(float dt);
  private:
    std::map<ClientIndex, Connection> mConnections;
    ClientIndex mFreeClientIndex;
    SPSocket mListener;
    static const int sBufferSize = 10;
    char mBuffer[sBufferSize];
    void SendToAllBut(const OutgoingMessage & msg, ClientIndex index);

    std::vector<Game> mGames;

    // todo: remove clients from here on disconnect
    std::map<ClientIndex, std::set<ClientIndex> > mWhoIsChallengingWho;

    // returns mGames.size() if not found
    unsigned GetGame(ClientIndex index)
    {
      for (unsigned i = 0; i < mGames.size(); ++i)
      {
        Game & iGame = mGames[i];
        auto & players = iGame.mPlayers;
        bool isPlayer = std::find(players.begin(), players.end(), index) 
          != players.end();
        if (isPlayer)
          return i;
      }
      return mGames.size();
    }

    void RemoveClientChallenges(ClientIndex index);
  };

} // Tac

#endif
