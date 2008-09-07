#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    �����������
*/

FboFrameBufferManager::FboFrameBufferManager (const ContextManager& context_manager)
  : ContextObject (context_manager)
{
}

/*
    �������� ������� ����������
*/

ITexture* FboFrameBufferManager::CreateRenderBuffer (const TextureDesc& desc)
{
  try
  {
    return new FboRenderBuffer (this, desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FboFrameBufferManager::CreateRenderBuffer");
    throw;
  }
}

/*
    �������� ������� �����
*/

//�������� ����������� �������� ������ �����
bool FboFrameBufferManager::IsSupported (View* color_view, View* depth_stencil_view)
{
  View* views [2] = {color_view, depth_stencil_view};

  for (int i=0; i<2; i++)
    if (views [i])
    {
      ITexture* base_texture = views [i]->GetTexture ();

      if (!dynamic_cast<IRenderTargetTexture*> (base_texture) && !dynamic_cast<FboRenderBuffer*> (base_texture))
        return false;
    }

  return true;
}

//�������� ������ �����
IFrameBuffer* FboFrameBufferManager::CreateFrameBuffer (View* color_view, View* depth_stencil_view)
{
  try
  {
    return new FboFrameBuffer (this, color_view, depth_stencil_view);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FboFrameBufferManager::CreateFrameBuffer");
    throw;
  }
}

/*
    ��������� �������� ������ �����
*/

void FboFrameBufferManager::SetFrameBuffer (size_t fbo_id, size_t cache_id)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FboFrameBufferManager::SetFrameBuffer";

    //�������� ������������� ������������� ������

  ContextDataTable &state_cache = GetContextDataTable (Stage_RenderTargetManager);
  size_t           &current_id  = state_cache [RenderTargetManagerCache_FrameBufferId];
  
  if (current_id == cache_id)
    return;

    //��������� �������� ���������  

  MakeContextCurrent ();

    //�������� ������� ������������ ����������

  if (!GetCaps ().has_ext_framebuffer_object)
    throw xtl::format_not_supported_exception (METHOD_NAME, "GL_EXT_framebuffer_object not supported");

    //��������� ������ � �������� OpenGL

  glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, fbo_id);

    //�������� ������

  CheckErrors (METHOD_NAME);

    //��������� �������� ���-����������

  current_id = cache_id;
}

/*
    ����������� ������������ �������� ������� ���������� � ������� �����
*/

namespace render
{

namespace low_level
{

namespace opengl
{

void register_fbo_manager (RenderTargetRegistry& registry, const ContextManager& context_manager, ISwapChain*)
{
  FrameBufferManagerPtr manager (new FboFrameBufferManager (context_manager), false);

  registry.RegisterCreater (RenderBufferCreater (xtl::bind (&FboFrameBufferManager::CreateRenderBuffer, manager, _1)));
  registry.RegisterCreater (&FboFrameBufferManager::IsSupported, xtl::bind (&FboFrameBufferManager::CreateFrameBuffer, manager, _1, _2));
}

}

}

}
