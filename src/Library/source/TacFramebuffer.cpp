// Nathan Park
#include "TacFramebuffer.h"
#include "TacResourceManager.h"
#include "TacErrorIf.h"
namespace Tac
{
  Framebuffer * Framebuffer::GetBackbuffer()
  {
    static Framebuffer * backbuffer = [](){
      Framebuffer * buffer = new Framebuffer();
      ResourceManager::Instance().AddResource(buffer);
      return buffer;
    }();
    return backbuffer;
  }

  GLuint Framebuffer::GetHandle() const
  {
    return mHandle;
  }

  Framebuffer::Framebuffer(const std::string & name) 
    : mName(name)
  {
    glGenFramebuffers(1, &mHandle);
  }
  Framebuffer::Framebuffer()
    : mHandle( 0 )
  {
  }

  void Framebuffer::AttachTexture(
    Texture* tex,
    GLuint attachmentPoint )
  {
    // errorif texture already bound
    ErrorIf(mAttachedTextures[attachmentPoint], 
      "Already exists an attached texture");
    mAttachedTextures[attachmentPoint] = tex;
    glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
    
    glFramebufferTexture(
      GL_FRAMEBUFFER, 
      attachmentPoint,
      tex->GetHandle(), 
      0); // mipmap level

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void Framebuffer::CheckCompleteness()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
    {
      DebugError("...");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  std::string Framebuffer::ReloadAux()
  {
    return "";
  }

  Texture * Framebuffer::GetTexture( GLuint attachmentPoint )const
  {
    return mAttachedTextures.at(attachmentPoint);
  }

  Framebuffer::~Framebuffer()
  {
    glDeleteFramebuffers(1, &mHandle);
  }

  bool Framebuffer::HasTexture( GLuint attachmentPoint ) const
  {
    return mAttachedTextures.find(attachmentPoint) !=
      mAttachedTextures.end();
  }

}