/*!
\author Nathan Park

\par Project: Tac Engine

\brief

*/

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once

#include "TacComponent.h"

#include <map>
#include "TacErrorIf.h"

namespace Tac
{
  class Entity
  {
  public:
    typedef int ID;
    TacDLLAPI const static ID InvalidId = -1;
    TacDLLAPI Entity(ID id);
    TacDLLAPI ~Entity();
    TacDLLAPI ID GetID() const;
    TacDLLAPI virtual bool Initialize();

    TacDLLAPI void AddComponent(Comp::Type compType, SPComp comp);
    TacDLLAPI void RemComponent(Comp::Type compType);
    TacDLLAPI bool HasComponent(Comp::Type compType);
    TacDLLAPI Comp * GetComponent(Comp::Type compType);
    TacDLLAPI const Comp * GetComponent(Comp::Type compType)const;
    TacDLLAPI unsigned GetNumComps() const;

#ifdef TACTWEAK

    TacDLLAPI void AddToTweakBar(TweakBar & bar);
#endif

  private:
    SPComp mComponents[Comp::Type::eCompCount];
    ID mId;
  };

  template<typename ComponentType>
  void GET_COMP(
    ComponentType *& comp, 
    Entity * gameObj, 
    Comp::Type compType)
  {
    Comp * baseComp = gameObj->GetComponent(compType);
    comp = dynamic_cast<ComponentType *>(baseComp);
    ErrorIf(
      !comp, 
      "Entity does not contain %i, make sure you've called "
      "Entity::HasComponent()", 
      (int)compType);
  }


  template<typename ComponentType>
  void GET_SIBL(
    ComponentType *& comp, 
    Comp * sibling, 
    Comp::Type compType)
  {
    Entity * gameObject = sibling->GetEntity();
    GET_COMP(comp, gameObject, compType);
  }


} // Tac
