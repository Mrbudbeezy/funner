#ifndef RENDER_MANAGER_FRAME_HEADER
#define RENDER_MANAGER_FRAME_HEADER

#include <math/matrix.h>

#include <render/common.h>
#include <render/entity.h>
#include <render/render_target.h>

namespace render
{

//implementation forwards
class FrameImpl;
class RectAreaImpl;
class Wrappers;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ClearFlag
{
  ClearFlag_RenderTarget  = 1, //������� ������ �����
  ClearFlag_Depth         = 2, //������� ������ �������
  ClearFlag_Stencil       = 4, //������� ������ ���������
  ClearFlag_ViewportOnly  = 8, //������� ������ ������� ������ (����� ���� ������ ������������������)

  ClearFlag_DepthStencil = ClearFlag_Depth | ClearFlag_Stencil,
  ClearFlag_All          = ClearFlag_DepthStencil | ClearFlag_RenderTarget
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ������� (��� ����������� ������� ���������� �������� ������)
///////////////////////////////////////////////////////////////////////////////////////////////////
class RectArea
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RectArea  ();
    RectArea  (const render::Rect&);
    ~RectArea ();
    
    RectArea& operator = (const RectArea&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                SetRect (const render::Rect& rect);
    void                SetRect (int x, int y, size_t width, size_t height);
    const render::Rect& Rect    () const;
    int                 X       () const;
    int                 Y       () const;
    size_t              Width   () const;
    size_t              Height  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (RectArea&);
  
  private:
    RectArea (RectAreaImpl*);
    
  private:
    RectAreaImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (RectArea&, RectArea&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct EntityDrawParams
{
  common::PropertyMap properties; //�������� ���� �����-������
  math::mat4f         mvp_matrix; //������� model-view-projection
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Frame
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Frame  (const Frame&);
    ~Frame ();

    Frame& operator = (const Frame&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetRenderTarget        (const char* name, const render::RenderTarget& target);
    void SetRenderTarget        (const char* name, const render::RenderTarget& target, const RectArea& viewport);
    void SetRenderTarget        (const char* name, const render::RenderTarget& target, const RectArea& viewport, const RectArea& scissor);
    void RemoveRenderTarget     (const char* name);
    void RemoveAllRenderTargets ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool                  HasRenderTarget (const char* name) const;
    render::RenderTarget  RenderTarget    (const char* name) const;
    RectArea              Viewport        (const char* name) const;
    RectArea              Scissor         (const char* name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetScissorState (bool state);
    bool ScissorState    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetClearFlags (size_t clear_flags);
    size_t ClearFlags    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetClearColor (const math::vec4f& color);
    const math::vec4f& ClearColor    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetClearDepthValue   (float depth_value);
    void          SetClearStencilIndex (unsigned char stencil_index);
    float         ClearDepthValue      () const;
    unsigned char ClearStencilIndex    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetLocalTexture        (const char* name, const Texture& texture);
    void RemoveLocalTexture     (const char* name);
    void RemoveAllLocalTextures ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool    HasLocalTexture (const char* name) const;
    Texture LocalTexture    (const char* name) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetEffect (const char* name);
    const char* Effect    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetProperties (const common::PropertyMap&);
    const common::PropertyMap& Properties    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����-����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetShaderOptions (const common::PropertyMap&);
    const common::PropertyMap& ShaderOptions    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t EntitiesCount     () const;
    void   AddEntity         (const Entity& entity);
    void   RemoveAllEntities ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� (������� ��������� ������������ �� ����� ��� �������� �������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t FramesCount     () const;
    void   AddFrame        (const Frame& frame);
    void   RemoveAllFrames ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���� �������� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveAllFramesAndEntities ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������������� ���������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Frame& frame, Entity& entity, EntityDrawParams& out_params)> EntityDrawFunction;

    void                      SetEntityDrawHandler (const EntityDrawFunction& handler);
    const EntityDrawFunction& EntityDrawHandler    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Draw ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Frame&);
    
  private:
    Frame (FrameImpl*);
    
  private:
    FrameImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Frame&, Frame&);

}

#endif
