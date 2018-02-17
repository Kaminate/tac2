/*!
*/
#include "TacShaderView.h"
namespace Tac
{
  ShaderView::ShaderView()
    : mResource(nullptr)
  {
    
  }

  void ShaderView::BindToResource( ShaderResource * resource )
  {
    mResource = resource;
    mResource->RegisterObserver(
      ResourceEvent::Reload, 
      std::bind(&ShaderView::OnReload, this));
    OnReload();
  }

  const Shader & ShaderView::GetShader() const
  {
    return mResource->GetShader();
  }

} // Tac
