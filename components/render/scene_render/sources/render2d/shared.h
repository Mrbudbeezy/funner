#ifndef SCENE_GRAPH_RENDER2D_HEADER
#define SCENE_GRAPH_RENDER2D_HEADER

#include <stl/hash_map>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/connection.h>
#include <xtl/bind.h>
#include <xtl/visitor.h>
#include <xtl/iterator.h>
#include <xtl/token_parser.h>
#include <xtl/trackable.h>

#include <common/component.h>
#include <common/strlib.h>

#include <media/image.h>
#include <media/font.h>
#include <media/rfx/material_library.h>
#include <media/rfx/sprite_material.h>

#include <sg/camera.h>
#include <sg/scene.h>
#include <sg/sprite.h>
#include <sg/text_line.h>

#include <render/mid_level/renderer2d.h>

#include <render/custom_render.h>

namespace render
{

namespace render2d
{

//forward declaration
class Render;

typedef mid_level::renderer2d::IPrimitive  IPrimitive;
typedef mid_level::renderer2d::IRenderer   IRenderer;
typedef mid_level::renderer2d::IFrame      IFrame;
typedef mid_level::renderer2d::ITexture    ITexture;
typedef mid_level::IRenderTarget           IRenderTarget;
typedef mid_level::renderer2d::BlendMode   BlendMode;
typedef media::rfx::SpriteMaterial         SpriteMaterial;
typedef xtl::com_ptr<IPrimitive>           PrimitivePtr;
typedef xtl::com_ptr<IRenderer>            RendererPtr;
typedef xtl::com_ptr<IFrame>               FramePtr;
typedef xtl::com_ptr<ITexture>             TexturePtr;
typedef xtl::com_ptr<IRenderTarget>        RenderTargetPtr;
typedef xtl::com_ptr<IRenderQuery>         RenderQueryPtr;
typedef SpriteMaterial::Pointer            SpriteMaterialPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Renderable: public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
            Renderable  (scene_graph::Entity*);
    virtual ~Renderable () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Draw (IFrame& frame);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �� ���������� � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Update () {}
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void DrawCore (IFrame& frame) = 0;
    
  private:
    void UpdateNotify ();
  
  private:
    xtl::auto_connection on_update_connection;  //���������� �� ������ ���������� �� ���������� �������
    bool                 need_update;           //���� ������������� ���������� ���������� �������� ������ �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������, ��������� �� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderableSpriteModel: public Renderable, public xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderableSpriteModel (scene_graph::SpriteModel* model, Render& render);
    ~RenderableSpriteModel ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update ();
    void UpdateMaterialNotify ();
    void UpdateSpritesNotify ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DrawCore (IFrame&);

