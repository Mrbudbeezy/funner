#ifndef RENDER_GL_DRIVER_TEXTURE_SHARED_HEADER
#define RENDER_GL_DRIVER_TEXTURE_SHARED_HEADER

#include <math.h>
#include <common/exception.h>
#include <shared/texture_manager.h>
#include <shared/context_object.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture : virtual public IBindableTexture, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture  (const ContextManager&, const TextureDesc&, GLenum target);
    ~Texture ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (TextureDesc&);
    void GetDesc (BindableTextureDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������� � ���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    GLenum GetTarget    () const { return target; }
    GLuint GetTextureId () const { return texture_id; }

  public:
    GLenum      target;      //������� ��� ��������
    GLuint      texture_id;  //������������� ��������
    TextureDesc desc;        //���������� ��������
    size_t      mips_count;  //���������� ���-�������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture1D : public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture1D (const ContextManager&, const TextureDesc& texture_desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer);
    void GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture2D : public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture2D (const ContextManager&, const TextureDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer);
    void GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture3D : public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture3D (const ContextManager&, const TextureDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer);
    void GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class TextureCubemap : public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextureCubemap (const ContextManager&, const TextureDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer);
    void GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
size_t texel_size           (PixelFormat format);  //������ ������� � ������
GLint  gl_internal_format   (PixelFormat format);  //���������� ������ OpenGL
GLenum gl_format            (PixelFormat format);  //������ OpenGL
GLenum gl_type              (PixelFormat format);  //��� OpenGL
bool   is_compressed_format (PixelFormat format);  //�������� �� ������ ������
size_t compressed_quad_size (PixelFormat format);  //������ ����� ������ �������� 4*4 � ������

void   scale_image_2x_down (PixelFormat format, size_t width, size_t height, const void* src, void* dest);

}

}

}

#endif
