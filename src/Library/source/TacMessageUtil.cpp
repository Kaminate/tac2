/*
Nathan Park
Copyright (C) 2014 DigiPen Institue of Technology
*/

#include "TacMessage.h"
#include "TacMessageUtil.h"
#include <algorithm> // std::min

namespace Tac
{
  // when message is read in completely, pushes the message onto
  // receivedMsgs and resets message.
  // Continues reading to message until buffer is empty
  void ReadDataIntoMessage(
    unsigned bufferSize,
    char * buffer,
    std::vector<std::shared_ptr<IncomingMessage> > & receivedMsgs,
    MessageBeingReceived & message)
  {
    if (bufferSize == 0)
      return;

    char * placeToMemcpy = nullptr;
    unsigned placeToMemcpySizeInBytes;

    std::shared_ptr<IncomingMessage> & msg = message.mMessage;
    bool isCString = false;

    // state machine of sorts. Based on the state is which memory we copy to
    switch(message.mState)
    {
    case MessageBeingReceived::ReceivingMessageName:
      placeToMemcpy = reinterpret_cast<char *>(msg->mName);
      placeToMemcpySizeInBytes = sizeof(decltype(msg->mName));
      break;
    case MessageBeingReceived::ReceivingNumData:
      placeToMemcpy = reinterpret_cast<char *>(&msg->mNumData);
      placeToMemcpySizeInBytes = sizeof(decltype(msg->mNumData));
      break;
    case MessageBeingReceived::ReceivingMessageDatatype:
      placeToMemcpy = reinterpret_cast<char *>(&msg->mData.back().mType);
      placeToMemcpySizeInBytes = sizeof(decltype(msg->mData.back().mType));
      break;
    case MessageBeingReceived::ReceivingMessageSize:
      placeToMemcpy = reinterpret_cast<char *>(&msg->mData.back().mSize);
      placeToMemcpySizeInBytes = sizeof(decltype(msg->mData.back().mSize));
      break;
    case MessageBeingReceived::ReceivingMessageData:
      {
        Data & curData = msg->mData.back();
        switch (curData.mType)
        {
        case Data::Type::eFloat:
        case Data::Type::eBool: 
        case Data::Type::eInt:
        case Data::Type::eUnsigned:
        case Data::Type::eLongLong:
          placeToMemcpy = reinterpret_cast<char*>(&curData.mBool);
          break;

        case Data::Type::eCString:
          // special case, use nullptr as a flag to signal push back instead of memcpy
          placeToMemcpy = nullptr;
          isCString = true;
          break;
        default:
          std::cout << "oh shit data got corrupted fuck fuck fuck";
          break;
        }
        placeToMemcpySizeInBytes = curData.mSize;
        break;
      }
    case MessageBeingReceived::ReceivingDone:
      return;
    default:
      // got here because the data got corrupted and enum was out of range
      std::cout << "Something went very wrong. Crash soon." << std::endl;
      return;
    }

    // we may have more bytes in our buffer than the size of the variable we
    // haven't copied to yet
    unsigned remainingBytes = placeToMemcpySizeInBytes - message.mCurStateBytesReceived;
    unsigned bytesToMemcpy = std::min(remainingBytes, bufferSize);


    if (isCString) // special case, treat nullptr as a flag to push back instead of memcpy
    {
      Data & lastData = msg->mData.back();
      for (unsigned i = 0; i < bytesToMemcpy; ++i)
      {
        lastData.mCString.push_back(buffer[i]);
      }
    }
    else // if (placeToMemcpy)
    {
      // placeToMemcpy originally points to the start location of the variable
      // add to it how far we've already copied
      // to get where we should copy to now
      placeToMemcpy += message.mCurStateBytesReceived;
      memcpy(placeToMemcpy, buffer, bytesToMemcpy);
    }


    // Update the buffer:
    // we just copied "bytesToMemcpy" from our buffer into the variable location
    // but the buffer might hold more data than that.
    // so we subtract bytesToMemcpy from buffer so that buffer now holds uncopied memory
    bufferSize -= bytesToMemcpy;

    // make buffer point to memory that hasnt yet been copied over
    buffer += bytesToMemcpy;

    // since we just copied bytesToMemcpy into our variable,
    // remainingBytes now represents how many bytes of our variable 
    // hasn't been copied into
    remainingBytes -= bytesToMemcpy;

    // we just finished coping into our variable
    if (remainingBytes == 0)
    {
      // enum is in order, so +1 is the next state
      MessageBeingReceived::State nextState 
        = (MessageBeingReceived::State) (message.mState + 1);

      // special case: RecivingNumData may have 0 data, 
      // so we should skip the retrieval of data
      if (message.mState == MessageBeingReceived::ReceivingNumData)
        if (message.mMessage->mNumData == 0)
          nextState = MessageBeingReceived::ReceivingDone;

      // special case: ReceivingMessage has multiple pieces of data, 
      // so it may go to a previous step
      // to retreive the next data
      if (message.mState == MessageBeingReceived::ReceivingMessageData)
      {
        bool doneReadingInMsgData 
          = message.mMessage->mNumData == message.mMessage->mData.size();
        if (!doneReadingInMsgData)
        {
          // data retreival starts with the type
          nextState = MessageBeingReceived::ReceivingMessageDatatype;
        }
      }
      // this state is done being read in. Go to appropriate next state
      message.mState = nextState;

      // basically state.OnEnter()
      switch (message.mState)
      {
        // upon entering ReceivingMessageDataType, push back a data
      case MessageBeingReceived::ReceivingMessageDatatype:
        // note there are 2 ways of entering ReceivingMessageDataType
        // 1. Entering it forwards from ReceivingNumData
        // 2. Entering it backwards from ReceivingMessageData
        message.mMessage->mData.push_back(Data());
        break;
      case MessageBeingReceived::ReceivingDone:
        // just read in a message
        receivedMsgs.push_back(msg);

        // reset the incoming message
        message = MessageBeingReceived();
        break;
      default:
        break;
      }

      // 0 bytes have been read into the next state
      message.mCurStateBytesReceived = 0;
    }
    else // the current state's bytes haven't been copied over
    {
      message.mCurStateBytesReceived += bytesToMemcpy;
    }

    // read the remainder of the buffer into the message
    ReadDataIntoMessage(bufferSize, buffer,receivedMsgs, message);
  }


