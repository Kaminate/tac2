/*!
\author Nathan Park
\brief
*/
#include "TacCS260FileShareClient.h"
#include "TacCore.h"

#include "TacCS260FileShareClientConnect.h"
#include "TacCS260FileShareClientError.h"

namespace Tac
{
  TacDLLAPI FileShareClient::FileShareClient()
    : mStateMachine(this)
  {
    
  }

  TacDLLAPI FileShareClient::~FileShareClient()
  {

  }

  TacDLLAPI bool FileShareClient::Init()
  {
    Connect();
    return true;
  }

  void FileShareClient::Error( const std::string & errorMessage )
  {
    mStateMachine.SetNextState(SPFileShareClientState(
      new FileShareClientError(errorMessage)));
  }

  TacDLLAPI  void FileShareClient::Update( float dt )
  {
    mStateMachine.Update(dt);
  }

  StateMachine<FileShareClient> & FileShareClient::GetStateMachine()
  {
    return mStateMachine;
  }

  void FileShareClient::Connect()
  {
    mStateMachine.SetNextState(
      SPFileShareClientState(new FileShareClientConnect()));
  }

} // Tac
