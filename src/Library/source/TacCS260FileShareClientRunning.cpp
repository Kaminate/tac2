/*!
\author Nathan Park
\brief
*/

#include "TacCS260FileShareClientRunning.h"
#include "TacCS260FileShareClientError.h"
#include "TacCS260FileShareCommon.h"
#include "TacCore.h"
#include <TacVeryUtil.h>
#include "TacMathUtil.h"
#include "Shlwapi.h"
#include <direct.h>

namespace Tac
{
  FileShareClientRunning::FileShareClientRunning( 
    SPSocket server)
    : mUsingTimeout(true)
    , mTimeoutTime(2.0f)
    , mConnectedToServer(true)
  {
    mServer.mSocket = server;
  }

  void TW_CALL FuckThisShitClient(void * data)
  {
    FileShareClientRunning * fileshare = (FileShareClientRunning*) data;
    fileshare->GetStateMachine()->SetNextState(
      SPFileShareClientState(new FileShareClientError(
      "Forced quit")));
  }

  void FileShareClientRunning::Init()
  {
    FileShareClient * fileshare = Get();
    AppSystem * app = fileshare->GetOwner();
    Tweak & tw = app->GetCore()->GetTweak();

    mTweakBar = tw.Create();
    mTweakBar->Init("Client");
    mTweakBar->SetSize(300, 250);
    mTweakBar->SetPosition(50,50);
    mTweakBar->AddComment("", "Connected to server");
    mTweakBar->AddVar("", "label='buffer size'", TW_TYPE_UINT32,
      &mOutBufDataSize, TweakBar::ReadOnly);
    mTweakBar->AddVar("", "label='timeout time' min=1 max=5 step=0.1 ", TW_TYPE_FLOAT, &mTimeoutTime);
    mTweakBar->AddVar("", "label='use timeout'", TW_TYPE_BOOL16, &mUsingTimeout);

    mAvailableForDownload = tw.Create();
    mAvailableForDownload->Init("Available");
    mAvailableForDownload->SetSize(370, 300);
    mAvailableForDownload->SetPosition(400, 50);

    std::string err;
    if (!LoadConfigAndUDPPort(gConfigFilepath, err))
    {
      Get()->Error(err);
      return;
    }

    if (!ListFilesInDir(mSharedFolder, mSharedFiles, mSharedFileLengths))
    {
      fileshare->Error("Failed to list files in dir");
      return;
    }

    SendMsg(mServer.mSocket, FileShareMsg::CtoSUDPPort(mUDPPort).ToMsg());
    if (!mServer.mSocket)
    {
      Get()->Error("Disconnected from server during init");
      return;
    }

    for (unsigned i = 0; i < mSharedFiles.size(); ++i)
    {
      std::string desc;
      const std::string & curFile = mSharedFiles[i];
      long long curLen = mSharedFileLengths[i];
      desc += curFile;
      desc += " ";
      desc += std::to_string(curLen);
      desc += " bytes";
      mTweakBar->AddButton("", desc, nullptr, nullptr, "Sharedfiles");

      SendMsg(mServer.mSocket, FileShareMsg::CtoSCanShare(curFile, curLen).ToMsg());
      if (!mServer.mSocket)
      {
        Get()->Error("Disconnected from server during init");
        return;
      }
    }

    mTweakBar->AddButton("", "force quit", FuckThisShitClient, this);

    std::string requestSharedStr = FileShareMsg::GetName(FileShareMsg::Type::eCtoSRequestShared);
    SendMsg(mServer.mSocket, OutgoingMessage(requestSharedStr.c_str()));
    if (!mServer.mSocket)
    {
      Get()->Error("Disconnected from server during init");
      return;
    }
  }

