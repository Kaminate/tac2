/*
Nathan Park
Copyright (C) 2014 DigiPen Institue of Technology
*/
#pragma once

#include "TacMessage.h"
#include "TacNetwork.h"
namespace Tac
{

  // on socket failure, calls sock.reset()
  // pushes onto the back of messages
	struct MessageBeingReceived
	{
		enum State { 
			ReceivingMessageName,
			ReceivingNumData,
			ReceivingMessageDatatype,
			ReceivingMessageSize,
			ReceivingMessageData,
		    ReceivingDone,};

		MessageBeingReceived()
			: mState(ReceivingMessageName)
			, mCurStateBytesReceived(0)
      , mMessage(new IncomingMessage())
		{
		}

		unsigned mCurStateBytesReceived;
		State mState;
		std::shared_ptr<IncomingMessage> mMessage;
	};


  struct Connection
  {
    SPSocket mSocket;
    MessageBeingReceived mInTransit;
  };

	// on socket failure, calls sock.reset()
  void ReceiveMessages(
    std::shared_ptr<Socket> & sock,
    std::vector<std::shared_ptr<IncomingMessage> > & messages,
    MessageBeingReceived & inTransit,
    char * buffer,
    unsigned bufferLen);
  

	// on socket failure, calls sock.reset()
	void SendMsg(std::shared_ptr<Socket> & sock, const OutgoingMessage & message);

} // Tac