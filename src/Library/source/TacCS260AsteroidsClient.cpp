/*!
\author Nathan Park
\brief
*/
#include "TacCS260AsteroidsClient.h"
#include "TacCore.h"

#include "TacCS260AsteroidsClientConnect.h"
#include "TacCS260AsteroidsClientError.h"

namespace Tac
{
  namespace Asteroids
  {
    Client::Client(AppSystem * ap) 
      : MiniApp(ap)
      , mStateMachine(this)
    {
      Connect();
    }

    void Client::Error( const std::string & errorMessage )
    {
      mStateMachine.SetNextState(SPClientState(
        new ClientError(&mStateMachine, errorMessage)));
    }

    void Client::Update( float dt )
    {
      mStateMachine.Update(dt);
    }

    void Client::Connect()
    {
      mStateMachine.SetNextState(
        SPClientState(new ClientConnect(&mStateMachine)));
    }
  }

} // Tac
