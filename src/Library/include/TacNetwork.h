/*
Nathan Park
Copyright (C) 2014 DigiPen Institue of Technology
*/

#pragma once

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#include "TacGLFWIncludes.h"

#include <WinSock2.h> // winsock functions, structures, and definitions
#include <WS2tcpip.h> // includes newer functs/structs to retrieve ip addrs
#include <IPHlpApi.h> // needed if ip helper apis used (inc after winsock2.h)

#pragma comment(lib, "Ws2_32.lib") // winsock library

#include "TacSharedPtr.h"
#include <string>

/*
#define DEFAULT_PORT "2718" // e = 2.718...
#define DEFAULT_BUFLEN 512
*/

namespace Tac
{
  class Winsock
  {
  public:
    Winsock();
    ~Winsock();
    WSAData mWSAData;
  private:
    int mResult;
  };

  class AddrInfo
  {
  public:
    AddrInfo(
      PCSTR pNodeName, 
      PCSTR pServiceName, 
      const ADDRINFOA * pHints);
    ~AddrInfo();
    bool Ok() const;
    // returns nullptr if error is not a getaddrinfo-related error 
    const char * GetSpecificError();
    addrinfo * operator -> ();
    const addrinfo * operator ->() const;
    addrinfo * GetInfo();
    int GetResult();

  private:
    int mResult;
    addrinfo * mInfo;
  };

  class Socket
  {
  public:
    Socket();
    Socket(int addressFamily, int sockType, int protocol);
    Socket(addrinfo * info);
    Socket(SOCKET socket);
    Socket(const AddrInfo & info);
    ~Socket();

    bool Ok() const;
    bool SetBlocking(bool blocking);
    bool Listen();
    bool Bind(const AddrInfo & info);
    
              // port is the port of client-server connection, not udp port 
    bool GetPeerAddress(
      std::string & ip,
      std::string & port,
      std::string & errormsg);

    SOCKET mSocket;

    void CreateSocket(int addressFamily, int sockType, int protocol);
  private:
    
  };
  DefineSharedPtr(Socket);

std::string GetWSAErrorString(int iResult);
void PrintWSAErrorString(int iResult);


#define PrintWSAError(desc) {\
  printf("Network error: %d %s, ", WSAGetLastError(), desc);\
  PrintWSAErrorString(WSAGetLastError());\
  printf("\n");}

#define PrintWSAErrorIf(pred, desc) if (pred) { PrintWSAError(desc); }
#define PrintWSAErrorElse(desc) else PrintWSAError(desc);
#define PrintWSAErrorReturn(desc) {PrintWSAError(desc); return;}
#define PrintWSAErrorIfReturn(pred, desc) if (pred) PrintWSAErrorReturn(desc)

} // Tac
