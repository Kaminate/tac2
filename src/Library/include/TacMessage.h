/*
Nathan Park
*/
#pragma once
#include "TacSharedPtr.h"
#include <memory>
#include <vector>
#include <iostream>
namespace Tac
{
  class Data
  {
  public:
    enum Type {eFloat, eBool, eInt, eUnsigned, eCString, eLongLong};

    Type mType;
    unsigned mSize;

    // data
    union // anon
    {
      float mFloat;
      bool mBool;
      int mInt;
      unsigned mUnsigned;
      long long mLongLong;
    };
    const void * GetPointer() const
    {
      if (mType == eCString)
      {
        if (mSize > 0)
          return &mCString[0];
        else
          return nullptr; // <-- unneccessary? (should have 1 char( '\0')
      }
      else
        // ok to do addr of float, because unioned
        return reinterpret_cast<const void *>(&mFloat);
    }

    // Because it's WAY too easy to use the wrong constructor
    // std::string(&mCString[0]) which goes until it finds a null
    // (we don't store the null, it goes until it finds a garbage null)
    std::string GetString() const
    {
      return std::string((char *) &mCString[0], mSize);
    }
    std::vector<char> mCString;
  };


  class DataPtr
  {
  public:
    // "size" is as long as the number of characters between the 
    // beginning of the string and the terminating null character 
    // (without including the terminating null character itself).
    //
    // tl;dr: 
    //   when making a string from a DataPtr or a Data,
    //   use the ctor std::string(char*, size);
    DataPtr(Data::Type type, const void * data, unsigned size);
    DataPtr(int * i);
    DataPtr(bool * val);
    DataPtr(const std::string & str);

    Data::Type mType;
    unsigned mSize;
    const void * mPointer;
  };


#define MAX_MESSAGE_DATA 100
  typedef char MessageName[10];


  class IncomingMessage
  {
  public:
    MessageName mName;
    void AddData(const Data & data);
    unsigned mNumData; // needs to exist outside of the std::vector
    std::vector<Data> mData;
  };
  DefineSharedPtr(IncomingMessage);

  // note: since this uses pointers
  //   it is only valid while the thing it points at is valid!
  class OutgoingMessage
  {
  public:
    OutgoingMessage(const char * name);
    OutgoingMessage(const IncomingMessage & msg);

    void AddData(const DataPtr & data);
    const char * GetName() const {return mName;}
    const std::vector<DataPtr> & GetData() const { return mData;}
  private:
    MessageName mName;
    std::vector<DataPtr> mData;
  };
} // Tac