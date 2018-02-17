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

#include "TacCS260FileShareClient.h"
#include "TacCS260FileShareCommon.h"
#include "TacRunningAverage.h"
#include "TacStateMachine.h"
#include "TacMessageUtil.h"
#include "TacVeryUtil.h"
#include <sstream>
#include <string>

namespace Tac
{
  class FileShareClientRunning : public State<FileShareClient>
  {
  public:
    FileShareClientRunning(SPSocket server);
    virtual void Init();
    virtual void Update(float dt);

    void RequestFile(
      FileShareID id, 
      const File & requested);

    //private: // ... 
  public:
    bool mConnectedToServer;
    bool mUsingTimeout;
    float mTimeoutTime;
    char mRecvBuf[MAX_BUFFER_BYTES];
    std::string mUDPPort;
    std::string mUDPIpv4Addr;
    SPSocket mUDPSock;

    FileShareID mId;
    Connection mServer;

    SPTweakBar mTweakBar;
    SPTweakBar mAvailableForDownload;

    static void TW_CALL OnDownloadFileButton(void * data);

    struct ButtonData
    {
      ButtonData(
        const File & downloadFile, 
        FileShareClientRunning * owner,
        FileShareID fileownerId)
        : mOwner(owner)
        , mFile(downloadFile)
        , mFileOwnerID(fileownerId)
      {
        // add to tweakbar
        std::stringstream address;
        address << (void const *)this;
        std::string name = address.str();

        std::string desc;
        desc += downloadFile.mName;
        desc += " (" + FileLenToString(downloadFile.mLen) + ")";
        if (mOwner->mAvailableForDownload)
        {
          mOwner->mAvailableForDownload->AddButton(
            to_string((const void*) this), 
            desc, 
            OnDownloadFileButton,
            this,
            "ID#" + std::to_string(mFileOwnerID));
        }
      }
      ~ButtonData()
      {
        // remove button from tweakbar
        std::stringstream address;
        address << (void const *)this;
        std::string name = address.str();

        if (mOwner->mAvailableForDownload)
          mOwner->mAvailableForDownload->RemoveVar(name);
      }
      FileShareClientRunning * mOwner;
      File mFile;
      FileShareID mFileOwnerID;
    };

    DefineSharedPtr(ButtonData);
    std::map<ButtonData *, SPButtonData> mButtonData;

    std::string mReceivedFolder;
    std::string mSharedFolder;

    unsigned mOutBufDataSize; // does not include sizeof(UDPheader)

    std::vector<std::string> mSharedFiles;
    std::vector<long long> mSharedFileLengths;

    bool LoadConfigAndUDPPort(const std::string & filepath, std::string & err);

    ShareMap mSharedWithUs;

    void UpdateIncoming(float dt);
    void UpdateOutgoing(float dt);
    void CheckForIncomingTimeouts();
    void CheckForOutgoingTimeouts();
    void SendAcks();
    void SendData();
    void Recv();

    void HandleServerDisconnect();

    struct FileTransfer2
    {
      enum Type{Up, Down};

      FileTransfer2(
        Type transferType,
        File f, 
        const std::string & port, 
        const std::string & ip, 
        FileShareClientRunning * owner,
        FileShareID otherGuysId);
      ~FileTransfer2();

      void Update(unsigned bytesSentThisLoop);
      void UpdatePercentDone();

      Type mTransferType;
      File mFile;
      long long mBytesTransferred;
      //float mPercentDone;
      std::string mOtherGuyUDPPort;
      std::string mOtherGuyIP;
      float mLastUpdateTime;
      SPTweakBar mTweakBar;
      FileShareClientRunning * mOwner;
      FileShareID mOtherGuysId;
      float mPercentDone;
      RunningAverage mRunningBytesPerSec;
      float mAvgBytesPerSec;
      float mTimeLeft;
    };
    DefineSharedPtr(FileTransfer2);

    SPFileTransfer2 mOutgoing;
    SPFileTransfer2 mIncoming;



  };

} // Tac
