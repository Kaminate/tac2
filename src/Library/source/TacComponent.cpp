// Nathan Park
#include "TacComponent.h"
#include "TacComponentCreator.h"
#include "TacSystem.h"
#include "TacEntity.h"
#include <TacErrorIf.h>
namespace Tac
{

  Comp::Comp() : mSystem(nullptr), mEntity(nullptr)
  {
  }

  Comp::~Comp()
  {
  }

  void Comp::SetSystem(System  * sys )
  {
    mSystem = sys;
  }

  System * Comp::GetSystem()
  {
    return mSystem;
  }

  void Comp::SetEntity( Entity * gameObjet )
  {
    mEntity = gameObjet;
  }

  bool Comp::HasSibling( Type compType )
  {
    return mEntity->HasComponent(compType);
  }

  Comp * Comp::GetSibling( Type compType )
  {
    return mEntity->GetComponent(compType);
  }

  const Comp * Comp::GetSibling( Type compType )const
  {
    return mEntity->GetComponent(compType);
  }
  const char * Comp::GetTypeString( Type compType )
  {
    switch (compType)
    {
    case Tac::Comp::eCompTransform:
      return "Transform";
      break;
    case Tac::Comp::eCompModel:
      return "Model";
      break;
    case Tac::Comp::eCompCamera:
      return "Camera";
      break;
    case Tac::Comp::eCompPhysics:
      return "Physics";
      break;
    case Tac::Comp::eCompLight:
      return "Light";
      break;
    case Tac::Comp::eCompCount:
      break;
    default:
      break;
    }

    DebugError("Component unregistered with Comp::GetTypeString()");
    return nullptr;
  }

  Entity * Comp::GetEntity() const
  {
    return mEntity;
  }


  
  std::ifstream & operator >> (
    std::ifstream & ifs, 
    Comp::Type & compType)
  {
    unsigned i;
    ifs >> i;
    compType = (Comp::Type) i;
    return ifs;
  }

  std::ofstream & operator<<( 
    std::ofstream & ofs, 
    Comp::Type compType )
  {
    ofs << (unsigned) compType;
    return ofs;
  }


}