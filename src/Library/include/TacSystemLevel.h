/*!
\author Nathan Park
\brief
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#ifndef __TAC_SYSTEM_LEVEL_H_
#define __TAC_SYSTEM_LEVEL_H_

#include "TacSystem.h"
#include "TacCallback.h"
#include "TacEntity.h"

#include <map>


namespace Tac
{
  /*
  class Level : public System
  {
  public:
    TacDLLAPI Level();
    TacDLLAPI virtual ~Level();
    TacDLLAPI virtual void Update(float dt);
    TacDLLAPI virtual void OnAdd(Core * engine);
    TacDLLAPI virtual void UpdateFakeTime(float dt);

    GameObject * AddGameObject();

  private:
    std::map<unsigned, SPGameObject> mGameObjects;

  };
  */

} // Tac

#endif
