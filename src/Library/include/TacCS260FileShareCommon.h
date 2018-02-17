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

#include <string>
#include <map>
#include <set>
#include "TacMessage.h"
#include "TacTweak.h"
#include "TacSingleton.h"

namespace Tac
{
#define MAX_BUFFER_BYTES 65507
#define MIN_BUFFER_BYTES 8
#define MAX_FILENAME_LEN 50

  class UDPHeader
  {
  public:
    int mID;
    char mFileName[MAX_FILENAME_LEN];
    long long mFileLen;

    long long mFirstByteIndex;
    unsigned mFileBytesSent; // num of bytes to send with this header

    // if this is true, mFirstByteIndex represents the index of 
    // the next byte we want (also == numBytesReceived)
    bool mIsAnAck; 
  };

  extern std::string gConfigFilepath;
  typedef int FileShareID;

  class File
  {
  public:
    File(std::string name, long long len);
    std::string mName;
    long long mLen;
    bool operator < (const File & rhs) const;
  };

  // example output: 4 megabytes
  std::string FileLenToString(long long lenInBytes);


  /*
  class FileTransfer
  {
  public:
  enum Type{eNone, eIncoming, eOutgoing};
  static std::string to_string(Type transferType);
  FileTransfer();
  ~FileTransfer()
  {
  int i = 0;
  ++i;
  }
  void OpenBar(
  Tweak & creator, 
  FileShareID from, 
  FileShareID to, 
  const File & transferring,
  float curTime);
  void Finish()
  {
  mTransferType = eNone;
  mBytesTransferred = 0;
  mOtherGuyUDPPort = "";
  mOtherGuyIP = "";
  mBar.reset();
  }

  Type mTransferType;
  long long mBytesTransferred;
  //float mPercentDone;
  std::string mOtherGuyUDPPort;
  std::string mOtherGuyIP;
  float mLastUpdateTime;

  private:
  SPTweakBar mBar;
  };
  */


  //typedef std::map<File, FileTransfer> IncomingFileSet;
  //typedef std::map<FileShareID, IncomingFileSet> ShareMap;
  typedef std::set<File> FileSet;
  typedef std::map<FileShareID, FileSet> ShareMap;

  namespace FileShareMsg
  {



    enum Type
    {
      // server to client
      eStoCClientDisconnect,
      eStoCAssignId,
      eStoCBeingShared,
      eStoCYouAreDownloding,
      eStoCYouAreUploading,
      eStoCUpdateBufSize,

      // client to servr
      eCtoSCanShare,
      eCtoSUDPPort,
      eCtoSRequestShared, // empty msg
      eCtoSRequestDownload,
      eCtoSDownloadDone,
      eCtoSUploadDone,
      
      eCount,
    };
    TacDLLAPI std::string GetName(Type messageType);
    // returns eCount on failure
    TacDLLAPI Type GetType(const std::string & message);


    /*
    class MsgManagr : public Singleton<MsgManagr>
    {
    public:
    void RegisterMessage(Type msgType, const std::vector<unsigned> & offsets)
    {
    mData[msgType] = data;
    }

    OutgoingMessage ToMsg(Type msgType, const void * msgStrut)
    {
    const char * c = reinterpret_cast<const char *>(msgStrut);

    OutgoingMessage msg(GetName(msgType).c_str());
    const std::vector<DataPtr> data = mData[msgType];
    for (const DataPtr & ptr : data)
    {
    if (ptr.mType == Data::Type::eCString)
    {
    // todo: differ between cstr and std string
    std::string * s = c + ptr.


    }
    ptr.mPointer;
    ptr.mSize;
    ptr.mType;
    }
    msg.AddData()

    }

    template<typename T>
    void FromMsg(T & t, const IncomingMessage & msg);

    private:
    MsgManagr(); friend class Singleton<MsgManagr>;
    std::vector<DataPtr> mData[Type::eCount];
    };
    */

