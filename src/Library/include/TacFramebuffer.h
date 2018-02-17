// Nathan Park
#pragma once
#include "TacGLIncludes.h"
#include "TacResource.h"
#include "TacTexture.h"
namespace Tac
{
  class Framebuffer : public Resource
  {
  public:
    static Framebuffer * GetBackbuffer(); 
    Framebuffer(const std::string & name);
    ~Framebuffer();

    virtual std::string ReloadAux() override;

    bool HasTexture(GLuint attachmentPoint) const;
    Texture * GetTexture(GLuint attachmentPoint)const;
    void AttachTexture(Texture *, GLuint attachmentPoint);
    GLuint GetHandle() const;
    void CheckCompleteness();

  private:
    std::string mName;
    GLuint mHandle;
    Framebuffer(); // used to create backbuffer

    // owned by resource manager
    std::map<GLuint, Texture*> mAttachedTextures;
  };
}
