/*!
\author Nathan Park
\brief
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#ifndef __TAC_SYSTEM_H_
#define __TAC_SYSTEM_H_

#include <map>
#include <set>

#include "TacSharedPtr.h"
#include "TacComponent.h"
#include "TacErrorIf.h"

namespace Tac
{
  class Core;
  class System
  {
  public:

    enum Type
    {
      eSystemGraphics, 
      eSystemInput, 
      eSystemTransform, 
      eSystemEditor,
      eSystemPhysics,
      eSystemApp,
      eSystemCount
    };
    TacDLLAPI static std::string toStr(Type sysType);
    // since this is owned, it is responsibility of owner to remove
    TacDLLAPI System();
    TacDLLAPI virtual ~System();

    TacDLLAPI virtual void Init();
    TacDLLAPI virtual void Update(float dt);
    TacDLLAPI virtual void UpdateFakeTime(float dt);

    // 1. someone news a component, using appropriate ctor
    // 2. that someone then adds the component to the system using this fn
    TacDLLAPI void AddComp(Comp::Type compType, SPComp comp);
    TacDLLAPI void RemComp(Comp::Type compType, SPComp comp);
    TacDLLAPI SPComp GetRandomComp(Comp::Type compType);
    
    TacDLLAPI Core * GetCore() const;
    TacDLLAPI void SetCore(Core * engine);

    typedef std::set<SPComp, LessSharedPtr<Comp> > ComponentSet;
    ComponentSet & GetComponentSet(Comp::Type compType);
    const ComponentSet & GetComponentSet(Comp::Type compType) const;
  protected:
    
  private:
    Core * mCore;
    ComponentSet mComponentMap[Comp::Type::eCompCount];
  };


  // example use:
  //   InputSystem * input;
  //   GET_SYSTEM(input, engine, System::eSystemInput);
  //   input->DoThings(...);
  template<typename SystemType>
  void GET_SYSTEM(SystemType *& sys, Core * engine, System::Type sysType)
  {
    System * baseSys = engine->GetSystem(sysType);
    sys = dynamic_cast<SystemType *>(baseSys);
    ErrorIf(!sys, "Engine does not contain %i", (int)sysType);
  }
} // Tac

#endif
