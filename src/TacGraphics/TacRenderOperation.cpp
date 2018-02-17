#include "TacRenderOperation.h"

namespace Tac
{


  RenderOperation::RenderOperation() 
    : mName("Unnamed Render Operation")
    , mOwner(nullptr)
  {

  }

  void RenderOperation::SetOwner( ShaderPass*owner )
  {
    mOwner=owner;

  }



}