  private:
    Render&                   render;                          //������ �� ������
    scene_graph::SpriteModel* model;                           //�������� ������
    PrimitivePtr              primitive;                       //��������������� ��������
    RenderQueryPtr            query;                           //������ ��������� ����������
    bool                      need_update_sprites;             //���� ������������� ���������� ������� ��������
    size_t                    tile_columns;                    //���������� �������� ������
    float                     tile_tex_width, tile_tex_height; //������� ����� � ���������� �����������
    size_t                    current_world_tm_hash;           //��� ������� ������� �������������
    size_t                    current_material_name_hash;      //��� �������� ����� ���������
    float                     current_alpha_reference;         //������� �������� ��������� �����-���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� � ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderableFont
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderableFont (const char* font_name, Render& in_render);
    ~RenderableFont ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const media::Font& GetFont () const { return font; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture* GetTexture () const { return texture.get (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����� ������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetMaxGlyphSide () const { return max_glyph_side; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const render::mid_level::renderer2d::Sprite& GetSprite (size_t index) const;

  private:
    typedef xtl::uninitialized_storage<render::mid_level::renderer2d::Sprite> SpritesBuffer;
    typedef xtl::com_ptr<ITexture>                                            TextuerPtr;

  private:
    Render&       render;                       //������ �� ������
    media::Font   font;                         //�����
    SpritesBuffer sprites_buffer;               //������ �������� (�� ������� ��� ������� �����)
    TexturePtr    texture;                      //��������
    size_t        max_glyph_side;               //������ ����� ������� ������� ����� ���� ������ (� ��������)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderableTextLine: public Renderable, public xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderableTextLine (scene_graph::TextLine* text_line, Render& render);
    ~RenderableTextLine ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PreprocessFont ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DrawCore (IFrame&);

  private:
    typedef xtl::uninitialized_storage<render::mid_level::renderer2d::Sprite> SpritesBuffer;

  private:
    Render&                        render;                       //������ �� ������
    scene_graph::TextLine*         text_line;                    //�������� ����� ������
    PrimitivePtr                   primitive;                    //��������������� ��������
    math::mat4f                    translate_tm;                 //������� ������ ��� ������������
    math::mat4f                    current_world_tm;             //������� ������� �������������
    size_t                         current_world_tm_hash;        //��� ������� ������� �������������
    stl::string                    current_font_name;            //������� �����
    stl::string                    current_text;                 //������� �����
    math::vec4f                    current_color;                //������� ����
    scene_graph::TextLineAlignment current_horizontal_alignment; //������� �������������� ������������
    scene_graph::TextLineAlignment current_vertical_alignment;   //������� ������������ ������������
    SpritesBuffer                  sprites_buffer;               //������ ��������
    stl::vector<size_t>            glyph_indices;                //������� ������ � ������� ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderView: public IRenderView, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderView  (scene_graph::Scene*, Render*);
    ~RenderView ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void           SetRenderTargets      (IRenderTarget* render_target, IRenderTarget* depth_stencil_target);
    IRenderTarget* GetRenderTarget       ();
    IRenderTarget* GetDepthStencilTarget ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetViewport (const Rect& rect);
    void GetViewport (Rect& out_rect);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                 SetCamera (scene_graph::Camera*);
    scene_graph::Camera* GetCamera ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetProperty (const char* name, const char* value);
    void GetProperty (const char* name, size_t buffer_size, char* value_buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Draw ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();
    void Release ();

  private:
    typedef xtl::intrusive_ptr<Render> RenderPtr;
    
  private:
    RenderPtr            render; //������
    FramePtr             frame;  //����
    scene_graph::Scene*  scene;  //�����
    scene_graph::Camera* camera; //������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Render: public ICustomSceneRender, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Render  (mid_level::IRenderer*);
    ~Render ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IRenderView* CreateRenderView (scene_graph::Scene* scene);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource (const char* tag, const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetLogHandler (const LogFunction&);
    const LogFunction& GetLogHandler ();

    void LogPrintf (const char* format, ...);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                 SetQueryHandler (const QueryFunction&);
    const QueryFunction& GetQueryHandler ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();
    void Release ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const RendererPtr& Renderer () const { return renderer; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Renderable*     GetRenderable (scene_graph::SpriteModel*);  // ������������!!!
    Renderable*     GetRenderable (scene_graph::TextLine*);     // ������������!!!
    ITexture*       GetTexture    (const char* file_name, bool need_alpha, RenderQueryPtr& out_query);
    SpriteMaterial* GetMaterial   (const char* name);
    RenderableFont* GetFont       (const char* name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddFrame (IFrame*);

  private:
    typedef xtl::intrusive_ptr<Renderable> RenderablePtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void InsertRenderable (scene_graph::Entity*, const RenderablePtr&);
    void RemoveRenderable (scene_graph::Entity*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadMaterialLibrary (const char* file_name);
    void InsertMaterial      (const char* id, const SpriteMaterialPtr&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TexturePtr CreateTexture        (const char* file_name, bool need_alpha, bool& has_alpha, RenderQueryPtr& out_query);
    TexturePtr CreateDynamicTexture (const char* name, RenderQueryPtr& out_query);

  private:
    struct RenderableHolder
    {
      RenderablePtr        renderable;
      xtl::auto_connection on_destroy;

      RenderableHolder (const RenderablePtr& in_renderable, const xtl::connection& in_on_destroy) :
        renderable (in_renderable), on_destroy (in_on_destroy) {}
    };
    
    struct TextureHolder
    {
      TexturePtr     base_texture;  //������� ��������
      TexturePtr     alpha_texture; //�����-��������
      RenderQueryPtr query;         //������ ��������� ����������

      TextureHolder (const TexturePtr& in_base_texture, const TexturePtr& in_alpha_texture, const RenderQueryPtr& in_query) :
        base_texture (in_base_texture), alpha_texture (in_alpha_texture), query (in_query) {}
    };

    typedef stl::hash_map<scene_graph::Entity*, RenderableHolder>        RenderableMap;
    typedef stl::hash_map<stl::hash_key<const char*>, SpriteMaterialPtr> MaterialMap;
    typedef stl::hash_map<stl::hash_key<const char*>, TextureHolder>     TextureMap;

    typedef xtl::shared_ptr<RenderableFont>                              RenderableFontPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, RenderableFontPtr> RenderableFontMap;

  private:
    LogFunction       log_handler;       //������� ����������������
    QueryFunction     query_handler;     //������� �������� �������� ��������
    RendererPtr       renderer;          //������� ����������
    RenderableMap     renderables_cache; //��� ��������������� ��������
    MaterialMap       materials;         //���������
    TextureMap        textures;          //��������
    RenderableFontMap fonts;             //������
};

}

}

#endif
