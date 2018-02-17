/*!
\author Nathan Park
\brief
*/
#include "TacCore.h"
#include "TacCS260AsteroidsServer.h"
#include "TacCS260AsteroidsServerError.h"
#include "TacCS260AsteroidsServerRunning.h"

namespace Tac
{
  namespace Asteroids 
  {
    Server::Server(AppSystem * sys)
      : MiniApp(sys)
      , mStateMachine(this)
    {
      Connect();
    }

    void Server::Update( float dt )
    {
      mStateMachine.Update(dt);
    }

    void Server::Error( const std::string & errorMessage )
    {
      mStateMachine.SetNextState(SPServerState(
        new ServerError(&mStateMachine, errorMessage)));
    }

    void Server::Connect()
    {
      std::ifstream ifs(gConfigFilepath);
      if (!ifs.is_open())
      {
        Error("Failed to open " + gConfigFilepath);
      }
      std::string ip;
      std::string port;
      ifs >> ip;
      ifs >> port;

      addrinfo hints;
      ZeroMemory(&hints, sizeof(hints));
      hints.ai_family = AF_INET; // ipv4
      hints.ai_socktype = SOCK_STREAM; // connection-oriented
      hints.ai_protocol = IPPROTO_TCP; // tcp
      hints.ai_flags = AI_PASSIVE; // use my ip

      // If the pNodeName parameter contains an empty string, 
      // all registered addresses on the local computer are returned.
      AddrInfo info(nullptr, port.c_str(), &hints);
      if (!info.Ok())
      {
        Error(info.GetSpecificError());
        return;
      }

      SPSocket tempSock(new Socket(info));

      if (!tempSock->Ok())
      {
        Error("Socket failed");
        return;
      }

      if (!tempSock->Bind(info))
      {
        Error("Bind failed");
        return;
      }

      if (!tempSock->Listen())
      {
        Error("Listen failed");
        return;
      }

      if (!tempSock->SetBlocking(false))
      {
        Error("Nonblock failed");
        return;
      }

      mStateMachine.SetNextState(
        SPServerState(new ServerRunning(&mStateMachine, tempSock)));

    }
  } //asteroids
} // Tac
