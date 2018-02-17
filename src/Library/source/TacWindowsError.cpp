#include "TacWindowsError.h"
#include <windows.h>
namespace Tac
{
  std::string GetLastWindowsError()
  {
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
      return "No error message has been recorded";

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER 
      | FORMAT_MESSAGE_FROM_SYSTEM 
      | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, 
      errorMessageID, 
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
      (LPSTR)&messageBuffer, 
      0, 
      NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
  }
}
