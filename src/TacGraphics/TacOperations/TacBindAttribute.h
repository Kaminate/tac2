// structure based off KoRE rendering engine

#ifndef TAC_BIND_ATTRIBUTE_OPERATION_H_
#define TAC_BIND_ATTRIBUTE_OPERATION_H_
#include "TacGraphics\TacRenderOperation.h"
#include "TacGeometryData.h"
#include "TacGeometry.h"

namespace Tac
{
  class ShaderInput;
  class RendererState;

  class BindAttribute : public RenderOperation //public ModelRenderOperation
  {
  public:
    // @param data - if null, uses the model's attribute
    BindAttribute(
      ShaderInput * input,
      Attribute attrib,
      Geometry::AttributeData * data); 
    virtual void Execute(RendererState * state) override;
    // owned by a shader somewhere
    ShaderInput * mInput;
    Attribute mAttrib;
    Geometry::AttributeData * mData;
  };

}
#endif