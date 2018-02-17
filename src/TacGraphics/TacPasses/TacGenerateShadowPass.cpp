#include "TacGenerateShadowPass.h"
#include "TacFramebuffer.h"
#include "TacSystemGraphics.h"
#include "TacResourceManager.h"
#include "TacGraphics\TacFramebufferStage.h"
#include "TacGraphics\TacOperations\TacBindUniform.h"
#include "TacGraphics\TacOperations\TacBindAttribute.h"
#include "TacGraphics\TacOperations\TacBindCameraUniform.h"
#include "TacGraphics\TacOperations\TacBindModelMatrix.h"
#include "TacGraphics\TacOperations\TacDrawOperation.h"
#include "TacGraphics/TacOperations/TacViewportOperation.h"
namespace Tac
{
  // this shouldbe called when the light's "cast shadows" 
  // is toggled from false to true

  // or if the light is initialized (after ofc the renderer is alive)
  // and its "cast shadows" is true
} // Tac

