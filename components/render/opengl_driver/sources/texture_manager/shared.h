#ifndef RENDER_GL_DRIVER_TEXTURE_SHARED_HEADER
#define RENDER_GL_DRIVER_TEXTURE_SHARED_HEADER

#include <cmath>
#include <cfloat>

#include <stl/auto_ptr.h>

#include <xtl/array>
#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/shared_ptr.h>
#include <xtl/trackable_ptr.h>
#include <xtl/uninitialized_storage.h>

#include <common/hash.h>

#include <render/low_level/utils.h>

#include <shared/context_object.h>
#include <shared/texture_manager.h>

#include "nv_dxt/blockdxt.h"

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ��������� ���� ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TextureManagerCacheEntry
{
  CacheEntry_TextureId0 = CacheEntry_TextureManagerPrivateFirst,
  CacheEntry_TextureIdLast = CacheEntry_TextureId0 + DEVICE_SAMPLER_SLOTS_COUNT - 1,
  CacheEntry_TextureTarget0,
  CacheEntry_TextureTargetLast = CacheEntry_TextureTarget0 + DEVICE_SAMPLER_SLOTS_COUNT - 1,
};

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
///������ ���-������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TextureLevelData
{
  size_t      size; //������ ������
  const void* data; //����� � �������  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��� ������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class TextureDataSelector
{
  public: 
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextureDataSelector (const TextureDesc& desc, const TextureData* data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool GetLevelData (size_t width, size_t height, size_t depth, TextureLevelData&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� � ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Next ();
    
  private:
    PixelFormat        format;
    const TextureData* data;
    size_t             data_size;
    size_t             index;
    size_t             images_count;
    size_t             offset;
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
///��������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual PixelFormat GetFormat () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ��������� ���� ����������� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void   SetSamplerHash (size_t hash) = 0;
    virtual size_t GetSamplerHash () = 0;

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
    GLenum      GetTarget    () { return target; }      //��������� ���� ��������
    GLuint      GetTextureId () { return texture_id; }  //��������� �������������� ��������
    size_t      GetMipsCount () { return mips_count; }  //��������� ���������� mip-�������
    PixelFormat GetFormat    () { return desc.format; } //��������� ������� ��������

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ��������� ���� ����������� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetSamplerHash (size_t hash);
    size_t GetSamplerHash ();

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
    Texture  (const ContextManager&, const TextureDesc& desc, GLenum target, size_t mips_count);
    ~Texture ();
    
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
    void BuildMipmaps (size_t x, size_t y, size_t z, size_t width, size_t unclamped_width, size_t height, PixelFormat format, const void* data);
    
  private:
    TextureDesc   desc;                //���������� ��������  
    GLenum        target;              //��� ��������
    GLuint        texture_id;          //������������� ��������
    size_t        mips_count;          //���������� ���-�������
    size_t        binded_sampler_hash; //��� ����������� �������������� ��������
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
    Texture1D (const ContextManager&, const TextureDesc& texture_desc, const TextureData* data);

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
///���������� �������� ��� ����������� ��������� ����� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture1DNoSubimage: public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture1DNoSubimage (const ContextManager&, const TextureDesc&, const TextureData* data);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                            GLenum format, size_t buffer_size, const void* buffer);

  private:
    GLenum gl_internal_format;
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
    Texture2D (const ContextManager&, const TextureDesc&, const TextureData* data);

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
///��������� �������� ��� ����������� ��������� ����� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture2DNoSubimage: public Texture
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture2DNoSubimage (const ContextManager&, const TextureDesc&, const TextureData* data);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                              GLenum format, GLenum type, const void* buffer);
    void SetCompressedData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height,
                            GLenum format, size_t buffer_size, const void* buffer);

  private:
    GLenum gl_internal_format;
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
    TextureNpot (const ContextManager&, const TextureDesc& texture_desc, const TextureData* data);

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
    Texture3D (const ContextManager&, const TextureDesc&, const TextureData* data);

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
    TextureCubemap (const ContextManager&, const TextureDesc&, const TextureData* data);

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
    ScaledTexture (const ContextManager&, TextureManager&, const TextureDesc& original_desc, const TextureData* data, size_t scaled_width=0, size_t scaled_height=0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        GetDesc         (TextureDesc&);
    void        GetDesc         (RenderTargetTextureDesc&);
    void        GetDesc         (BindableTextureDesc&);
    void        GetMipLevelDesc (size_t level, MipLevelDesc& desc);
    PixelFormat GetFormat       ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ��������� ���� ����������� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetSamplerHash (size_t hash);
    size_t GetSamplerHash ();

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
    typedef xtl::com_ptr<BindableTexture>    TexturePtr;    
    typedef xtl::uninitialized_storage<char> Buffer;

    void ScaleImage (size_t width, size_t height, PixelFormat source_format, const void* src_buffer, Buffer& scaled_buffer, PixelFormat& scaled_format);

  private:
    TexturePtr    shadow_texture;   //������� ��������, �� ��������� �������� ������� ������
    size_t        original_width;   //������ ������������ ��������
    size_t        original_height;  //������ ������������ ��������
    float         horisontal_scale; //����������� ���������� �� �����������
    float         vertical_scale;   //����������� ���������� �� ���������
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
    SamplerState  (const ContextManager& manager, const SamplerDesc& desc);
    ~SamplerState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (GLenum texture_target, bool is_depth);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDesc (const SamplerDesc&);
    void GetDesc (SamplerDesc&);    

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    size_t GetDescHash ();
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
size_t      get_image_size                      (PixelFormat format, size_t width, size_t height, size_t depth); //��������� ������� �������� ������
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
