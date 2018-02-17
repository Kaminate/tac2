/*!
\author Nathan Park
\file System.cpp
\par Project: Tac Engine
\brief
*/
#include "TacSystem.h"
#include "TacErrorIf.h"
#include "TacMacros.h"

namespace Tac
{
  System::System() : mCore(nullptr)
  {

  }

  System::~System()
  {

  }

  void System::Update( float dt )
  {
    UnusedParameter(dt);
  }

  void System::UpdateFakeTime( float dt )
  {
    UnusedParameter(dt);
  }

  Core * System::GetCore() const
  {
    return mCore;
  }

  void System::AddComp( Comp::Type compType, SPComp comp )
  {
    GetComponentSet(compType).insert(comp);
    //mComponentMap[compType][comp.get()] = comp;
  }

  void System::RemComp( Comp::Type compType, SPComp comp )
  {
    //mComponentMap[compType].erase(comp.get());
    GetComponentSet(compType).erase(comp);
  }

  // non-const so it can insert a componentset into empty componentMap
  System:: ComponentSet & System::GetComponentSet( Comp::Type compType )
  {
    return mComponentMap[compType];
  }

  const System::ComponentSet & System::GetComponentSet( Comp::Type compType ) const
  {
    return mComponentMap[compType];

  }

  SPComp System::GetRandomComp( Comp::Type compType )
  {
    SPComp comp;
    ComponentSet & compSet = GetComponentSet(compType);
    if (!compSet.empty())
    {
      unsigned index = rand() % compSet.size();
      ComponentSet::iterator it = compSet.begin();
      while (index--)
      {
        ++it;
      }
      comp = *it;
    }

    return comp;
  }

  std::string System::toStr( Type sysType )
  {
    switch (sysType)
    {
    case Tac::System::eSystemGraphics:
      return "Graphics";
      break;
    case Tac::System::eSystemTransform:
      return "Transform";
      break;
    case Tac::System::eSystemEditor:
      return "Editor";
      break;
    case Tac::System::eSystemPhysics:
      return "Phys";
      break;
    case Tac::System::eSystemApp:
      return "App";
      break;
    case Tac::System::eSystemCount:
      return "Count";
      break;
    default:
      return "Error";
      break;
    }

  }

  void System::SetCore( Core * engine )
  {
    mCore = engine;
  }

  void System::Init()
  {

  }

} // Tac
