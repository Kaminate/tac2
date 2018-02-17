/*
Nathan Park
Copyright (C) 2014 DigiPen Institue of Technology
*/

#include "TacNetwork.h"
#include "TacException.h"

#include <stdio.h>
#include <iostream>

namespace Tac
{
  void PrintWSAErrorString(int iResult)
  {
    /*
    wchar_t * s = nullptr;
    FormatMessageW(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr,
      WSAGetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPWSTR)&s, 
      0, 
      nullptr);
    printf("%S", s);
    LocalFree(s);
    */
    std::cout << GetWSAErrorString(iResult);
  }

  std::string GetWSAErrorString( int iResult )
  {
    wchar_t * s = nullptr;
    FormatMessageW(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr,
      iResult,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPWSTR)&s, 
      0, 
      nullptr);
    printf("%S", s);

    std::wstring werrorstr(s);
    std::string errorStr(werrorstr.begin(), werrorstr.end());
    LocalFree(s);
    return errorStr;
  }

  Winsock::Winsock()
  {
    mResult = WSAStartup(MAKEWORD(2,2), &mWSAData);
    bool success = mResult == 0;
    if (!success)
      throw Exception("wsastartup failed");
  }

  Winsock::~Winsock()
  {
    WSACleanup();
  }

  Socket::Socket() : mSocket(INVALID_SOCKET)
  {
  }

  Socket::Socket(int addressFamily, int sockType, int protocol)
  {
    CreateSocket(addressFamily, sockType, protocol);
  }

  Socket::Socket(addrinfo * info)
  {
    CreateSocket(info->ai_family, info->ai_socktype, info->ai_protocol);
  }

  Socket::Socket(SOCKET socket) : mSocket(socket)
  {

  }

  Socket::Socket( const AddrInfo & info )
  {
    CreateSocket(info->ai_family, info->ai_socktype, info->ai_protocol);
  }

  Socket::~Socket()
  {
    if (Ok())
    {
      std::cout << "closing socket..." << std::endl;
      closesocket(mSocket);
      mSocket = 0;
    }
  }

  void Socket::CreateSocket(int addressFamily, int sockType, int protocol)
  {
    mSocket = socket(addressFamily, sockType, protocol);
  }

  bool Socket::Ok() const
  {
    return mSocket != INVALID_SOCKET;
  }

  // set false to enable non-blocking mode
  bool Socket::SetBlocking( bool blocking )
  {
    unsigned long mode = blocking ? 0 : 1;
    return SOCKET_ERROR != ioctlsocket(mSocket, FIONBIO, &mode);
  }

  bool Socket::Listen()
  {
    return listen(mSocket, SOMAXCONN) != SOCKET_ERROR;
  }

  bool Socket::Bind( const AddrInfo & info )
  {
    return SOCKET_ERROR != bind(
      mSocket, 
      info->ai_addr, 
      (int) info->ai_addrlen);
  }

              // port is the port of client-server connection, not udp port 
  bool Socket::GetPeerAddress( 
    std::string & ip,
    std::string & port,
    std::string & errormsg)
  {
    sockaddr_in address;
    int len = sizeof(address);
    int iresult = getpeername(mSocket, (sockaddr*) &address, &len);
    if (iresult == SOCKET_ERROR)
    {
      errormsg = GetWSAErrorString(iresult);
      return false;
    }
    else
    {
      std::string tempIp(inet_ntoa(address.sin_addr));
      std::string tempPort(std::to_string((int)ntohs(address.sin_port)));
      
      

      char hostname[NI_MAXHOST];
      char servInfo[NI_MAXSERV];

      /*
      sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = ;
      */
      iresult = getnameinfo(
        (sockaddr*)&address, sizeof(address),//(sockaddr*)&addr, sizeof(addr), 
        hostname, sizeof(hostname), 
        servInfo, sizeof(servInfo), 
        NI_NUMERICHOST | NI_NUMERICSERV );

      if (iresult == SOCKET_ERROR)
      {
        errormsg = GetWSAErrorString(iresult);
        return false;
      }

      ip = std::string(hostname);
      port = std::string(servInfo);

      return true;
    }
  }

  AddrInfo::AddrInfo(
    PCSTR pNodeName, 
    PCSTR pServiceName, 
    const ADDRINFOA * pHints)
  {
    mResult = getaddrinfo(pNodeName, pServiceName, pHints, &mInfo);
  }

  addrinfo * AddrInfo::operator -> ()
  {
    return mInfo;
  }

  const addrinfo * AddrInfo::operator->() const
  {
    return mInfo;
  }

  bool AddrInfo::Ok() const
  {
    return mResult == 0;
  }

  AddrInfo::~AddrInfo() 
  {
    if (Ok())
      freeaddrinfo(mInfo);
  }

  addrinfo * AddrInfo::GetInfo()
  {
    return mInfo;
  }

  int AddrInfo::GetResult()
  {
    return mResult;
  }

  const char * AddrInfo::GetSpecificError()
  {
    return gai_strerror(mResult);
  }


} // Tac
