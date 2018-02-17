/*!
\author Nathan Park
\brief
*/

#include "TacCS260FileShareServerRunning.h"
#include "TacCS260FileShareServerError.h"
#include "TacCore.h"
#include <TacCS260FileShareCommon.h>

namespace Tac
{
  FileShareServerRunning::FileShareServerRunning(
    StateMachine<FileShareServer> * sm, SPSocket listener)
    : State<FileShareServer>(sm)
    , mListener(listener)
    , mNextId(0)
  {

  }

  void TW_CALL FuckThisShitServer(void * data)
  {
    FileShareServerRunning * fileshare = (FileShareServerRunning*) data;
    fileshare->GetStateMachine()->SetNextState(
      SPFileShareServerState(new FileShareServerError(
      "todo: FileShareServerRunning::Init()")));
  }
  
  void TW_CALL UpdateBufCallback(void * data)
  {
    FileShareServerRunning * fileshare = (FileShareServerRunning *) data;
    fileshare->UpdateClientBufSizes();
  }

  void FileShareServerRunning::Init()
  {
    mTweakBar = Get()->GetOwner()->GetCore()->GetTweak().Create();
    mTweakBar->Init("Server");
    mTweakBar->SetSize(275, 200);
    mTweakBar->AddButton("", "force quit", FuckThisShitServer, this);

    std::string configfilepath("config.txt");
    std::ifstream ifs(configfilepath);
    if (ifs.is_open())
    {
      std::string str;
      ifs >> str >> str >> str >> str >> str >> str;
      mClientBufSize = atoi(str.c_str());
    }
    else
    {
      Get()->Error("Cannot open " + configfilepath);
    }

    std::string def;
    
    def += " label='Client Buffer Size'";
    def += " min=" + std::to_string(MIN_BUFFER_BYTES);
    def += " max=" + std::to_string(MAX_BUFFER_BYTES);
    mTweakBar->AddVar("", def, TW_TYPE_UINT32, &mClientBufSize);
    mTweakBar->AddButton("", "Update client buffer", UpdateBufCallback, 
      this);
  }

  void FileShareServerRunning::Update( float dt )
  {
    AcceptConnections();
    HandleMessages();
  }

  void FileShareServerRunning::AcceptConnections()
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

        FileShareID id = GetNextId();
        mConnections[id] = newConnection;
        mSending[id] = false;
        mRecving[id] = false;

        mTweakBar->AddComment("", 
          "Added a connection. " 
          + std::to_string(mConnections.size()) 
          + " connections.");