    struct StoCUpdateBufSize
    {
      StoCUpdateBufSize(const SPIncomingMessage & msg)
      {
        mBufSize = msg->mData[0].mUnsigned;
      }
      StoCUpdateBufSize(unsigned bufsize) : mBufSize(bufsize)
      {
        
      }
      OutgoingMessage ToMsg()
      {
        std::string msgType = GetName(FileShareMsg::Type::eStoCUpdateBufSize);
        OutgoingMessage out(msgType.c_str());
        out.AddData(DataPtr(Data::Type::eUnsigned, &mBufSize, sizeof(unsigned)));
        return out;
      }
      unsigned mBufSize;
    };

    // server to client
    struct StoCAssignId
    {
      StoCAssignId(const SPIncomingMessage & msg)
      {
        mIndex = msg->mData[0].mInt;
      }
      StoCAssignId(FileShareID index) : mIndex(index)
      {
      }
      OutgoingMessage ToMsg()
      {
        std::string msgType = GetName(FileShareMsg::Type::eStoCAssignId);
        OutgoingMessage out(msgType.c_str());
        out.AddData(DataPtr(Data::Type::eInt, &mIndex, sizeof(int)));
        return out;
      }
      FileShareID mIndex;
    };

    // sent to server on connection
    struct CtoSUDPPort
    {
      CtoSUDPPort(const SPIncomingMessage & msg)
      {
        mUDPPort = msg->mData[0].GetString();
      }
      CtoSUDPPort(const std::string & udpport) : mUDPPort(udpport){}
      OutgoingMessage ToMsg()
      {
        std::string msgType = GetName(FileShareMsg::Type::eCtoSUDPPort);
        OutgoingMessage out(msgType.c_str());
        out.AddData(DataPtr(
          Data::Type::eCString, 
          mUDPPort.c_str(), 
          mUDPPort.length()));
        return out;
      }
      std::string mUDPPort;
    };

    struct CtoSCanShare
    {
      CtoSCanShare(const SPIncomingMessage & msg);
      CtoSCanShare(const std::string & filename, long long filesize);
      OutgoingMessage ToMsg();
      std::string mFilename;
      long long mFileSize;
    };

    // send from server to clients
    struct StoCBeingShared
    {
      StoCBeingShared(const SPIncomingMessage & msg);
      StoCBeingShared(
        FileShareID id,
        const std::string & filename, 
        long long filesize);
      OutgoingMessage ToMsg();
      int mId;
      std::string mFilename;
      long long mFileSize;
    };

    // server to client
    struct StoCClientDisconnect
    {
      StoCClientDisconnect(const SPIncomingMessage & msg)
      {
        mId = msg->mData[0].mInt;
      }
      StoCClientDisconnect(FileShareID id) : mId(id)
      {
      }
      OutgoingMessage ToMsg()
      {
        std::string msgType
          = GetName(FileShareMsg::Type::eStoCClientDisconnect);
        OutgoingMessage out(msgType.c_str());
        out.AddData(DataPtr(Data::Type::eInt, &mId, sizeof(int)));
        return out;
      }
      FileShareID mId;
    };

    struct CtoSRequestDownload
    {
      CtoSRequestDownload(const SPIncomingMessage & msg)
      {
        unsigned index = 0;
        mId = msg->mData[index++].mInt;
        mFilename = msg->mData[index++].GetString();
        mFileLen = msg->mData[index++].mLongLong;
      }
      CtoSRequestDownload(
        FileShareID id, 
        const std::string & filename,
        long long filelen) 
        : mId(id)
        , mFilename(filename)
        , mFileLen(filelen)
      {
      }
      OutgoingMessage ToMsg()
      {
        std::string msgType
          = GetName(FileShareMsg::Type::eCtoSRequestDownload);
        OutgoingMessage out(msgType.c_str());
        out.AddData(DataPtr(Data::Type::eInt, &mId, sizeof(int)));
        out.AddData(DataPtr(Data::Type::eCString, mFilename.c_str(), mFilename.size()));
        out.AddData(DataPtr(Data::Type::eLongLong, &mFileLen, sizeof(long long)));
        return out;
      }
      FileShareID mId;
      std::string mFilename;
      long long mFileLen;

    };

