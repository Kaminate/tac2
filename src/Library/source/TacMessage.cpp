/*
Nathan Park
Copyright (C) 2014 DigiPen Institue of Technology
*/
#include "TacMessage.h"
#include "TacErrorIf.h"
namespace Tac
{
  DataPtr::DataPtr(Data::Type type, const void * data, unsigned size)
    : mType(type), mPointer(data), mSize(size)
  {
  }

  DataPtr::DataPtr( int * i )
    : mType(Data::Type::eInt)
    , mPointer(i)
    , mSize(sizeof(int))
  {

  }

  DataPtr::DataPtr( const std::string & str )
    : mType(Data::Type::eCString)
    , mPointer(str.c_str())
    , mSize(str.length())
  {

  }

  DataPtr::DataPtr( bool * val )
    : mType(Data::Type::eBool)
    , mPointer(val)
    , mSize(sizeof(bool))
  {

  }

  //Data::Data(Type type) : mType(type) {}

  OutgoingMessage::OutgoingMessage(const char * name) 
  {
    unsigned size = sizeof(MessageName);
    strncpy_s(mName, name, size);
    mName[size - 1] = '\0';
  }

  void OutgoingMessage::AddData(const DataPtr & data)
  {
    ErrorIf(mData.size() > MAX_MESSAGE_DATA, "Msg has too much data");
    mData.push_back(data);
  }

  void IncomingMessage::AddData(const Data & data)
  {
    ErrorIf(mData.size() > MAX_MESSAGE_DATA, "Msg has too much data");
    mData.push_back(data);
  }
  //const std::vector<Data> & IncomingMessage::GetData() const {return mData;}

  OutgoingMessage::OutgoingMessage(const IncomingMessage & msg)
  {
    strncpy_s(mName, msg.mName, sizeof (MessageName));
    for (const Data & iData : msg.mData)
    {
      DataPtr data(iData.mType, iData.GetPointer(), iData.mSize);
      AddData(data);
    }
  }

} // tac

