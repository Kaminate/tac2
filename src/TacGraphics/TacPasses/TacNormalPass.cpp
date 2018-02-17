#include "TacNormalPass.h"
#include "TacFramebuffer.h"
#include "TacSystemGraphics.h"
#include "TacResourceManager.h"
#include "TacGraphics\TacFramebufferStage.h"
#include "TacGraphics\TacOperations\TacBindUniform.h"
#include "TacGraphics\TacOperations\TacBindAttribute.h"
#include "TacGraphics\TacOperations\TacBindCameraUniform.h"
#include "TacGraphics\TacOperations\TacBindModelMatrix.h"
#include "TacGraphics\TacOperations\TacDrawOperation.h"

namespace Tac
{
  float gArrowLen = 0.05f;
  ShaderData gArrowLenData;
  float gArrowDistFromSurface = 0.0;
  ShaderData gArrowDistFromSurfaceData;

  void AddNormalPass(GraphicsSystem * graphicsSys)
  {
    gArrowLenData.mdata = &gArrowLen;
    gArrowLenData.mname = "ArrowLen";
    gArrowLenData.msize = 1;
    gArrowLenData.mtype = GL_FLOAT;
    gArrowDistFromSurfaceData.mdata = &gArrowDistFromSurface;
    gArrowDistFromSurfaceData.mname = "ArrowDistFromSurface";
    gArrowDistFromSurfaceData.msize = 1;
    gArrowDistFromSurfaceData.mtype = GL_FLOAT;

    FramebufferStage * backBufferStage = new FramebufferStage();
    backBufferStage->GetActiveRef() = false; // temp
    backBufferStage->SetName("Draw normals onto backbuffer");
    ResourceManager::Instance().AddResource(backBufferStage);



    std::vector<GLenum> drawBufs;
    drawBufs.push_back(GL_BACK_LEFT);
    backBufferStage->SetFramebuffer(Framebuffer::GetBackbuffer(), drawBufs);
    graphicsSys->GetRenderer()->AddFramebufferStage(backBufferStage);

    ShaderPass *normalPass = new ShaderPass();
    ResourceManager::Instance().AddResource(normalPass);
    backBufferStage->AddShaderPass(normalPass);

    ShaderResource * resourceNor = new ShaderResource();
    resourceNor->AddShader(ShaderResource::eVertexShader,
      "resources/TacShaders/ShaderNor.v.glsl");
    resourceNor->AddShader(ShaderResource::eGeometryShader,
      "resources/TacShaders/ShaderNor.g.glsl");
    resourceNor->AddShader(ShaderResource::eFragmentShader,
      "resources/TacShaders/ShaderNor.f.glsl");
    resourceNor->Init();
    ResourceManager::Instance().AddResource(resourceNor);

    normalPass->SetShader(resourceNor);


    normalPass->AddPreRenderOp(new BindCameraUniform(
      resourceNor->GetUniform("view"),
      BindCameraUniform::eMatrixView));

    normalPass->AddPreRenderOp(new BindCameraUniform(
      resourceNor->GetUniform("proj"), 
      BindCameraUniform::eMatrixProjPerspective));

    normalPass->AddPreRenderOp(new BindUniform(
      resourceNor->GetUniform("arrowLen"), &gArrowLenData));

    normalPass->AddPreRenderOp(new BindUniform(
      resourceNor->GetUniform("arrowDistFromSurface"), 
      &gArrowDistFromSurfaceData));

    normalPass->AddModelRenderOperation(new BindModelMatrix(
      resourceNor->GetUniform("world"),
      BindModelMatrix::eWorld));
      
    normalPass->AddModelRenderOperation(new BindModelMatrix(
      resourceNor->GetUniform("invTransWorld"), 
      BindModelMatrix::eInverseWorld));
    
    normalPass->AddModelRenderOperation(new BindAttribute(
      resourceNor->GetAttribute("position"),
      Attribute::ePos,
      nullptr));

    normalPass->AddModelRenderOperation(new BindAttribute(
      resourceNor->GetAttribute("normal"),
      Attribute::eNor,
      nullptr));

    normalPass->AddModelRenderOperation(new DrawOperation(GL_POINTS));

#ifdef TACTWEAK
    std::string tweakGroup(" group=normal ");
    graphicsSys->mRenderBarIsWunderBar->AddVar(
      "normalActive",
      "label='Active'" + tweakGroup,
      TW_TYPE_BOOLCPP, &backBufferStage->GetActiveRef());
    graphicsSys->mRenderBarIsWunderBar->AddVar(
      "arrowLen", 
      "label='Arrow length' min=0 step=0.01" + tweakGroup,
      TW_TYPE_FLOAT, &gArrowLen );
    graphicsSys->mRenderBarIsWunderBar->AddVar(
      "distFromSurface", 
      "label='Distance from surface' min=0 step=0.01" + tweakGroup,
      TW_TYPE_FLOAT, &gArrowDistFromSurface );
#endif
    
  }
} // Tac
