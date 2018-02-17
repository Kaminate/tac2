
#ifndef TAC_DRAW_OPERATION_H_
#define TAC_DRAW_OPERATION_H_

#include "TacGraphics\TacRenderOperation.h"

#define IncludeGLFile

#ifdef IncludeGLFile
#include "TacGLIncludes.h"
#endif
#include <functional>


namespace Tac
{
  class RendererState;
  class DrawOperation : public RenderOperation
  {
  public:
#ifdef IncludeGLFile
    DrawOperation(GLenum primitiveType);
#else
    enum Primitive
    {
      ePoints,

      eLineStrip,
      eLineLoop,
      eLines,
      eLineStripAdjacency,
      eLineAdjacency,

      eTriStrip,
      eTriFan,
      eTris,
      eTriStripAdjacency,
      eTriAdjacency,

      ePatches
    };
    DrawOperation(Primitive primitiveType);
#endif
    virtual void Execute( RendererState * state) override;


  private:
#ifdef IncludeGLFile
    GLenum mPrimitive;
#else
    Primitive mPrimitive;
#endif
  };

  class DrawOperationIndependent : public RenderOperation
  {
    GLenum mPrimitive;
    std::function<int(void)> mBufferElementCountGetterFunction;
  public:
    DrawOperationIndependent(
      GLenum primitiveType,
	  // getter fn for how many vertexes to draw
      std::function<int(void)> bufferElementCountGetterFunction);

    virtual void Execute(RendererState*state) override;
  };

}
#endif