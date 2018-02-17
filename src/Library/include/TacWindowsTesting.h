// Nathan Park
#pragma once

#ifdef HowToCreateAFileAndCheckErrorMessages

#include <fstream>
#include <windows.h>
#include <comdef.h>
#include <strsafe.h>
#include <stdio.h>


void DisplayError(LPTSTR lpszFunction) 
  // Routine Description:
  // Retrieve and output the system error message for the last-error code
{ 
  LPVOID lpMsgBuf;
  LPVOID lpDisplayBuf;
  DWORD dw = GetLastError(); 

  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    dw,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR) &lpMsgBuf,
    0, 
    NULL );

  lpDisplayBuf = 
    (LPVOID)LocalAlloc( LMEM_ZEROINIT, 
    ( lstrlen((LPCTSTR)lpMsgBuf)
    + lstrlen((LPCTSTR)lpszFunction)
    + 40) // account for format string
    * sizeof(TCHAR) );

  if (FAILED( StringCchPrintf((LPTSTR)lpDisplayBuf, 
    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
    TEXT("%s failed with error code %d as follows:\n%s"), 
    lpszFunction, 
    dw, 
    lpMsgBuf)))
  {
    printf("FATAL ERROR: Unable to output error code.\n");
  }
  std::string error((LPCTSTR)lpDisplayBuf);
  error.append("!");
  printf(TEXT("ERROR: %s\n"), (LPCTSTR)lpDisplayBuf);

  LocalFree(lpMsgBuf);
  LocalFree(lpDisplayBuf);

}


int main()
{
  auto hFile = CreateFile(
    "foo.txt",    // name of the write
    GENERIC_WRITE,          // open for writing
    0,                      // do not share
    NULL,                   // default security
    CREATE_NEW,             // create new file only
    FILE_ATTRIBUTE_NORMAL,  // normal file
    NULL);                  // no attr. template

  if (hFile == INVALID_HANDLE_VALUE) 
  { 
    DisplayError(TEXT("CreateFile"));
    return -1;
  }
  return 0;

}

#endif
