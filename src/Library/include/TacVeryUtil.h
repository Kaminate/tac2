// Nathan Park
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once
#include "TacGLFWIncludes.h"
#include <fstream>
#include <string>
#include <vector>



namespace Tac
{
  TacDLLAPI void RemoveSpaces(std::string & str);
  TacDLLAPI void GetCurrentDir(std::string & str);
  TacDLLAPI bool GetPathToFileFromWorkingDir(
    const std::string & to, 
    std::string & out);
  TacDLLAPI bool ListFilesInDir(const std::string & dir,
    std::vector<std::string> & files,
    std::vector<long long> & fileSizes);
  TacDLLAPI std::string to_string(const void * ptr);
  /*
  template<enum T> // <-- totally doesn't work
  std::ifstream & operator >> (std::ifstream & ifs, T & t)
  {
    unsigned temp;
    ifs >> temp;
    t = (T) temp;
    return ifs;
  }
  */
}

