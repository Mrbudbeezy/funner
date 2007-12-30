#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    �����������
*/

ColorBuffer::ColorBuffer (const ContextManager& manager)
  : ContextObject (manager)
{
  desc.dimension            = TextureDimension_2D;
  desc.width                = 0;
  desc.height               = 0;
  desc.layers               = 1;
  desc.format               = PixelFormat_RGBA8;
  desc.generate_mips_enable = false;
  desc.access_flags         = AccessFlag_Read | AccessFlag_Write;  
  desc.bind_flags           = BindFlag_RenderTarget;
  desc.usage_mode           = UsageMode_Static;
}

/*
    ��������� �����������
*/

void ColorBuffer::GetDesc (TextureDesc& out_desc)
{
  out_desc = desc;
  
  GetSize (out_desc.width, out_desc.height);
}

/*
    ������ � �������
*/

void ColorBuffer::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, const void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ColorBuffer::SetData";

  if (!buffer)
    RaiseNullArgument (METHOD_NAME, "buffer");
    
  if (layer)
    RaiseOutOfRange (METHOD_NAME, "layer", layer, 1);
    
  if (mip_level)
    RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, 1);

    //��������� ������ � �������� OpenGL

  Bind ();

    //�������� ������� ���������� GL_ARB_window_pos ���� ������ OpenGL �� ���� 1.4

  if      (glWindowPos2iARB) glWindowPos2iARB  (x, y);
  else if (glWindowPos2i)    glWindowPos2i     (x, y);
  else                       RaiseNotSupported ("render::low_level::opengl::ColorBuffer::SetData", "Extension GL_ARB_window_pos not supported");

    //�����������

  glDrawPixels (width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    //�������� ��������� OpenGL

  CheckErrors (METHOD_NAME);
}

void ColorBuffer::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ColorBuffer::GetData";

  if (!buffer)
    RaiseNullArgument (METHOD_NAME, "buffer");
    
  if (layer)
    RaiseOutOfRange (METHOD_NAME, "layer", layer, 1);
    
  if (mip_level)
    RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, 1);    

    //��������� ������ � �������� OpenGL

  Bind ();

    //�����������

  glReadPixels (x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    //�������� ��������� OpenGL

  CheckErrors (METHOD_NAME);
}
