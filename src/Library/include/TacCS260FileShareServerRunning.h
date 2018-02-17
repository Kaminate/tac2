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
#include "TacCS260FileShareServer.h"
#include "TacMessageUtil.h"
#include "TacCS260FileShareCommon.h"

namespace Tac
{
  class FileShareServerRunning : public State<FileShareServer>
  {
  public:
    FileShareServerRunning(
      StateMachine<FileShareServer> * sm,
      SPSocket listener);
    virtual void Init();
    virtual void Update(float dt);
    void AcceptConnections();
    void HandleMessages();

    void HandleDisconnect(const std::vector<FileShareID> & disconnected);
    void HandleDisconnect(FileShareID disconnected);

    void UpdateClientBufSizes()
    {
      std::vector<FileShareID> disconnectedIds;
      std::string sendtoallErrs;
      FileShareMsg::StoCUpdateBufSize msg (mClientBufSize);
      SendToAll(
        msg.ToMsg(),
        sendtoallErrs, 
        disconnectedIds);
      HandleDisconnect(disconnectedIds);
    }

  private:
    // todo: add to this when clients disconnect
    std::vector<FileShareID> mFreeIds;
    FileShareID mNextId;
    FileShareID GetNextId();

    /*
    struct ClientData
    {
      ClientData(SPSocket sock)
      {
      }
      ~ClientData()
      {
        mOnDeath.Execute(this);
      }
      Connection mConnection;
      std::set<File> mShareMap;
      std::string mUDP;
      std::string mIP;
      bool mRecving;
      bool mSending;
    };
    std::map<FileShareID, ClientData> mCLientData;
    */

    std::map<FileShareID, Connection> mConnections;
    std::map<FileShareID, std::set<File> > mShareMap;
    std::map<FileShareID, std::string> mUDPPorts;
    std::map<FileShareID, std::string> mIP;
    std::map<FileShareID, bool > mSending;
    std::map<FileShareID, bool > mRecving;

    // may invalidate
    void SendToAllBut(
      FileShareID index, 
      const OutgoingMessage & msg, 
      std::string & errmsg,
      std::vector<FileShareID> & invalidatedIds);

    void SendToAll(const OutgoingMessage & msg,
      std::string & errmsg,
      std::vector<FileShareID> & invalidids)
    {

      // using this type of loop because we may erase in the middle
      for (std::pair<const FileShareID, Connection> & curPair : mConnections)
      {
        FileShareID curId = curPair.first;

        Connection & curConection= curPair.second;
        SPSocket & curSocket = curConection.mSocket;

        SendMsg(curSocket, msg);
        if (!curSocket)
        {
          errmsg += GetWSAErrorString(WSAGetLastError());
          errmsg += "\n";
          invalidids.push_back(curId);
        }
      }


    }

    SPSocket mListener;
    SPTweakBar mTweakBar;

    unsigned mClientBufSize; 
  };

} // Tac
