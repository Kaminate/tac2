/*!
\author Nathan Park
*/
#pragma once

#include "TacGLIncludes.h"
#include "TacResource.h"
#include <string>

namespace Tac
{
  class Texture
  {

    
    //brief Creates and allocates an empty texture.
    //param properties The requested texture properties.
    //param name The KoRE-internal name of the Texture (be creative! ;) )
    //param semantics The Texture-semantics used to identify the 
    //useage of this texture
    //param pixelData Pointer to the pixels.
    //       If this parameter is not provided,
    //       an empty texture will be created (e.g. for use with FBOs)
    //return True, if the creation was successful,
    //        False if creation failed (see Log for infos why)
    //
    //bool init(const STextureProperties& properties, const std::string& name,
    //          const ETextureSemantics semantics = TEXSEMANTICS_UNKNOWN,
    //          const GLvoid* pixelData = NULL);

  public:
    struct Properties
    {
      unsigned mWidth; 
      unsigned mHeight;
      unsigned mDepth;
      GLuint mTarget; // ie: GL_TEXTURE2D
      unsigned mBorder; // always zero?
      // for depth/stencil, must be some form of GL_DEPTH_COMPONENT,
      // GL_STENCIL_INDEX, or GL_DEPTH_STENCIL
      // otherwise GL_RGBA, GL_RED, GL_RGB, GL_GBR, etc
      GLuint mFormat;
      GLuint mPixelType; // ex: GL_FLOAT
      GLuint mInternalFormat; // ex: GL_RGBA8
      // numMips?
    };

    Texture(const Properties & props, void * pixelData);
    GLuint GetHandle() const;
    const Properties &GetProperties()const;
  private:

    GLuint mHandle;
    Properties mProperties;
  };


  class TextureResource : public Resource
  {
  public:
    TextureResource();
    ~TextureResource();
    void Load(const std::string & path);
    void Create(const Texture::Properties & props, void * pixData);
    virtual std::string ReloadAux() override;
  private:
    std::string mTexturePath;
    Texture * mTexture;
  };
} // Tac

