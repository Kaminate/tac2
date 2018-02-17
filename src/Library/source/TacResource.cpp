/*!
\author Nathan Park
*/
#include "TacResource.h"
#include "TacException.h"
#include "windows.h"
#include <TacErrorIf.h>

namespace Tac
{
  Resource::~Resource()
  {

  }

  time_t Resource::GetLastModified(const std::string & path)const
  {
    HANDLE file = CreateFile(
      path.c_str(),
      0, // only want last accessed, do not need read or write
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      nullptr);

    if (INVALID_HANDLE_VALUE == file)
      throw Exception("Resource failed to open file " + path);

    BY_HANDLE_FILE_INFORMATION fileInfo;
    BOOL success = GetFileInformationByHandle(file, &fileInfo);
    if (!success)
      throw Exception("GetFileInformationByHandle failed");

    SYSTEMTIME lastWrite;
    success 
      = FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &lastWrite);

    if (!success)
      throw Exception("FileTimeToSystemTime failed");

    lastWrite;
    tm tempTm;
    tempTm.tm_sec = lastWrite.wSecond;
    tempTm.tm_min = lastWrite.wMinute;
    tempTm.tm_hour = lastWrite.wHour;
    tempTm.tm_mday = lastWrite.wDay;
    tempTm.tm_mon = lastWrite.wMonth - 1;
    tempTm.tm_year = lastWrite.wYear - 1900;
    tempTm.tm_wday; // not needed for mktime
    tempTm.tm_yday; // not needed for mktime
    tempTm.tm_isdst = -1; // meaning who gives a f*ck

    time_t modified = mktime(&tempTm);
    ErrorIf(modified == -1, "Calandar time cannot be represented");

    success = CloseHandle(file);
    if (!success)
      throw Exception("Failed to close handle " + path);

    return modified;
  }

  bool Resource::IsOutOfDate() const
  {
    for (unsigned i = 0; i < mPaths.size(); ++i)
    {
      const std::string & path = mPaths[i];
      time_t oldModified = mLastModified[i];
      time_t newModified = GetLastModified(path);
      double seconds = difftime(newModified, oldModified);
      if (seconds != 0)
      {
        return true;
      }
    }
    return false;
  }

  // return errors
  std::string Resource::TryReload()
  {
    std::string errs = ReloadAux();
    if( errs.empty() )
    {
      UpdateFileTimes();
      Subject::Notify(ResourceEvent::Reload);
    }
    return errs;
  }

  void Resource::AddPathToWatch( const std::string & path )
  {
    mPaths.push_back(path);
    mLastModified.push_back(GetLastModified(path));
  }

  void Resource::UpdateFileTimes()
  {
    for(unsigned i = 0 ; i < mLastModified.size(); ++i)
      mLastModified[i] = GetLastModified(mPaths[i]);
  }

} // Tac
