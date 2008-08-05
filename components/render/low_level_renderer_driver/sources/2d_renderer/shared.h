#ifndef RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_2D_RENDERER_SHARED_HEADER
#define RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_2D_RENDERER_SHARED_HEADER

#include <stl/algorithm>

#include <xtl/common_exceptions.h>

#include <xtl/uninitialized_storage.h>

#include <common/log.h>

#include <media/image.h>

#include <render/low_level/utils.h>

#include <shared/shared.h>
#include <shared/2d_renderer.h>

namespace render
{

namespace mid_level
{

namespace low_level_driver
{

namespace renderer2d
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ImageTexture: virtual public mid_level::renderer2d::ITexture, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ImageTexture  (render::low_level::ITexture*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetWidth  ();
    size_t GetHeight ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::PixelFormat GetFormat ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ �������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CaptureImage (media::Image&);    

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::low_level::ITexture* GetTexture ();

  private:
    typedef xtl::com_ptr<render::low_level::ITexture> ITexturePtr;
    
  private:
    ITexturePtr texture;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� � ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetTexture: virtual public mid_level::renderer2d::ITexture, public RenderTarget
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetTexture (render::low_level::IView* render_target_view);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������, ����������� ��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SpriteVertexData
{
  struct SpriteVertex
  {
    math::vec3f position;
    math::vec2f texcoord;
  };

  SpriteVertex                     vertices[4];
  math::vec4f                      color;
  render::low_level::ITexture*     texture;
  mid_level::renderer2d::BlendMode blend_mode;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Primitive: virtual public mid_level::renderer2d::IPrimitive, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Primitive  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetTransform (const math::mat4f&);
    void GetTransform (math::mat4f&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                             SetTexture (mid_level::renderer2d::ITexture*);
    mid_level::renderer2d::ITexture* GetTexture ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::low_level::ITexture* GetLowLevelTexture () { return low_level_texture; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                             SetBlendMode (mid_level::renderer2d::BlendMode blend_mode);
    mid_level::renderer2d::BlendMode GetBlendMode ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetSpritesCount  ();
    void   GetSprite        (size_t index, mid_level::renderer2d::Sprite& sprite);
    size_t AddSprites       (size_t sprites_count, const mid_level::renderer2d::Sprite* sprites);
    void   RemoveSprites    (size_t first_sprite, size_t sprites_count);
    void   RemoveAllSprites ();
    void   ReserveSprites   (size_t sprites_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ��� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SpriteVertexData* GetSpriteVertexBuffer ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ������ ��� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BuildSpriteVertexData (size_t sprite_index);
    void ComputeSpriteTransorm (size_t sprite_index);

  private:
    typedef stl::vector<mid_level::renderer2d::Sprite>    SpriteArray;
    typedef xtl::uninitialized_storage<SpriteVertexData>  SpriteVertexArray;
    typedef xtl::com_ptr<mid_level::renderer2d::ITexture> TexturePtr;

  private:
    math::mat4f                      transform;
    TexturePtr                       texture;
    render::low_level::ITexture      *low_level_texture;
    mid_level::renderer2d::BlendMode blend_mode;
    SpriteArray                      sprites;
    SpriteVertexArray                sprite_vertex_buffer;
    bool                             dirty_transform;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ProgramParameters
{
  math::mat4f view_matrix;
  math::mat4f projection_matrix;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������, ����������� ��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderedSpriteVertex
{
  math::vec3f position;
  math::vec2f texcoord;
  math::vec4f color;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� 2D ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Frame: virtual public mid_level::renderer2d::IFrame, public BasicFrame
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Frame (CommonResources* in_common_resources, render::low_level::IDevice* device);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� / ������� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetView       (const math::mat4f&);
    void SetProjection (const math::mat4f&);
    void GetView       (math::mat4f&);
    void GetProjection (math::mat4f&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t PrimitivesCount ();
    void   AddPrimitive    (mid_level::renderer2d::IPrimitive*);
    void   Clear           ();

  private:
    typedef xtl::com_ptr<Primitive>                  PrimitivePtr;
    typedef stl::vector<PrimitivePtr>                PrimitiveArray;
    typedef stl::vector<SpriteVertexData*>           SpriteVertexArray;
    typedef xtl::com_ptr<render::low_level::IBuffer> BufferPtr;
    typedef xtl::com_ptr<CommonResources>            CommonResourcesPtr;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����� ��� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ReserveNotBlendedSpritesVertexBuffer (render::low_level::IDevice* device, size_t sprites_count);
    void ReserveBlendedSpritesVertexBuffer    (render::low_level::IDevice* device, size_t sprites_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void MakeVertexBuffer (SpriteVertexArray& vertex_data_array, BufferPtr& buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DrawCore (render::low_level::IDevice* device);

  private:
    math::mat4f                   view_tm, proj_tm;
    PrimitiveArray                primitives;
    CommonResourcesPtr            common_resources;
    BufferPtr                     not_blended_sprites_vertex_buffer;              //��������� ������ �������� ��� ���������
    BufferPtr                     blended_sprites_vertex_buffer;                  //��������� ������ �������� � ����������
    SpriteVertexArray             not_blended_sprites_vertex_data_buffer;         //����������� ������, �� �������� ����������� ������ ��� ���������
    SpriteVertexArray             blended_sprites_vertex_data_buffer;             //����������� ������, �� �������� ����������� ������ ��� ���������
    size_t                        current_not_blended_sprites_vertex_buffer_size; //������� ������ ���������� ������� �������� ��� ���������
    size_t                        current_blended_sprites_vertex_buffer_size;     //������� ������ ���������� ������� �������� � ����������
    render::low_level::BufferDesc vertex_buffer_desc;
    
    typedef xtl::uninitialized_storage<RenderedSpriteVertex> RenderedSpriteVertexArray;
    
    RenderedSpriteVertexArray vertex_buffer_cache;
};

}

}

}

}

#endif