  void FileShareClientRunning::Update( float dt )
  {
    UpdateIncoming(dt);
    UpdateOutgoing(dt);
    Recv();

    const unsigned bufSize = 256;
    char buffer[bufSize];
    std::vector<SPIncomingMessage> received;
    ReceiveMessages(
      mServer.mSocket, 
      received, 
      mServer.mInTransit, 
      buffer, 
      bufSize);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    
    HandleServerDisconnect();

    for (const SPIncomingMessage & msg : received)
    {
      FileShareMsg::Type msgType = FileShareMsg::GetType(msg->mName);
      switch (msgType)
      {
      case FileShareMsg::eStoCAssignId:
        {
          FileShareMsg::StoCAssignId assignIdMsg(msg);
          mId = assignIdMsg.mIndex;
          mTweakBar->AddComment("", "Your ID is: " + std::to_string(mId));
          break;
        }
      case FileShareMsg::eStoCClientDisconnect:
        {
          // todo: remove from tweakbar
          FileShareMsg::StoCClientDisconnect dc(msg);
          mSharedWithUs.erase(dc.mId);

          if (mOutgoing && mOutgoing->mOtherGuysId == dc.mId)
            mOutgoing.reset();

          if (mIncoming && mIncoming->mOtherGuysId == dc.mId)
            mIncoming.reset();


          // remove the disconnected clients download buttons
          auto it = mButtonData.begin();
          while (it != mButtonData.end())
          {
            std::pair<ButtonData * , SPButtonData> p = *it;
            if (p.first->mFileOwnerID == dc.mId)
              it = mButtonData.erase(it);
            else
              ++it;
          }

          break;
        }
      case FileShareMsg::eStoCBeingShared:
        {
          FileShareMsg::StoCBeingShared sharedMsg(msg);

          typedef std::set<File> FileSet;
          typedef std::map<FileShareID, FileSet> ShareMap;
          File sharedFile(sharedMsg.mFilename, sharedMsg.mFileSize);
          FileSet & shared = mSharedWithUs[sharedMsg.mId];
          auto it = shared.find(sharedFile);
          if (it == shared.end())
          {
            // doesnt have the file. lets insert it and add the button
            shared.insert(sharedFile);

            SPButtonData data(new ButtonData(sharedFile, this, sharedMsg.mId));
            mButtonData[data.get()] = data;
          }
          else
          {
            ErrorIf(true, "A client shared the same file with us twice");
          }

          /*
          IncomingFileSet & inSet = mSharedWithUs[sharedMsg.mId];

          FileTransfer incoming;
          incoming.mTransferType = FileTransfer::Type::eNone;
          incoming.mBytesTransferred = 0;

          File inFile(sharedMsg.mFilename, sharedMsg.mFileSize);

          inSet[inFile] = incoming;

          RecalculateAvailableTweak();
          */

          break;
        }
      case FileShareMsg::eStoCUpdateBufSize:
        {
          FileShareMsg::StoCUpdateBufSize bufmsg(msg);

          // it will be clamped later
          mOutBufDataSize = bufmsg.mBufSize;
          break;
        }
      case FileShareMsg::eStoCYouAreDownloding:
        {
          FileShareMsg::StoCYouAreDownloading m(msg);
          ErrorIf(mIncoming, "already downloading");
          mIncoming = SPFileTransfer2(
            new FileTransfer2(
            FileTransfer2::Type::Down,
            m.mf,
            m.mport,
            m.mip,
            this,
            m.mID));
          break;
        }
      case FileShareMsg::eStoCYouAreUploading:
        {
          FileShareMsg::StoCYouAreUploading m(msg);
          ErrorIf(mOutgoing, "already uploading");
          mOutgoing = SPFileTransfer2(
            new FileTransfer2(
            FileTransfer2::Type::Up,
            m.mf,
            m.mport,
            m.mip,
            this,
            m.mID));
          break;
        }
      case Tac::FileShareMsg::eCount:
        break;
      default:
        break;
      }
    }
  }

  bool FileShareClientRunning::LoadConfigAndUDPPort(const std::string & filepath, std::string & err)
  {
    std::ifstream ifs(filepath);
    bool success = ifs.is_open();
    if (success)
    {
      std::string serverIP;
      std::string serverPort;
      ifs 
        >> serverIP 
        >> serverPort 
        >> mUDPPort
        >> mReceivedFolder 
        >> mSharedFolder 
        >> mOutBufDataSize;


      if (!PathFileExists(mReceivedFolder.c_str()))
        _mkdir(mReceivedFolder.c_str());
      if (!PathFileExists(mSharedFolder.c_str()))
        _mkdir(mSharedFolder.c_str());

    }
    else
    {
      err = "failed to open " + filepath;
      return false;
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
        err = "inet_ntoa failed " + GetWSAErrorString(WSAGetLastError());
        return false;
      }
    }
    else
    {
      err = "gethostbyname failed " + GetWSAErrorString(WSAGetLastError());
      return false;
    }


    // create the udp socket
    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // ipv4
    hints.ai_socktype = SOCK_DGRAM; 
    hints.ai_protocol = IPPROTO_UDP; 
    hints.ai_flags = AI_PASSIVE;


