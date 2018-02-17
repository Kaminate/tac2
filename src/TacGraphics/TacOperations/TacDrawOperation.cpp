#include "TacDrawOperation.h"
#include "TacComponentModel.h"
#include "TacGraphics\TacRendererState.h"

namespace Tac
{
  DrawOperation::DrawOperation( 
#ifdef IncludeGLFile
    GLenum primitiveType
#else
    Primitive primitiveType
#endif
    ):mPrimitive(primitiveType)
  {
    

  }

  void DrawOperation::Execute( RendererState * state)
  {
    const ModelComponent * modelComp  = state->GetModel();
    GLsizei count = modelComp->GetGeometry()->GetVertexCount();
#ifdef IncludeGLFile
    glDrawArrays(mPrimitive, 0, count);
#else
    //GLenum glPrimitive = GL_TRIANGLES;
    switch (mPrimitive)
    {
    case DrawOperation::ePoints: glPrimitive =GL_POINTS;break;
    case DrawOperation::eLineStrip: glPrimitive = GL_LINE_STRIP; break;
    case DrawOperation::eLineLoop: glPrimitive = GL_LINE_LOOP; break;
    case DrawOperation::eLines:glPrimitive = GL_LINES; break;
    case DrawOperation::eLineStripAdjacency:glPrimitive = GL_LINE_STRIP_ADJACENCY; break;
    case DrawOperation::eLineAdjacency: glPrimitive = GL_LINES_ADJACENCY; break;
    case DrawOperation::eTriStrip: glPrimitive = GL_TRIANGLE_STRIP; break;
    case DrawOperation::eTriFan: glPrimitive = GL_TRIANGLE_FAN; break;
    case DrawOperation::eTris: glPrimitive = GL_TRIANGLES; break;
    case DrawOperation::eTriStripAdjacency: glPrimitive = GL_TRIANGLE_STRIP_ADJACENCY; break;
    case DrawOperation::eTriAdjacency: glPrimitive = GL_TRIANGLES_ADJACENCY; break;
    case DrawOperation::ePatches: glPrimitive = GL_PATCHES;
      break;
    default:
      break;
    }
    glDrawArrays(glPrimitive, 0, count);
#endif
  }

  DrawOperationIndependent::DrawOperationIndependent(
    GLenum primitiveType,
    std::function<int(void)> bufferElementCountGetterFunction )
    : mPrimitive(primitiveType)
    , mBufferElementCountGetterFunction(bufferElementCountGetterFunction)
  {

  }

  void DrawOperationIndependent::Execute(RendererState*state)
  {
    state;
    glDrawArrays(mPrimitive, 0, mBufferElementCountGetterFunction());
  }

}