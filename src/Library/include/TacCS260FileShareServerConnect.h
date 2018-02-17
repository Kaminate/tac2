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

#include "TacStateMachine.h"
#include "TacCS260FileShareServer.h"

namespace Tac
{
  class FileShareServerConnect: public State<FileShareServer>
  {
  public:
    virtual void Init();
    virtual void Update(float dt);
  };

} // Tac
