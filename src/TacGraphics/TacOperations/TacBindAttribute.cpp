#include "TacBindAttribute.h"
#include "TacGraphics\TacShaderInput.h"
#include "tacGraphics\TacRendererState.h"
#include "TacComponentModel.h"

namespace Tac
{



  BindAttribute::BindAttribute(
    ShaderInput * input ,
    Attribute attrib,
    Geometry::AttributeData * Data)

    : mInput(input)
    , mAttrib(attrib)
    , mData(Data)
  {

  }

  void BindAttribute::Execute(RendererState * state )
  { 
    if(mData)
    {
      glVertexAttribPointer(
        mInput->mLocation,
        mData->componentsPerVert, 
        mData->componentType, GL_FALSE, mData->stride, nullptr);
    }
    else
    {
      const ModelComponent * modelComp = state->GetModel();
      const Geometry * geom = modelComp->GetGeometry();

      const Geometry::AttributeData & data 
        = geom->GetAttributeData(mAttrib);

      glBindBuffer(GL_ARRAY_BUFFER, geom->GetAttribute(mAttrib));

      glVertexAttribPointer(
        mInput->mLocation,
        data.componentsPerVert, 
        data.componentType, GL_FALSE, data.stride, nullptr);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
  }
}