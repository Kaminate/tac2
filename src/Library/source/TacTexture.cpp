// Nathan Park
#include "TacTexture.h"
namespace Tac
{


  TextureResource::TextureResource() : mTexture(nullptr)
  {

  }

  TextureResource::~TextureResource()
  {
    delete mTexture;
  }

  void TextureResource::Load( const std::string & path )
  {
    delete mTexture;
    mTexturePath = path;
    Resource::AddPathToWatch(path);

    // todo: load pixdata, properties from file
    __debugbreak();
    void * pixData = nullptr;
    Texture::Properties props;

    mTexture = new Texture(props, pixData);
  }

  std::string TextureResource::ReloadAux()
  {
    std::string errors;
    delete mTexture;
    Load(mTexturePath);
    return errors;
  }

  void TextureResource::Create(
    const Texture::Properties & props, void * pixData )
  {
    delete mTexture;
    mTexture = new Texture(props, pixData);
  }


  Texture::Texture( const Properties & props, void * pixelData ) 
    : mProperties(props)
  {
    glGenTextures(1, &mHandle);
    glBindTexture(props.mTarget, mHandle);
    switch (props.mTarget)
    {
    case GL_TEXTURE_1D: 
      glTexImage1D(
        props.mTarget, 0, props.mInternalFormat,
        props.mWidth,
        props.mBorder, props.mFormat, props.mPixelType, pixelData);
      break;
    case GL_TEXTURE_2D: 
      glTexImage2D(
        props.mTarget, 0, props.mInternalFormat,
        props.mWidth, props.mHeight,
        props.mBorder, props.mFormat, props.mPixelType, pixelData);
      break;
    case GL_TEXTURE_3D: 
      glTexImage3D(
        props.mTarget, 0, props.mInternalFormat,
        props.mWidth, props.mHeight,props.mDepth,
        props.mBorder, props.mFormat, props.mPixelType, pixelData);
      break;
    }
    glBindTexture(props.mTarget, 0);
  }

  GLuint Texture::GetHandle() const
  {
    return mHandle;
  }

  const Texture::Properties & Texture::GetProperties() const
  {
    return mProperties;
  }

} // Tac
