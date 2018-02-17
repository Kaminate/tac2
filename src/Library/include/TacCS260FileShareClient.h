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
#include <thread>
#include <mutex>
#include "TacTweak.h"

#include "TacStateMachine.h"

namespace Tac
{
  class FileShareState;
  class FileShareClient : public MiniApp
  {
  public:
    TacDLLAPI FileShareClient();
    TacDLLAPI ~FileShareClient();
    TacDLLAPI bool Init();
    TacDLLAPI virtual void Update(float dt);
    void Error(const std::string & errorMessage);
    void Connect();
    StateMachine<FileShareClient> & GetStateMachine();

  private:
    StateMachine<FileShareClient> mStateMachine;
    
  };
  typedef State<FileShareClient> FileShareClientState;
  DefineSharedPtr(FileShareClientState);

} // Tac
