#include "TacColorMaskOp.h"
#include "TacGraphics\TacRendererState.h"
#include "TacGLIncludes.h"

namespace Tac
{
  ColorMaskOp::ColorMaskOp( bool r, bool g, bool b, bool a )
    : mRMask(r)
    , mGMask(g)
    , mBMask(b)
    , mAMask(a)
  {

  }

  void ColorMaskOp::Execute( RendererState * state )
  {
    state;
    // Disable color, it's useless, we only want depth.
    glColorMask(mRMask,mGMask,mBMask,mAMask);
    GL_FALSE;
    GL_TRUE;
  }

}

