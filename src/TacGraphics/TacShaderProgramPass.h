// structure based off KoRE rendering engine

#ifndef TAC_SHADER_PROGRAM_PASS_H_
#define TAC_SHADER_PROGRAM_PASS_H_

#include <string>
#include <vector>

#include "TacRenderOperation.h"
#include "TacResource.h"
namespace Tac
{
  class ShaderResource;
  class FramebufferStage;
  class ShaderPass : public Resource
  {
  public:
    ShaderPass();
    ~ShaderPass();

    virtual std::string ReloadAux() override {return "";} // todo: remove

    void SetShader(ShaderResource * myshader);
    void * GetShader();

    void SetName(const std::string & name);
    const std::string & GetName()const;

    void SetOwner(FramebufferStage*bufferstage);
    void AddPreRenderOp(RenderOperation * op);
    void AddPostRenderOperation(RenderOperation * op);
    void AddModelRenderOperation(RenderOperation*op);

  private:
    bool mActive;

    friend class Renderer;
    // takes ownership
    std::vector<RenderOperation* > mInternalPreRenderOps;
    std::vector<RenderOperation* > mPreRenderOps;
    std::vector<RenderOperation* > mModelOps;
    std::vector<RenderOperation *> mPostRenderOps;
    std::vector<RenderOperation* > mInternalPostRenderOps;

    FramebufferStage * mOwner;
    std::string mName;

    // owned by resource manager
    ShaderResource * mResource;
  };
}
#endif