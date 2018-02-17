// Nathan Park
#include "TacComponentCreator.h "
#include "TacErrorIf.h"
// every component
#include <TacComponentCamera.h>
#include <TacComponentModel.h>
#include <TacComponentTransform.h>
#include <TacComponentLight.h>
#include <TacComponentPhysics.h>
namespace Tac
{
  struct Base
  {
    virtual SPComp Create() = 0;
  };
  DefineSharedPtr(Base);

  template <typename T>
  struct Derived : public Base
  {
    virtual SPComp Create()
    {
      return std::shared_ptr<T>(new T());
    }
  };

  struct ComponentCreator::Impl
  {
    SPBase mCreators[Comp::Type::eCompCount];
    Impl()
    {
      // Register every component
      mCreators[Comp::Type::eCompCamera] = SPBase(new Derived<CameraComponent>());
      mCreators[Comp::Type::eCompModel] = SPBase(new Derived<ModelComponent>());
      mCreators[Comp::Type::eCompTransform] = SPBase(new Derived<TransformComp>());
      mCreators[Comp::Type::eCompLight] = SPBase(new Derived<LightComponent>());
      mCreators[Comp::Type::eCompPhysics] = SPBase(new Derived<PhysicsComp>());
    }
  };

  ComponentCreator::~ComponentCreator()
  {
    delete mImpl;
  }

  SPComp ComponentCreator::Create( Comp::Type compType )
  {
    SPBase b = mImpl->mCreators[compType];
    ErrorIf(!b, "Component not registered in ComponentCreator.cpp");
    return b->Create();
  }

  TacDLLAPI ComponentCreator::ComponentCreator() : mImpl(new Impl())
  {

  }

  //
  // CreatorMap
  // 
  
  CreatorMap::CreatorMap()
  {
    // to guarante valid
    for (unsigned i = 0 ; i < Comp::Type::eCompCount; ++i)
      mCreators[i] = System::Type::eSystemCount;
    
    // register every component
    mCreators[Comp::Type::eCompCamera] = System::Type::eSystemGraphics;
    mCreators[Comp::Type::eCompModel] = System::Type::eSystemGraphics;
    mCreators[Comp::Type::eCompTransform] = System::Type::eSystemTransform;
    mCreators[Comp::Type::eCompPhysics] = System::Type::eSystemPhysics;
    mCreators[Comp::Type::eCompLight] = System::Type::eSystemGraphics;

    // see? valid.
    for (unsigned i = 0 ; i < Comp::Type::eCompCount; ++i)
      ErrorIf(mCreators[i] == System::Type::eSystemCount, 
      "There is an unregistered component");
  }

  System::Type CreatorMap::GetCreator( Comp::Type compType )
  {
    return mCreators[compType];
  }

}// Tac;