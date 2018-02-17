// Nathan Park
#include "TacException.h"
#include "TacVeryUtil.h"
#include <fstream>
#include <iostream>
#include <cstdio>
namespace Tac
{
  Exception::Exception( const std::string & str ) : mStr(str)
  {
    std::ofstream ofs("tacexception.txt",
      std::ofstream::out | std::ofstream::trunc);

    ofs << str << std::endl;
    std::cout << str << std::endl;
    fprintf(stderr, "%s", str.c_str());
    OutputDebugString(str.c_str());
  }

  const char * Exception::what() const 
  {
    return mStr.c_str();
  }

  void ThrowExceptionAux(
    const std::string & str, 
    const std::string & line, 
    const std::string & file, 
    const std::string & fn)
  {
    throw Exception(
      "Exception in file " + file 
      + " on line " + line 
      + " in function " + fn 
      + ": " + str);
  }
}