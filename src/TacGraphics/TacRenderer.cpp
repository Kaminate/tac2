#include "TacRenderer.h"
#include "TacSystem.h"
#include "TacComponentModel.h"

namespace Tac
{
  Renderer::Renderer()
    : mState(new RendererState())
  {

  }

  Renderer::~Renderer()
  {
    delete mState;
  }

  void Renderer::Render(
    std::set<SPComp, LessSharedPtr<Comp> > & modelSet )
  {
    for(FramebufferStage * stage : mFramebufferStages)
    {
      if(!stage->GetActiveRef())
        continue;

      for(RenderOperation * op : stage->mInternalPreRenderOps)
        op->Execute(mState);
      for(RenderOperation * op : stage->mPreRenderOps)
        op->Execute(mState);

      for(ShaderPass * pass : stage->mShaderPasses)
      {
        for(RenderOperation * op : pass->mInternalPreRenderOps)
          op->Execute(mState);
        for(RenderOperation * op : pass->mPreRenderOps)
          op->Execute(mState);

        for (SPComp comp: modelSet)
        {
          ModelComponent * curModel = (ModelComponent*) comp.get();
          mState->SetModel(curModel);

          for(RenderOperation * op : pass->mModelOps)
            op->Execute(mState);
        }

        for(RenderOperation * op : pass->mPostRenderOps)
          op->Execute(mState);
        for(RenderOperation * op : pass->mInternalPostRenderOps)
          op->Execute(mState);
      }

      for(RenderOperation * op : stage->mPostRenderOps)
        op->Execute(mState);
      for(RenderOperation * op : stage->mInternalPostRenderOps)
        op->Execute(mState);
    }
  }

  void Renderer::AddFramebufferStage( FramebufferStage * stage )
  {
    mFramebufferStages.push_back(stage);
  }

  void Renderer::RemoveFramebufferStage( FramebufferStage * stage )
  {
    auto it = std::find(
      mFramebufferStages.begin(),
      mFramebufferStages.end(),
      stage);
    ErrorIf(it == mFramebufferStages.end(),
      "Failure to find frambuffer stage to remove!");
    mFramebufferStages.erase(it);
  }

}