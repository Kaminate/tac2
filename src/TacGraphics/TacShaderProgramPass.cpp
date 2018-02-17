#include "TacShaderProgramPass.h"
#include "TacGraphics\TacOperations\TacBindShader.h"
#include "TacGraphics\TacFramebufferStage.h"
namespace Tac
{


  ShaderPass::ShaderPass() 
    : mName("Unnamed shader pass")
    , mResource(nullptr)
    , mActive( true )
    , mOwner(nullptr)
  {

  }

  void ShaderPass::SetShader(ShaderResource * myShader)
  {
    mResource = myShader;

    mInternalPreRenderOps.push_back(new BindShader(myShader));
    mInternalPostRenderOps.push_back(new UnbindShader(myShader));
  }

  void ShaderPass::SetName( const std::string & name )
  {
    mName = name;
  }

  const std::string & ShaderPass::GetName() const
  {
    return mName;
  }

  void * ShaderPass::GetShader()
  {
    return mResource;
  }

  ShaderPass::~ShaderPass()
  {
#define DeleteAndClear(container) \
  for(auto pointer : container) {delete pointer;} container.clear();

    DeleteAndClear(mPreRenderOps);
    DeleteAndClear(mPostRenderOps);
    DeleteAndClear(mInternalPreRenderOps);
    DeleteAndClear(mInternalPostRenderOps);
    DeleteAndClear(mModelOps);
  }

  void ShaderPass::SetOwner( FramebufferStage*bufferstage )
  {
    mOwner = bufferstage;
  }

  void ShaderPass::AddPreRenderOp( RenderOperation * op )
  {
    mPreRenderOps.push_back(op);
  }

  void ShaderPass::AddPostRenderOperation( RenderOperation * op )
  {
    mPostRenderOps.push_back(op);
  }

  void ShaderPass::AddModelRenderOperation( RenderOperation*op )
  {
    mModelOps.push_back(op);
  }

} // Tac
