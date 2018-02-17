/*!
\author Nathan Park
\brief
*/

#pragma once

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#include "TacSystemApplication.h"
#include "TacTweak.h"
#include "TacMessageUtil.h"
#include "TacStateMachine.h"

#include <thread>
#include <mutex>
#include <list>


namespace Tac
{
  class FileShareServer : public MiniApp
  {
  public:
    TacDLLAPI FileShareServer();
    TacDLLAPI ~FileShareServer();
    TacDLLAPI bool Init();
    TacDLLAPI virtual void Update(float dt);

    void Error(const std::string & errorMessage);
    void Connect();
    StateMachine<FileShareServer> & GetStateMachine();

  private:
    StateMachine<FileShareServer> mStateMachine;
  };

  typedef State<FileShareServer> FileShareServerState;
  DefineSharedPtr(FileShareServerState);

} // Tac
