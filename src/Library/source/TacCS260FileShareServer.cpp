/*!
\author Nathan Park
\brief
*/
#include "TacCS260FileShareServer.h"
#include "TacCS260FileShareServerConnect.h"
#include "TacCS260FileShareServerError.h"
#include "TacCore.h"

namespace Tac
{
  TacDLLAPI FileShareServer::FileShareServer()
    : mStateMachine(this)
  {
    Connect();
  }

  TacDLLAPI FileShareServer::~FileShareServer()
  {

  }

  TacDLLAPI bool FileShareServer::Init()
  {
    return true; 
  }

  TacDLLAPI  void FileShareServer::Update( float dt )
  {
    mStateMachine.Update(dt);
  }

  void FileShareServer::Error( const std::string & errorMessage )
  {
    mStateMachine.SetNextState(SPFileShareServerState(
      new FileShareServerError(&mStateMachine, errorMessage)));
  }

  StateMachine<FileShareServer> & FileShareServer::GetStateMachine()
  {
    return mStateMachine;
  }

  void FileShareServer::Connect()
  {
    mStateMachine.SetNextState(
      SPFileShareServerState(new FileShareServerConnect(&mStateMachine)));
  }


} // Tac
