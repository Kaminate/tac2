/*!
\author Nathan Park
*/
#pragma once

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#include <iostream>
#include <typeinfo>

namespace Tac
{
  // crtp
  template<typename T>
  class Yeller
  {
  public:
    Yeller() : mTName(typeid(T).name())
    {
      static unsigned count = 0;
      std::cout << mTName << "() count: " << ++count << std::endl;
    }
    ~Yeller()
    {
      static unsigned count = 0;
      std::cout << "~" << mTName << "() count: " << ++count << std::endl;
    }
    // copy ctor
    Yeller(const Yeller&rhs)
    {
      static unsigned count = 0;
      std::cout 
        << mTName 
        << "( const " 
        << mTName 
        << "& rhs ) count: " 
        << ++count 
        << std::endl;
    }
    Yeller & operator = (const Yeller & rhs)
    {
      static unsigned count = 0;
      std::cout 
        << mTName 
        << "& operator = ( const " 
        << mTName 
        << "& rhs ) count: " 
        << ++count
        << std::endl;
    }

  private:
    const char* mTName;
  };
} // Tac

