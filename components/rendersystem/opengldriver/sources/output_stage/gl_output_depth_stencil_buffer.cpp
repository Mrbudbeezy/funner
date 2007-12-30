#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    �����������
*/

DepthStencilBuffer::DepthStencilBuffer (const ContextManager& manager)
  : ContextObject (manager)
{
  desc.dimension            = TextureDimension_2D;
  desc.width                = 0;
  desc.height               = 0;
  desc.layers               = 1;
  desc.format               = PixelFormat_D24S8;
  desc.generate_mips_enable = false;
  desc.access_flags         = AccessFlag_Read | AccessFlag_Write;  
  desc.bind_flags           = BindFlag_DepthStencil;
  desc.usage_mode           = UsageMode_Static;
}

/*
    ��������� �����������
*/

void DepthStencilBuffer::GetDesc (TextureDesc& out_desc)
{
  out_desc = desc;
  
  GetSize (out_desc.width, out_desc.height);
}

/*
    ������ � �������
*/

namespace
{

//��������� ���������� �������
inline float get_depth_component (size_t pixel)
{
  return float (pixel & 0xFFFFFF) / float (0xFFFFFF);
}

//��������� ���������� ���������
inline unsigned char get_stencil_index (size_t pixel)
{
  return unsigned char (pixel >> 24);
}

//��������� ������������ ������� �� ����������� ������� � ���������
inline size_t get_depth_stencil_pixel (float depth, unsigned char stencil)
{
  return (size_t (depth * 0xFFFFFF) & 0xFFFFFF) + (size_t (stencil) << 24);
}

}

void DepthStencilBuffer::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, const void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::DepthStencilBuffer::SetData";

  if (!buffer)
    RaiseNullArgument (METHOD_NAME, "buffer");
    
  if (layer)
    RaiseOutOfRange (METHOD_NAME, "layer", layer, 1);
    
  if (mip_level)
    RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, 1);
    
    //���������� ������ ������ �� ���������� ������� � ���������
    
  xtl::uninitialized_storage<float>         depth_buffer (width * height);
  xtl::uninitialized_storage<unsigned char> stencil_buffer (width * height);    
  
  const size_t*  src_pixel         = static_cast<const size_t*> (buffer);
  float*         dst_depth_pixel   = depth_buffer.data ();
  unsigned char* dst_stencil_pixel = stencil_buffer.data ();
  
  for (size_t count=width*height; count--; src_pixel++, dst_depth_pixel++, dst_stencil_pixel++)
  {
    *dst_depth_pixel   = get_depth_component (*src_pixel);
    *dst_stencil_pixel = get_stencil_index (*src_pixel);
  }

    //��������� ������ � �������� OpenGL

  Bind ();

    //�������� ������� ���������� GL_ARB_window_pos ���� ������ OpenGL �� ���� 1.4

  if      (glWindowPos2iARB) glWindowPos2iARB  (x, y);
  else if (glWindowPos2i)    glWindowPos2i     (x, y);
  else                       RaiseNotSupported ("render::low_level::opengl::DepthStencilBuffer::SetData", "Extension GL_ARB_window_pos not supported");

    //�����������    

  glDrawPixels (width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer.data ());
  glDrawPixels (width, height, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, stencil_buffer.data ());

    //�������� ��������� OpenGL

  CheckErrors (METHOD_NAME);
}

void DepthStencilBuffer::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::DepthStencilBuffer::GetData";

  if (!buffer)
    RaiseNullArgument (METHOD_NAME, "buffer");
    
  if (layer)
    RaiseOutOfRange (METHOD_NAME, "layer", layer, 1);
    
  if (mip_level)
    RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, 1);    
    
    //�������� ������� ��� �������� ����������� ������ ������� � ���������

  xtl::uninitialized_storage<float>         depth_buffer (width * height);
  xtl::uninitialized_storage<unsigned char> stencil_buffer (width * height);

    //��������� ������ � �������� OpenGL

  Bind ();

    //�����������

  glReadPixels (x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer.data ());
  glReadPixels (x, y, width, height, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, stencil_buffer.data ());

    //�������� ��������� OpenGL

  CheckErrors (METHOD_NAME);  

    //�������� ����������� ������

  size_t*              dst_pixel         = static_cast<size_t*> (buffer);
  const float*         src_depth_pixel   = depth_buffer.data ();
  const unsigned char* src_stencil_pixel = stencil_buffer.data ();

  for (size_t count=width*height; count--; dst_pixel++, src_depth_pixel++, src_stencil_pixel++)
    *dst_pixel = get_depth_stencil_pixel (*src_depth_pixel, *src_stencil_pixel);
}
