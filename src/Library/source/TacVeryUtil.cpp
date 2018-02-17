// Nathan Park
#include "TacVeryUtil.h"

#include <algorithm>
#include <windows.h>
//#define UNICODE
#include <Shlwapi.h> // pathrelativepathto
#pragma comment(lib,"Shlwapi.lib") // ya rly
#include <strsafe.h>

#include <strsafe.h> // StringCchCopy
#include <sstream>
namespace Tac
{
  void RemoveSpaces(std::string & str)
  {
    str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
  }


void GetCurrentDir(std::string & str)
{
  char buffer[200];
  GetCurrentDirectory(sizeof(buffer), buffer);
  str = std::string(buffer);
}

void FindFilesByExt(
  const std::string& dir_name,
  const std::string& extension,
  std::vector<std::string> & files )
{
  WIN32_FIND_DATA ffd;
  TCHAR szDir[MAX_PATH];
  StringCchCopy(szDir, MAX_PATH, dir_name.c_str());
  StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

  HANDLE hFind = FindFirstFile(szDir, &ffd);
  while (hFind != INVALID_HANDLE_VALUE)
  {
    if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
      std::string fileName(ffd.cFileName);
      if (fileName.size() > extension.size())
      {
        std::string fileEnd = fileName.substr(fileName.size() - extension.size());
        if (extension == fileEnd)
        {
          files.push_back(dir_name + "/" + fileName);
        }
      }
    }
    if (!FindNextFile(hFind, &ffd))
    {
      FindClose(hFind);
      hFind = INVALID_HANDLE_VALUE;
    }
  }
}



  bool GetPathToFileFromWorkingDir( 
    const std::string & to, std::string & out )
  {
    //WCHAR buf[MAX_PATH];
    char buf[MAX_PATH];
    std::string dir;
    GetCurrentDir(dir);

    if (PathRelativePathTo(
      buf, 
      dir.c_str(), FILE_ATTRIBUTE_DIRECTORY,
      to.c_str(), FILE_ATTRIBUTE_NORMAL))
    {
      out = std::string(buf);
      return true;
    }
    return false;
  }

  TacDLLAPI bool ListFilesInDir( 
    const std::string & dir, 
    std::vector<std::string> & files,
    std::vector<long long> & fileSizes)
  {
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;
    TCHAR szDir[MAX_PATH];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;

    // If the directory is not specified as a command-line argument,
    // print usage.

    // Check that the input path plus 3 is not longer than MAX_PATH.
    // Three characters are for the "\*" plus NULL appended below.
    if (dir.length() + 3 > MAX_PATH)
      return false;

    // Prepare string for use with FindFile functions.  First, copy the
    // string to a buffer, then append '\*' to the directory name.

    StringCchCopy(szDir, MAX_PATH, dir.c_str());
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    // Find the first file in the directory.

    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) 
    {
      return false;
    } 

    // List all the files in the directory with some info about them.

    do
    {
      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
      }
      else
      {
        filesize.LowPart = ffd.nFileSizeLow;
        filesize.HighPart = ffd.nFileSizeHigh;

        std::string filename(ffd.cFileName);
        files.push_back(filename);
        fileSizes.push_back(filesize.QuadPart);
      }
    }
    while (FindNextFile(hFind, &ffd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) 
    {
      return false;
    }

    FindClose(hFind);
    return true;
  }


  TacDLLAPI std::string to_string(const void * ptr)
  {
    std::stringstream address;
    address << ptr;
    return address.str();
  }


} // Tac

