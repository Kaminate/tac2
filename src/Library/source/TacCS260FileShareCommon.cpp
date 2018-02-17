// Nathan Park
#include "TacCS260FileShareCommon.h"
#include <string> //getline
#include "TacErrorIf.h"
namespace Tac
{
  std::string gConfigFilepath("config.txt");

  namespace FileShareMsg
  {
    TacDLLAPI std::string GetName( Type messageType )
    {
      switch (messageType)
      {
      case FileShareMsg::eCtoSDownloadDone:     return "downdone_";
      case FileShareMsg::eCtoSUploadDone:       return "updone___";
      case FileShareMsg::eStoCAssignId:         return "assignid_";
      case FileShareMsg::eCtoSUDPPort:          return "udpport__";
      case FileShareMsg::eCtoSCanShare:         return "canshare_";
      case FileShareMsg::eStoCBeingShared:      return "isshared_";
      case FileShareMsg::eStoCClientDisconnect: return "clientdc_";
      case FileShareMsg::eCtoSRequestShared:    return "requpload";
      case FileShareMsg::eCtoSRequestDownload:  return "reqdownld";
      case FileShareMsg::eStoCYouAreDownloding: return "youdown__";
      case FileShareMsg::eStoCYouAreUploading:  return "youup____";
      case FileShareMsg::eStoCUpdateBufSize:    return "bufsize__";
      case FileShareMsg::eCount:
      default: ErrorIf(true, "fileshare msg name not registered");
      }
      return "";
    }

    TacDLLAPI Type GetType( const std::string & message )
    {
      for(unsigned i = 0; i < Type::eCount; ++i)
      {
        Type curType = (Type)i;
        if (GetName(curType).compare(message) == 0)
        {
          return curType;
        }
      }
      return Type::eCount;
    }

    CtoSCanShare::CtoSCanShare( const SPIncomingMessage & msg )
    {
      mFilename = msg->mData[0].GetString();
      mFileSize = msg->mData[1].mLongLong;
    }

    CtoSCanShare::CtoSCanShare( const std::string & filename, long long filesize ) 
      : mFilename(filename)
      , mFileSize(filesize)
    {

    }

    Tac::OutgoingMessage CtoSCanShare::ToMsg()
    {
      std::string msgType = GetName(FileShareMsg::Type::eCtoSCanShare);
      OutgoingMessage out(msgType.c_str());
      out.AddData(DataPtr(
        Data::Type::eCString, mFilename.c_str(), mFilename.size()));
      out.AddData(DataPtr(
        Data::Type::eLongLong, &mFileSize, sizeof(long long)));

      return out;
    }


    StoCBeingShared::StoCBeingShared( const SPIncomingMessage & msg )
    {
      mId = msg->mData[0].mInt;
      mFilename = msg->mData[1].GetString();
      mFileSize = msg->mData[2].mLongLong;
    }

    StoCBeingShared::StoCBeingShared( 
      FileShareID id,
      const std::string & filename, 
      long long filesize )
      : mId(id)
      , mFilename(filename)
      , mFileSize(filesize)
    {
    }

    Tac::OutgoingMessage StoCBeingShared::ToMsg()
    {
      std::string msgType = GetName(FileShareMsg::Type::eStoCBeingShared);
      OutgoingMessage out(msgType.c_str());
      out.AddData(DataPtr(
        Data::Type::eInt, &mId, sizeof(int)));
      out.AddData(DataPtr(
        Data::Type::eCString, mFilename.c_str(), mFilename.size()));
      out.AddData(DataPtr(
        Data::Type::eLongLong, &mFileSize, sizeof(long long)));

      return out;
    }


    /*
    MsgManagr::MsgManagr()
    {
    switch (messageType)
    {
    case FileShareMsg::eCtoSDownloadDone: return "downdone";
    case FileShareMsg::eCtoSUploadDone: return "updone";
    case FileShareMsg::eStoCAssignId: return "assignid";
    case FileShareMsg::eCtoSUDPPort: return "udpport";
    case FileShareMsg::eCtoSCanShare: return "canshare";
    case FileShareMsg::eStoCBeingShared: return "isshared";
    case FileShareMsg::eStoCClientDisconnect: return "clientdc";
    case FileShareMsg::eCtoSRequestShared: return "requpload";
    case FileShareMsg::eCtoSRequestDownload: return "reqdownld";
    case FileShareMsg::eStoCYouAreDownloding: return "youaredown";
    case FileShareMsg::eStoCYouAreUploading: return "youareup";
    case FileShareMsg::eCount:
    default: ErrorIf(true, "fileshare msg name not registered");
    }
    return "";

    }
    */

  } // namespace FileShareMsg


  /*

  FileTransfer::FileTransfer()
  : mTransferType(eNone)
  , mBytesTransferred(0)
  {

  }

  void FileTransfer::OpenBar( 
  Tweak & creator, 
  FileShareID from ,
  FileShareID to, 
  const File & transferring,
  float curTime)
  {
  this->mLastUpdateTime = curTime;

  ErrorIf(mBar, "Already has a bar");
  mBar = creator.Create();

  std::string name;
  name += FileTransfer::to_string(mTransferType);
  name += "_From_" + std::to_string(from);
  name += "_";
  name += "to_" + std::to_string(to);
  name += "_";
  name += transferring.mName;
  name += "_";
  name += FileLenToString(transferring.mLen);

  // can't long long into tweakbar
  //mBar->AddVar("bytesTransferred", "label='bytes Transfred'", TW_TYPE_INT )

  mBar->Init(name);
  }

  std::string FileTransfer::to_string( Type transferType )
  {
  switch (transferType)
  {
  case Tac::FileTransfer::eNone:
  return "None";
  break;
  case Tac::FileTransfer::eIncoming:
  return "Incoming";
  break;
  case Tac::FileTransfer::eOutgoing:
  return "Outgoing";
  break;
  default:
  ErrorIf(true, "?");
  return "";
  }
  }

  */

  File::File( std::string name, long long len ) : mName(name), mLen(len)
  {

  }

  bool File::operator<( const File & rhs ) const
  {
    std::string namelen = mName + " " + std::to_string(mLen);
    std::string rhsnamelen = rhs.mName + " " + std::to_string(rhs.mLen);
    return namelen < rhsnamelen;
  }

  std::string FileLenToString( long long lenInBytes )
  {
    std::string desc;
    std::string prefix;
    if (lenInBytes> 1024)
    {
      lenInBytes /= 1024;
      if (lenInBytes > 1024)
      {
        lenInBytes /= 1024;
        prefix = "mega";
      }
      else
      {
        prefix = "kilo";
      }
    }
    desc += std::to_string(lenInBytes);
    desc += " ";
    desc += prefix;
    desc += "byte";
    if (lenInBytes != 1)
      desc += "s";
    return desc;
  }

} // namespace Tac