    std::shared_ptr<AddrInfo> fo;
    bool bindSucceeded = false;
    while(!bindSucceeded)
    {
      // If the pNodeName parameter contains an empty string, 
      // all registered addresses on the local computer are returned.
      fo = std::shared_ptr<AddrInfo>(new AddrInfo(nullptr, mUDPPort.c_str(), &hints));
      if (!fo->Ok()){
        err = fo->GetSpecificError();
        return false;
      }

      mUDPSock = SPSocket(new Socket(*fo));

      if (!mUDPSock->Ok())
      {
        err = "socket failed " + GetWSAErrorString(WSAGetLastError());
        return false;
      }

      if (!mUDPSock->Bind(*fo))
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
      err ="Nonblock failed" + GetWSAErrorString(WSAGetLastError())
        ;return false;
    }

    return true;
  }

  void FileShareClientRunning::CheckForIncomingTimeouts()
  {
    if (mIncoming)
    {
      float currTIme = Get()->GetOwner()->GetCore()->GetElapsedTime();
      float difftime = currTIme - mIncoming->mLastUpdateTime;

      if (difftime > mTimeoutTime)
      {
        mIncoming.reset();
      }
    }
  }

  void FileShareClientRunning::CheckForOutgoingTimeouts()
  {
    if (mOutgoing)
    {
      float currTIme = Get()->GetOwner()->GetCore()->GetElapsedTime();
      float difftime = currTIme - mOutgoing->mLastUpdateTime;

      if (difftime > mTimeoutTime)
      {
        mOutgoing.reset();
      }
    }
  }

  void FileShareClientRunning::SendAcks()
  {
    if (mIncoming)
    {
      // tell the owner how much we've received
      UDPHeader * header = reinterpret_cast<UDPHeader*>(mRecvBuf);
      header->mID = mId;
      header->mFileBytesSent = 0;
      header->mFileLen = mIncoming->mFile.mLen;
      strncpy_s(header->mFileName, mIncoming->mFile.mName.c_str(), MAX_FILENAME_LEN);
      header->mIsAnAck = true;
      header->mFirstByteIndex = mIncoming->mBytesTransferred; // next byte we want

      int port = atoi(mIncoming->mOtherGuyUDPPort.c_str());
      // todo: use getsockopt with value of socket option SO_MAX_MSG_SIZE
      sockaddr_in RecvAddr;
      RecvAddr.sin_family = AF_INET;
      RecvAddr.sin_port = htons(port);
      RecvAddr.sin_addr.s_addr = inet_addr(mIncoming->mOtherGuyIP.c_str());

      int iresult = sendto(
        mUDPSock->mSocket, 
        mRecvBuf, 
        sizeof(UDPHeader), 
        0,
        (sockaddr *) &RecvAddr, 
        sizeof(RecvAddr));

      if (iresult == SOCKET_ERROR)
      {
        switch (WSAGetLastError())
        {
        case WSAEWOULDBLOCK:
          // ignore
          break;
        case WSAECONNRESET:
          mIncoming.reset();
          break;
        default:
          std::string err = GetWSAErrorString(WSAGetLastError());
          Get()->Error(err);
          break;
        }
      }
    }
  }

  void FileShareClientRunning::SendData()
  {
    if (!mOutgoing)
      return;

    const File & outFile = mOutgoing->mFile;

    Clamp(
      mOutBufDataSize, 
      (unsigned) MIN_BUFFER_BYTES, 
      (unsigned) MAX_BUFFER_BYTES - sizeof(UDPHeader));

    long long numBytesRemaining = outFile.mLen - mOutgoing->mBytesTransferred;

    unsigned maxBytesToSend 
      = numBytesRemaining > (long long) mOutBufDataSize
      ? mOutBufDataSize
      : (unsigned) numBytesRemaining;

    //maxBytesToSend = strlen("hello world"); // temp
    UDPHeader * header = reinterpret_cast<UDPHeader*>(mRecvBuf);
    header->mFileBytesSent = maxBytesToSend;
    header->mFileLen = outFile.mLen;
    header->mFirstByteIndex = mOutgoing->mBytesTransferred;
    header->mID = mId;
    header->mIsAnAck = false;

    if (outFile.mName.length() > MAX_FILENAME_LEN)
    {
      Get()->Error(
        "file name " + outFile.mName + " is too long "
        + "(max " + std::to_string(MAX_FILENAME_LEN) + " chars)");
      return;
    }
    strncpy_s(header->mFileName, outFile.mName.c_str(), MAX_FILENAME_LEN);

    // starting point where we can memcpy to
    char * dataBuf = mRecvBuf + sizeof(UDPHeader);

    // fill out the data
    std::string fullpath = mSharedFolder + "\\" + outFile.mName;
    std::ifstream ifs(fullpath, std::ifstream::binary);
    if (ifs.is_open())
    {
      ifs.seekg(mOutgoing->mBytesTransferred);
      ifs.read(dataBuf, maxBytesToSend);
      ifs.close();
    }
    else
    {
      Get()->Error("Failed to open file " + fullpath);
      return;
    }

    int port = atoi(mOutgoing->mOtherGuyUDPPort.c_str());
    // todo: use getsockopt with value of socket option SO_MAX_MSG_SIZE
    sockaddr_in RecvAddr;
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(port);
    RecvAddr.sin_addr.s_addr = inet_addr(mOutgoing->mOtherGuyIP.c_str());

    int iresult = sendto(
      mUDPSock->mSocket, 
      mRecvBuf, 
      sizeof(UDPHeader) + unsigned(header->mFileBytesSent), 
      0, 
      (SOCKADDR *) & RecvAddr, 
      sizeof (RecvAddr));


    if (iresult == SOCKET_ERROR)
    {
      int lastError = WSAGetLastError();
      switch (lastError)
      {
      case WSAEWOULDBLOCK:
        // ignore
        break;
      case WSAECONNRESET:
        mOutgoing.reset();
        break;
      default:
        Get()->Error(GetWSAErrorString(lastError));
        break;
      }

      return;
    }
  }

  void FileShareClientRunning::Recv()
  {
    int iresult = recvfrom(
      mUDPSock->mSocket, 
      mRecvBuf, 
      MAX_BUFFER_BYTES, 
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
        std::string err = GetWSAErrorString(lastError);
        Get()->Error(err);
        break;
      }
    }
    else
    {
      UDPHeader * header = reinterpret_cast<UDPHeader*>(mRecvBuf);
      const char * actualData = mRecvBuf + sizeof(UDPHeader);

      File inFile(header->mFileName, header->mFileLen);


      if (header->mIsAnAck)
      {
        // we're uploading a outfile to a guy, and he's acking to tell us
        if (!mOutgoing)
          return;

        if (header->mID != mOutgoing->mOtherGuysId)
        {
          mTweakBar->AddComment("", "The wrong person is acking us");
          return;
        }

        long long ackBytesTransferred = header->mFirstByteIndex;

        if (ackBytesTransferred > mOutgoing->mBytesTransferred)
        {
          long long difference 
            = ackBytesTransferred - mOutgoing->mBytesTransferred;

          mOutgoing->Update((unsigned) difference);
        }

      }
      else
      {
        // we're downloading from a guy, and he's sending us 
        // data for our incoming file
        if (!mIncoming)
          return;

        if (header->mID != mIncoming->mOtherGuysId)
        {
          mTweakBar->AddComment("", "The wrong person is uploading to us");
          return;
        }

        if (mIncoming->mBytesTransferred != header->mFirstByteIndex)
          return;

        std::string fullpath = mReceivedFolder + "\\" + inFile.mName;


        std::ofstream ofs( 
          fullpath,
          std::ofstream::app | std::ofstream::binary);

        if (!ofs.is_open())
        {
          Get()->Error("Cannot open file " + fullpath);
          return;
        }

        ofs.seekp(0, std::ios_base::end);
        long long len = ofs.tellp();
        if (len == inFile.mLen)
        {
          // file transfer complete
          mIncoming.reset();
        }
        else
        {
          ofs.write(actualData, header->mFileBytesSent);

          mIncoming->Update(header->mFileBytesSent);

          if (inFile.mLen == mIncoming->mBytesTransferred)
          {
            // we're done
            mIncoming.reset();
          }
        }
      }
    }
  }

  void FileShareClientRunning::UpdateIncoming( float dt )
  {
    if (mUsingTimeout)
      CheckForIncomingTimeouts();

    SendAcks();
  }

  void FileShareClientRunning::UpdateOutgoing( float dt )
  {
    if (mUsingTimeout)
      CheckForOutgoingTimeouts();

    SendData();
  }

  void TW_CALL FileShareClientRunning::OnDownloadFileButton( void * data )
  {
    ButtonData * bData = (ButtonData *) data;
    bData->mOwner->RequestFile(bData->mFileOwnerID, bData->mFile);
  }

  void FileShareClientRunning::RequestFile( FileShareID id, const File & requested )
  {
    if (mIncoming)
      mTweakBar->AddComment("", "Already downloading a file");
    else
    {
      SendMsg(
        mServer.mSocket, 
        FileShareMsg::CtoSRequestDownload(
        id, 
        requested.mName,
        requested.mLen).ToMsg());
      if (!mServer.mSocket)
        HandleServerDisconnect();
    }
  }

  void FileShareClientRunning::HandleServerDisconnect()
  {
    if (!mServer.mSocket)
    {
      if (mConnectedToServer)
      {
        mConnectedToServer = false;
        mTweakBar->AddComment("", "Disconnected from server");
      }
      mAvailableForDownload.reset();
    }
  }

  FileShareClientRunning::FileTransfer2::FileTransfer2( 
    Type transferType, 
    File f, 
    const std::string & port, 
    const std::string & ip, 
    FileShareClientRunning * owner, 
    FileShareID otherGuysId ) 
    : mTweakBar()
    , mTransferType(transferType)
    , mBytesTransferred(0)
    , mFile(f)
    , mLastUpdateTime(owner->Get()->GetOwner()->GetCore()->GetElapsedTime())
    , mOtherGuyIP(ip)
    , mOtherGuyUDPPort(port)
    , mOtherGuysId(otherGuysId)
    , mOwner(owner)
    , mPercentDone(0)
    , mRunningBytesPerSec(60)
    , mAvgBytesPerSec(0)
    , mTimeLeft(0)
  {
    mTweakBar = owner->Get()->GetOwner()->GetCore()->GetTweak().Create();

    std::string name; 
    name += mTransferType == Down ? "Downloading" : "Uploading";

    mTweakBar->Init(name);
    mTweakBar->AddComment("", mFile.mName);
    mTweakBar->AddComment("", FileLenToString(mFile.mLen));
    mTweakBar->AddVar(
      "",
      "label='Percent Complete' precision=2", 
      TW_TYPE_FLOAT, 
      &mPercentDone, 
      TweakBar::Type::ReadOnly);
    mTweakBar->AddVar(
      "", 
      "label='Average bytes/sec' precision=0", 
      TW_TYPE_FLOAT, 
      &mAvgBytesPerSec,
      TweakBar::Type::ReadOnly);
    mTweakBar->AddVar(
      "",
      "label = 'seconds remaining'",
      TW_TYPE_FLOAT,
      &mTimeLeft,
      TweakBar::ReadOnly);

    if (mTransferType == Down)
    {
      // see if transfer is partially done already
      // (correct bytesTransferred)
      std::string fullpath(mOwner->mReceivedFolder);
      fullpath += "\\";
      fullpath += mFile.mName;
      std::ofstream ofs(fullpath, 
        std::ofstream::app | std::ofstream::binary);
      if (ofs.is_open())
      {
        ofs.seekp(0, std::ios_base::end);
        long long len = ofs.tellp();
        mBytesTransferred = len;
        UpdatePercentDone();
      }
      // its ok to not be able to open the file (transfer hasnt started)
    }
  }

  FileShareClientRunning::FileTransfer2::~FileTransfer2()
  {
    switch (mTransferType)
    {
    case Tac::FileShareClientRunning::FileTransfer2::Up:
      SendMsg(mOwner->mServer.mSocket, FileShareMsg::CtoSUploadDone().ToMsg());
      break;
    case Tac::FileShareClientRunning::FileTransfer2::Down:
      SendMsg(mOwner->mServer.mSocket, FileShareMsg::CtoSDownloadDone().ToMsg());
      break;
    }

    mOwner->HandleServerDisconnect();
  }

  void FileShareClientRunning::FileTransfer2::Update( unsigned fileBytesSent )
  {
    mBytesTransferred += fileBytesSent;
    UpdatePercentDone();

    float currTime = mOwner->Get()->GetOwner()->GetCore()->GetElapsedTime();
    float diffTime = currTime - mLastUpdateTime;
    float bytesPerSecond = fileBytesSent / diffTime;
    mRunningBytesPerSec.Push(bytesPerSecond);
    mAvgBytesPerSec = mRunningBytesPerSec.GetAverage();
    mLastUpdateTime = currTime;

    long long bytesRemaining = mFile.mLen - mBytesTransferred;

    mTimeLeft = bytesRemaining / mAvgBytesPerSec;
  }

  void FileShareClientRunning::FileTransfer2::UpdatePercentDone()
  {
    mPercentDone 
      = float(double(mBytesTransferred) / double(mFile.mLen));
    mPercentDone *= 100.0f;
  }

} // Tac