  // on socket failure, calls sock.reset()
  void SendMsg(std::shared_ptr<Socket> & sock, const OutgoingMessage & message)
  {
    if (!sock)
      return;
    // send the message name
    int iResult = send(
      sock->mSocket, 
      message.GetName(), 
      sizeof(MessageName), 
      0);
    if (iResult == INVALID_SOCKET) 
    {
      PrintWSAError("send failed");
      sock.reset(); 
      return;
    }

    // send the number of data
    unsigned numData = message.GetData().size();
    iResult = send(
      sock->mSocket, 
      reinterpret_cast<const char *>(&numData), 
      sizeof(numData), 
      0);
    if (iResult == INVALID_SOCKET) 
    {
      PrintWSAError("send failed");
      sock.reset(); 
      return;
    }

    // send each piece of data
    for (const DataPtr & iData : message.GetData())
    {
      // send the type of data
      iResult = send(
        sock->mSocket, 
        reinterpret_cast<const char*>(&iData.mType), 
        sizeof(Data::Type),0);
      if (iResult == INVALID_SOCKET) 
      {
        PrintWSAError("send failed");
        sock.reset(); 
        return;
      }

      // send the size of data
      iResult = send(
        sock->mSocket, 
        reinterpret_cast<const char*>(&iData.mSize), 
        sizeof(iData.mSize), 0);
      if (iResult == INVALID_SOCKET)
      {
        PrintWSAError("send failed");
        sock.reset(); 
        return;
      }

      // send the data itself
      iResult = send(
        sock->mSocket, 
        reinterpret_cast<const char*>(iData.mPointer), 
        iData.mSize, 
        0);
      if (iResult == INVALID_SOCKET) 
      {
        PrintWSAError("send failed");
        sock.reset(); 
        return;
      }
    }
  }

  // on socket failure, calls sock.reset()
  void ReceiveMessages(
    std::shared_ptr<Socket> & sock,
    std::vector<std::shared_ptr<IncomingMessage> > & messages,
    MessageBeingReceived & inTransit,
    char * buffer,
    unsigned bufferLen)
  {
    if (!sock)
      return;


    bool messageToReceive = true;


    // loop until done
    while ( 
      sock && 
      inTransit.mState != MessageBeingReceived::ReceivingDone && 
      messageToReceive)
    {
      int iResult = recv(sock->mSocket, buffer, bufferLen, 0);
      if (iResult == INVALID_SOCKET)
      {
        switch(WSAGetLastError())
        {
        case WSAEMSGSIZE:
          // not all of the data was read in. it was truncated
          ReadDataIntoMessage(bufferLen, buffer, messages, inTransit);
          break;
        case WSAEWOULDBLOCK: // no msg to receive

          // yo.
          // this is important:
          //   winsock can totally just be like
          //   here, recv the a packet from a send
          //   here, recv the beginning-half of another backet
          //   and then motherfucking WSAWOULDBLOCK without recv the end-half

          messageToReceive = false; // will exit the loop

          break;
        default: // some other error
          sock.reset();
          break;
        }
      }
      else if (iResult > 0) // iresult now contains the number of bytes read in
      {
        ReadDataIntoMessage(iResult, &buffer[0], messages, inTransit);
      }
      else if (iResult == 0) // connection gracefully closed
      {
        std::cout << "Server closed connection gracefully" << std::endl;
        sock.reset();
      }
    }
  } // ReceiveMessage

} // Tac