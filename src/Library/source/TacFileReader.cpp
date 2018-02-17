#include "TacFileReader.h"
#include "TacWindowsError.h"
#include "TacException.h"
#include <fstream>

namespace Tac
{
  FileReader::FileReader( const std::string & filename )
  {
    std::ifstream myfile(filename.c_str(), std::ifstream::binary);
    bool success = myfile.is_open();
    if (success)
    {
      myfile.seekg(0, myfile.end);
      unsigned length = (unsigned) myfile.tellg();
      myfile.seekg(0, myfile.beg);
      mSource.resize(length);
      myfile.read(&mSource[0], mSource.size());
      myfile.close();
    }
    else
      throw Exception(GetLastWindowsError());
  }

  const char * FileReader::GetSource() const
  {
    return mSource.c_str();
  }
}
