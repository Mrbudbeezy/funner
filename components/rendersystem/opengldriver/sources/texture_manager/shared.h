#ifndef RENDER_GL_DRIVER_TEXTURE_SHARED_HEADER
#define RENDER_GL_DRIVER_TEXTURE_SHARED_HEADER

#include <math.h>

#include <common/exception.h>

#include <xtl/trackable_ptr.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/shared_ptr.h>
#include <xtl/array>

#include <render/low_level/utils.h>

#include <shared/texture_manager.h>
#include <shared/context_object.h>

#include "nv_dxt/blockdxt.h"

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������, �������������� ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TextureExtensions
{  
  bool   has_ext_texture_compression_s3tc;   //GL_EXT_texture_compression_s3tc
  bool   has_sgis_generate_mipmap;           //GL_SGIS_generate_mipmap
  bool   has_ext_texture_rectangle;          //GL_EXT_texture_rectangle
  bool   has_ext_texture3d;                  //GL_EXT_texture3D
  bool   has_ext_packed_depth_stencil;       //GL_EXT_packed_depth_stencil
  bool   has_arb_multitexture;               //GL_ARB_multitexture
  bool   has_arb_texture_cube_map;           //GL_ARB_texture_cubemap
  bool   has_arb_texture_non_power_of_two;   //GL_ARB_texture_non_power_of_two
  bool   has_arb_depth_texture;              //GL_ARB_depth_texture
  bool   has_sgis_texture_lod;               //GL_SGIS_texture_lod
  bool   has_ext_texture_lod_bias;           //GL_EXT_texture_lod_bias
  bool   has_ext_texture_filter_anisotropic; //GL_EXT_texture_filter_anisotropic
  bool   has_ext_shadow_funcs;               //GL_EXT_shadow_funcs
  bool   has_arb_texture_border_clamp;       //GL_ARB_texture_border_clamp
  bool   has_arb_texture_mirrored_repeat;    //GL_ARB_texture_mirrored_repeat
  size_t max_anisotropy;                     //����������� ��������� ������� �����������

  TextureExtensions (const ContextManager&);
};

