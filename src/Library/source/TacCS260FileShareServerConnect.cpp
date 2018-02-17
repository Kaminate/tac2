///*!
//\author Nathan Park
//\brief
//*/
//
//#include "TacCS260FileShareServerConnect.h"
//#include <TacCS260FileShareServerRunning.h>
//#include <TacCS260FileShareCommon.h>
//
//namespace Tac
//{
//
//  void FileShareServerConnect::Init()
//  {
//    std::ifstream ifs(gConfigFilepath);
//    if (!ifs.is_open())
//    {
//      Get()->Error("Failed to open " + gConfigFilepath);
//      return;
//    }
//    std::string ip;
//    std::string port;
//    ifs >> ip;
//    ifs >> port;
//
//    addrinfo hints;
//    ZeroMemory(&hints, sizeof(hints));
//    hints.ai_family = AF_INET; // ipv4
//    hints.ai_socktype = SOCK_STREAM; // connection-oriented
//    hints.ai_protocol = IPPROTO_TCP; // tcp
//    hints.ai_flags = AI_PASSIVE; // use my ip
//
//    // If the pNodeName parameter contains an empty string, 
//    // all registered addresses on the local computer are returned.
//    AddrInfo info(nullptr, port.c_str(), &hints);
//    if (!info.Ok()){
//      Get()->Error(info.GetSpecificError());
//      return;
//    }
//
//    SPSocket tempSock(new Socket(info));
//
//    if (!tempSock->Ok()){
//      Get()->Error("Socket failed");
//      return;
//    }
//
//    if (!tempSock->Bind(info)){
//      Get()->Error("Bind failed");
//      return;
//    }
//
//    if (!tempSock->Listen()){
//      Get()->Error("Listen failed");
//      return;
//    }
//
//    if (!tempSock->SetBlocking(false)){
//      Get()->Error("Nonblock failed");
//      return;
//    }
//
//    Get()->GetStateMachine().SetNextState(
//      SPFileShareServerState(new FileShareServerRunning(tempSock)));
//  }
//
//  void FileShareServerConnect::Update( float dt )
//  {
//
//  }
//
//} // Tac