        SendMsg(acceptedSocket, FileShareMsg::StoCAssignId(id).ToMsg());
      }
    }
  }

  void FileShareServerRunning::HandleMessages()
  {
    const unsigned tcpBufSize = 100;
    char tcpBuffer[tcpBufSize];

    // using this type of loop because we may erase in the middle
    std::map<FileShareID, Connection>::iterator it = mConnections.begin();
    while (it != mConnections.end())
    {
      std::pair<const FileShareID, Connection> & curPair = *it;
      FileShareID curId = curPair.first;
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
        std::vector<FileShareID> disconnected;
        disconnected.push_back(curId);
        HandleDisconnect(disconnected);
        continue;
      }

      for (std::shared_ptr<IncomingMessage> & inMsg : incomingMessages)
      {
        switch (FileShareMsg::GetType(inMsg->mName))
        {
        case FileShareMsg::Type::eCtoSUDPPort:
          {
            FileShareMsg::CtoSUDPPort portmsg(inMsg);
            mUDPPorts[curId] = portmsg.mUDPPort;

            std::string ip, port, errs;
            // port is the port of client-server connection, not udp port 
            if (curSocket->GetPeerAddress(ip, port, errs))
              mIP[curId] = ip;
            else
              HandleDisconnect(curId);
            break;
          }
        case FileShareMsg::Type::eCtoSRequestDownload:
          {
            FileShareMsg::CtoSRequestDownload request(inMsg);
            FileShareID fileOwnerId = request.mId;

            auto it = mConnections.find(fileOwnerId);
            if (bool fileOwnerExists = it != mConnections.end())
            {
              Connection & fileOwner = (*it).second;

              // start the download if possible
              bool & ownerUploading= mSending[fileOwnerId];
              bool & wanterDownloading= mRecving[curId];

              if (!ownerUploading && !wanterDownloading)
              {
                ownerUploading = true;
                wanterDownloading = true;

                File fil(request.mFilename, request.mFileLen);

                FileShareMsg::StoCYouAreDownloading toDownloader(
                  fil, 
                  mIP[fileOwnerId], 
                  mUDPPorts[fileOwnerId],
                  fileOwnerId);
                SendMsg(curSocket, toDownloader.ToMsg());
                if (!curSocket)
                  HandleDisconnect(curId);

                FileShareMsg::StoCYouAreUploading toUploader(
                  fil, 
                  mIP[curId], 
                  mUDPPorts[curId],
                  curId);
                SendMsg(fileOwner.mSocket, toUploader.ToMsg());
                if (!fileOwner.mSocket)
                  HandleDisconnect(fileOwnerId);
              }


              /*
              request.mId = curId;
              SendMsg(fileOwner.mSocket, request.ToMsg());
              if (!fileOwner.mSocket)
              HandleDisconnect(fileOwnerId);
              */


              /*
              FileShareMsg::UDPPortIpOfGuyYoureDownloadingFrom udpmsg(
              fileOwnerId, 
              request.mFilename,
              request.mFileLen,
              mUDPPorts[fileOwnerId],
              ip);

              SendMsg(curSocket, udpmsg.ToMsg());
              if (!curSocket)
              HandleDisconnect(curId);
              */

            }
            break;
          }
        case FileShareMsg::Type::eCtoSRequestShared:
          {
            bool lostConnection = false;

            // todo: send ever canshare data to them
            for (std::pair<const FileShareID, std::set<File> > & curPair 
              : mShareMap)
            {
              FileShareID curPairId = curPair.first;
              if (curPairId == curId)
                continue;

              std::set<File> & curPairMap = curPair.second;
              for (const File & curFile : curPairMap)
              {
                FileShareMsg::StoCBeingShared sharedMsg(
                  curPairId,
                  curFile.mName,
                  curFile.mLen);
                SendMsg(curSocket, sharedMsg.ToMsg());
                if (!curSocket)
                {
                  lostConnection = true;
                  break;
                }
              }
              if (lostConnection)
                break;
            }
            if (lostConnection)
              HandleDisconnect(curId);
            break;
          }
        case FileShareMsg::Type::eCtoSCanShare:
          {
            FileShareMsg::CtoSCanShare specificMsg(inMsg);
            FileShareMsg::StoCBeingShared sharedMsg(
              curId,
              specificMsg.mFilename, 
              specificMsg.mFileSize);

            std::vector<FileShareID> disconnectedIds;
            std::string sendtoallErrs;
            // may invalidate
            SendToAllBut(
              curId, 
              sharedMsg.ToMsg(), 
              sendtoallErrs, 
              disconnectedIds);
            HandleDisconnect(disconnectedIds);

            mShareMap[curId].insert(
              File(specificMsg.mFilename, specificMsg.mFileSize));

            break;
          }
        case FileShareMsg::Type::eCtoSDownloadDone:
          {
            FileShareMsg::CtoSDownloadDone done(inMsg);
            mRecving[curId] = false;
            break;
          }
        case FileShareMsg::Type::eCtoSUploadDone:
          {
            FileShareMsg::CtoSUploadDone done(inMsg);
            mSending[curId] = false;
            break;
          }
        case FileShareMsg::Type::eCount:
          break;
        default:
          Get()->Error("Unhandled message " + std::string(inMsg->mName));
          return;
        }
      }
      ++it;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  // this removes from the vector, so it may invalidate an interator
  void FileShareServerRunning::SendToAllBut(
    FileShareID index, 
    const OutgoingMessage & msg, 
    std::string & errmsg,
    std::vector<FileShareID> & invalidatedIds)
  {

    // using this type of loop because we may erase in the middle
    for (std::pair<const FileShareID, Connection> & curPair : mConnections)
    {
      FileShareID curId = curPair.first;
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

  Tac::FileShareID FileShareServerRunning::GetNextId()
  {
    if (!mFreeIds.empty())
    {
      FileShareID id = mFreeIds.back();
      mFreeIds.pop_back();
      return id;
    }
    return mNextId++;
  }

  void FileShareServerRunning::HandleDisconnect( 
    const std::vector<FileShareID> & disconnected )
  {
    for (FileShareID id : disconnected)
    {
      // remove
      mConnections.erase(id);
      mShareMap.erase(id);
      mUDPPorts.erase(id);
      mSending.erase(id);
      mRecving.erase(id);

      mTweakBar->AddComment("", 
        "Lost a connection, " 
        + std::to_string(mConnections.size()) 
        + " left");

      // send message to remaining,
      std::vector<FileShareID> moreDisconnected;
      std::string errors;
      SendToAllBut(
        id, 
        FileShareMsg::StoCClientDisconnect(id).ToMsg(), 
        errors, 
        moreDisconnected);
      // during the send message, store the disconnect in an array
      // call this function recursively
      HandleDisconnect(moreDisconnected);
    }
  }

  void FileShareServerRunning::HandleDisconnect( FileShareID disconnected )
  {
    std::vector<FileShareID> lost;
    lost.push_back(disconnected);
    HandleDisconnect(lost);
  }

} // Tac
