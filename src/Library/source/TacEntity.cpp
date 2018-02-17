// Nathan Park
#include "TacEntity.h"
#include "TacSystem.h"
namespace Tac
{
  Entity::Entity( ID id ) : mId(id)
  {

  }

  Tac::Entity::ID Entity::GetID() const
  {
    return mId;
  }

  void Entity::AddComponent( Comp::Type compType, SPComp comp )
  {
    // removes existing one
    mComponents[compType] = comp;
    comp->SetEntity(this);
  }

  Comp * Entity::GetComponent( Comp::Type compType )
  {
    return mComponents[compType].get();
  }
  const Comp * Entity::GetComponent( Comp::Type compType )const
  {
    return mComponents[compType].get();
  }

  Entity::~Entity()
  {
    for(unsigned i = 0; i < Comp::Type::eCompCount; ++i)
    {
      if (SPComp & iComp = mComponents[i])
      {
        iComp->GetSystem()->RemComp((Comp::Type) i, iComp);
        iComp.reset();
      }
    }
  }

  unsigned Entity::GetNumComps() const
  {
    unsigned count = 0;
    for (unsigned i = 0; i < Comp::Type::eCompCount; ++i)
    {
      if (mComponents[i])
        ++count;
    }
    return count;
  }

  bool Entity::Initialize()
  {
    for (unsigned i = 0; i < Comp::Type::eCompCount; ++i)
    {
      if (SPComp comp = mComponents[i])
      {
        if (!comp->Initialize())
          return false;
      }
    }
    return true;
  }
#ifdef TACTWEAK

  void Entity::AddToTweakBar( TweakBar & bar )
  {
    for (unsigned i = 0; i < Comp::Type::eCompCount; ++i)
    {
      if (SPComp comp = mComponents[i])
      {
        comp->AddToTweakBar(bar);
      }
    }
  }
#endif
  TacDLLAPI void Entity::RemComponent( Comp::Type compType )
  {
    mComponents[compType].reset();

  }

  TacDLLAPI bool Entity::HasComponent( Comp::Type compType )
  {
    return GetComponent(compType) != nullptr;
  }

} // Tac