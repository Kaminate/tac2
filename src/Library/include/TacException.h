/*!
\author Nathan Park

\par Project: Tac Engine

\brief

*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once

#include <exception>
#include <string>

namespace Tac
{
  class Exception : public std::exception
  {
  public:
    TacDLLAPI Exception(const std::string & str);
    TacDLLAPI virtual const char * what() const override;
  private:
    std::string mStr;
  };

  void ThrowExceptionAux(
    const std::string & str, 
    const std::string & line, 
    const std::string & file, 
    const std::string & fn);
#define ThrowException(str) \
  ThrowExceptionAux(str, __LINE__, __FILE__, __FUNCTION__);

} // Tac
