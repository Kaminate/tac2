/*!
\author Nathan Park
\brief
*/

#include "TacCS260AsteroidsServerError.h"
#include "TacCore.h"
#include "TacMathUtil.h"

namespace Tac
{
  namespace Asteroids
  {
    ServerError::ServerError( 
      StateMachine<Server> * sm, 
      const std::string & errMsg)
      : State<Server>(sm)
      , mErrMsg(errMsg)
    {
      mPos.x = float( rand() % 400);
      mPos.y = float( rand() % 300);

      mVel.x = float( rand() % 100 - 50);
      mVel.y = float( rand() % 100 - 50);

#ifdef TACTWEAK

      Server * serv = Get();
      AppSystem * app = serv->GetOwner();
      Core * engine = app->GetCore();

      Tweak & coreTweak = engine->GetTweak();
      mErrorBar = coreTweak.Create("error");
      mErrorBar->AddComment("error", mErrMsg);
      mErrorBar->AddButton("button", "Try Again?", ServerConnect, this);
      mErrorBar->SetPosition((int) mPos.x, (int) mPos.y);
#endif
    }

    void ServerError::Update( float dt )
    {
      mPos += mVel * dt;
      if (mPos.x > 400 || mPos.x < 0)
        mVel.x *= -1;
      if (mPos.y > 300 || mPos.y < 0)
        mVel.y *= -1;
      Clamp(mPos.x, 0.0f, 400.0f);
      Clamp(mPos.y, 0.0f, 300.0f);
#ifdef TACTWEAK

      mErrorBar->SetPosition((int) mPos.x, (int) mPos.y);
#endif

    }
#ifdef TACTWEAK

    void TW_CALL ServerError::ServerConnect( void * data )
    {
      ServerError * err = (ServerError*)data;
      Server * base = err->Get();
      base->Connect();
    }
#endif

  }
} // Tac
