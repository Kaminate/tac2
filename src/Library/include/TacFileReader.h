#pragma once

#include <memory>
#include <string>

namespace Tac
{
  class FileReader
  {
  public:
    FileReader(const std::string & filename);
    const char * GetSource() const;
    size_t GetLen()const;
  private:
    std::string mSource;
  };

}