    struct StoCYouAreDownloading 
    {
      StoCYouAreDownloading(const File & f, const std::string & ip, const std::string & port, FileShareID id)
        : mf(f), mip(ip), mport(port), mID(id)
      {
      }
      StoCYouAreDownloading(const SPIncomingMessage& msg)
        : mf(
        msg->mData[0].GetString(), 
        msg->mData[1].mLongLong)
        , mip(msg->mData[2].GetString())
        , mport(msg->mData[3].GetString())
        , mID(msg->mData[4].mInt)
      {
      }
      OutgoingMessage ToMsg()
      {
        std::string msgType
          = GetName(FileShareMsg::Type::eStoCYouAreDownloding);
        OutgoingMessage out(msgType.c_str());
        out.AddData(DataPtr(Data::Type::eCString, mf.mName.c_str(), mf.mName.size()));
        out.AddData(DataPtr(Data::Type::eLongLong, &mf.mLen, sizeof(long long)));
        out.AddData(DataPtr(Data::Type::eCString, mip.c_str(), mip.size()));
        out.AddData(DataPtr(Data::Type::eCString, mport.c_str(), mport.size()));
        out.AddData(DataPtr(Data::Type::eInt, &mID, sizeof(int)));
        return out;
      }
      File mf;
      std::string mip;
      std::string mport;
      FileShareID mID;
    };

    struct StoCYouAreUploading
    {
      StoCYouAreUploading(const File & f, const std::string & ip, const std::string & port, FileShareID id)
        : mf(f), mip(ip), mport(port), mID(id)
      {
      }
      StoCYouAreUploading(const SPIncomingMessage& msg)
        : mf(
        msg->mData[0].GetString(), 
        msg->mData[1].mLongLong)
        , mip(msg->mData[2].GetString())
        , mport(msg->mData[3].GetString())
        , mID(msg->mData[4].mInt)
      {
      }
      OutgoingMessage ToMsg()
      {
        std::string msgType
          = GetName(FileShareMsg::Type::eStoCYouAreUploading);
        OutgoingMessage out(msgType.c_str());
        out.AddData(DataPtr(Data::Type::eCString, mf.mName.c_str(), mf.mName.size()));
        out.AddData(DataPtr(Data::Type::eLongLong, &mf.mLen, sizeof(long long)));
        out.AddData(DataPtr(Data::Type::eCString, mip.c_str(), mip.size()));
        out.AddData(DataPtr(Data::Type::eCString, mport.c_str(), mport.size()));
        out.AddData(DataPtr(Data::Type::eInt, &mID, sizeof(int)));
        return out;
      }
      File mf;
      std::string mip;
      std::string mport;
      FileShareID mID;
    };

    struct CtoSDownloadDone
    {
      CtoSDownloadDone()
      {
      }
      CtoSDownloadDone(const SPIncomingMessage& msg)
      {
      }
      OutgoingMessage ToMsg()
      {
        std::string msgType
          = GetName(FileShareMsg::Type::eCtoSDownloadDone);
        OutgoingMessage out(msgType.c_str());
        return out;
      }
    };

    struct CtoSUploadDone 
    {
      CtoSUploadDone()
      {
      }
      CtoSUploadDone(const SPIncomingMessage& msg)
      {
      }
      OutgoingMessage ToMsg()
      {
        std::string msgType
          = GetName(FileShareMsg::Type::eCtoSUploadDone);
        OutgoingMessage out(msgType.c_str());
        return out;
      }
    };

  } // FileShareMsg
} // Tac
