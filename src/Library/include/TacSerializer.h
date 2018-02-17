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
#include <fstream> // ifstream, ofstream
#include <string>

namespace Tac
{
  template<typename T>
  void SaveFormatted(
    std::ostream & ofs, 
    int numSpaces, 
    const std::string & valName, 
    const T&val)
  {
    while(numSpaces--)
      ofs << " ";
    ofs << valName << " " << val << std::endl;
  }
  class Saver
  {
  public:
    Saver(const std::string & filename);
    bool Ok() const;
    //void CloseFile();
    void Tab();
    void DeTab();
    template <typename T>
    void SaveFormatted(const std::string & valName, const T & val)
    {
      Tac::SaveFormatted(mFile, mSpacesPerTab * mNumTabs, valName, val);
      //Tac::SaveFormatted(
      // std::cout,
      // mSpacesPerTab * mNumTabs, 
      // valName, 
      // val);
    }

  private:
    unsigned mSpacesPerTab;
    unsigned mNumTabs;
    std::ofstream mFile;
  };

  class Loader
  {
  public:
    Loader(const std::string & filename);
    template <typename T>
    void Load(T & val)
    {
      mFile >> mTemp >> val;
    }
    void LoadEnum(unsigned & val)
    {
      mFile >> mTemp >> val;
    }
    bool Ok() const;
  private:
    std::string mTemp;
    std::ifstream mFile;
  };
} // Tac
