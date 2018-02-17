#ifndef TAC_RENDERER_H
#define TAC_RENDERER_H
//#include <vector>
#include <set>
#include <algorithm>
#include <list>
#include "TacGraphics/TacFramebufferStage.h"
#include "TacComponent.h"
#include "TacRendererState.h"

namespace Tac
{
  class Renderer
  {
  public:
    Renderer();
    ~Renderer();
    void Render( std::set<SPComp, LessSharedPtr<Comp> > & modelSet );
    void AddFramebufferStage(FramebufferStage * stage);
    void RemoveFramebufferStage(FramebufferStage * stage);

  private:
    // owned by resource manager
    std::list<FramebufferStage*> mFramebufferStages;
    RendererState * mState;
  };
}

#endif  