typedef xtl::shared_ptr<TextureExtensions> ExtensionsPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LayerDesc
{
  GLenum target;    //��� ��������
  size_t new_index; //������ ���� � ������ ���������� ����������� glGetTexImage
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� �������������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
struct BindableTextureDesc
{
  size_t target; //������� ��� ��������
  size_t id;     //������������� ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class BindableTexture: virtual public ITexture, virtual public IRenderTargetTexture, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    using ITexture::GetDesc;
    using IRenderTargetTexture::GetDesc;
    
    virtual void GetDesc (BindableTextureDesc&) = 0;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Bind () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ��������� ������ �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void   SetSamplerId (size_t id) = 0;
    virtual size_t GetSamplerId () = 0;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    BindableTexture (const ContextManager& context_manager) : ContextObject (context_manager) {}      
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture: public BindableTexture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc         (TextureDesc&);
    void GetDesc         (RenderTargetTextureDesc&);
    void GetDesc         (BindableTextureDesc&); 
    void GetMipLevelDesc (size_t level, MipLevelDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    GLenum      GetTarget    () const { return target; }      //��������� ���� ��������
    GLuint      GetTextureId () const { return texture_id; }  //��������� �������������� ��������
    size_t      GetMipsCount () const { return mips_count; }  //��������� ���������� mip-�������
    PixelFormat GetFormat    () const { return desc.format; } //��������� ������� ��������

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ��������� ������ �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetSamplerId (size_t id);
    size_t GetSamplerId ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer);
    void GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer);
    
  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture  (const ContextManager&, const ExtensionsPtr& extensions, const TextureDesc& desc, GLenum target, size_t mips_count);
    ~Texture ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const TextureExtensions& GetExtensions () const { return *extensions; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetFormat (PixelFormat);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GetLayerDesc (size_t layer, LayerDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                                      GLenum format, GLenum type, const void* buffer) = 0;
    virtual void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                                    GLenum format, size_t buffer_size, const void* buffer) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� mip-�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BuildMipmaps (size_t x, size_t y, size_t z, size_t width, size_t height, PixelFormat format, const void* data);
    
  private:
    TextureDesc   desc;              //���������� ��������  
    GLenum        target;            //��� ��������
    GLuint        texture_id;        //������������� ��������
    size_t        mips_count;        //���������� ���-�������
    size_t        binded_sampler_id; //������������� �������������� ��������
    ExtensionsPtr extensions;        //�������������� ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture1D: public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture1D (const ContextManager&, const ExtensionsPtr& extensions, const TextureDesc& texture_desc);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                            GLenum format, size_t buffer_size, const void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture2D: public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture2D (const ContextManager&, const ExtensionsPtr& extensions, const TextureDesc&);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                            GLenum format, size_t buffer_size, const void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ��������� �� ������� 2, ���������� ����� ���������� GL_EXT_texture_rectangle
///////////////////////////////////////////////////////////////////////////////////////////////////
class TextureNpot: public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextureNpot (const ContextManager&, const ExtensionsPtr& extensions, const TextureDesc& texture_desc);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                            GLenum format, size_t buffer_size, const void* buffer);    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture3D : public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture3D (const ContextManager&, const ExtensionsPtr& extensions, const TextureDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                            GLenum format, size_t buffer_size, const void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class TextureCubemap : public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextureCubemap (const ContextManager&, const ExtensionsPtr& extensions, const TextureDesc&);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetLayerDesc (size_t layer, LayerDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                            GLenum format, size_t buffer_size, const void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ���������, ����������������� �� ��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ScaledTexture: public BindableTexture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ScaledTexture (const ContextManager&, TextureManager&, const TextureDesc& original_desc, size_t scaled_width=0, size_t scaled_height=0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc         (TextureDesc&);
    void GetDesc         (RenderTargetTextureDesc&);
    void GetDesc         (BindableTextureDesc&);
    void GetMipLevelDesc (size_t level, MipLevelDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ��������� ������ �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetSamplerId (size_t id);
    size_t GetSamplerId ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer);
    void GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer);

  private:
    typedef xtl::com_ptr<BindableTexture> TexturePtr;

  private:
    TexturePtr    shadow_texture;   //������� ��������, �� ��������� �������� ������� ������
    size_t        original_width;   //������ ������������ ��������
    size_t        original_height;  //������ ������������ ��������
    float         horisontal_scale; //����������� ���������� �� �����������
    float         vertical_scale;   //����������� ���������� �� ���������
    ExtensionsPtr extensions;       //�������������� ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SamplerState : virtual public ISamplerState, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SamplerState (const ContextManager& manager, const ExtensionsPtr& extensions, const SamplerDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (GLenum texture_target);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDesc (const SamplerDesc&);
    void GetDesc (SamplerDesc&);

  private:
    SamplerDesc   desc;         //���������� ��������
    int           display_list; //����� ������� ������ ������ ������������ OpenGL (����� ������� OpenGLTextureTarget_Num)
    ExtensionsPtr extensions;   //�������������� ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
size_t      get_next_higher_power_of_two        (size_t size);        //��������� ��������� ������ ������� ������
GLint       get_gl_internal_format              (PixelFormat format); //���������� ������ OpenGL
GLenum      get_gl_format                       (PixelFormat format); //������ OpenGL
GLenum      get_gl_type                         (PixelFormat format); //��� OpenGL
GLenum      get_uncompressed_gl_internal_format (PixelFormat format); //������������� ���������� ������ OpenGL
GLenum      get_uncompressed_gl_format          (PixelFormat format); //������������� ������ OpenGL
GLenum      get_uncompressed_gl_type            (PixelFormat format); //������������� ��� OpenGL
PixelFormat get_pixel_format                    (GLenum gl_format);   //���������� ����������� ������� OpenGL

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
void scale_image_2x_down (PixelFormat format, size_t width, size_t height, const void* src, void* dest);
void scale_image         (PixelFormat format, size_t width, size_t height, size_t new_width, size_t new_height, const void* src, void* dest);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � DXT �������� ��� ���������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
void unpack_dxt (PixelFormat format, size_t width, size_t height, const void* dxt_data, void* unpacked_data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
void copy_image (size_t pixels_count, PixelFormat src_format, const void* src_buffer, PixelFormat dst_format, void* dst_buffer);

}

}

}

#endif
