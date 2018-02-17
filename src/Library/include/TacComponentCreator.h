// Nathan Park

#pragma once
#include "TacComponent.h"
#include "TacSystem.h"

namespace Tac
{
  class ComponentCreator
  {
  public:
    TacDLLAPI ComponentCreator();
    TacDLLAPI ~ComponentCreator();

    // guaranteed valid
    TacDLLAPI SPComp Create(Comp::Type compType);
  private:
    struct Impl;
    Impl * mImpl;
  };

  struct CreatorMap
  {
    TacDLLAPI CreatorMap();

    // guaranteed valid
    TacDLLAPI System::Type GetCreator(Comp::Type compType);
  private:
    System::Type mCreators[Comp::Type::eCompCount];
  };
} // Tac
