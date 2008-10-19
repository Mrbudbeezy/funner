#include "shared.h"

using namespace render;
using namespace render::render2d;
using namespace scene_graph;

/*
    ���������
*/

namespace
{

const char* DYNAMIC_STRING_MARKER = "dynamic"; //������ ������������ ��������

}

/*
    ����������� / ����������
*/

Render::Render (mid_level::IRenderer* in_renderer)
{
  static const char* METHOD_NAME = "render::render2d::Render::Render";

  if (!in_renderer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "renderer");

  IRenderer* casted_renderer = dynamic_cast<IRenderer*> (in_renderer);

  if (!casted_renderer)
    throw xtl::make_argument_exception (METHOD_NAME, "renderer", typeid (in_renderer).name (),
      "Renderer type is incompatible with render::renderer2d::IRenderer");

  renderer = casted_renderer;
}

Render::~Render ()
{
}

/*
    ������� ������
*/

void Render::AddRef ()
{
  addref (this);
}

void Render::Release ()
{
  release (this);
}

/*
    �������� �������� ������
*/

IRenderView* Render::CreateRenderView (scene_graph::Scene* scene)
{
  return new RenderView (scene, this);
}

/*
    ��������� ������� ����������� ����������������
*/

void Render::SetLogHandler (const LogFunction& log)
{
  log_handler = log;
}

const Render::LogFunction& Render::GetLogHandler ()
{
  return log_handler;
}

void Render::LogPrintf (const char* format, ...)
{
  va_list args;
  
  va_start (args, format);
  
  try
  {
    log_handler (common::vformat (format, args).c_str ());
  }
  catch (...)
  {
  }
}

/*
    �������� �������� ��������
*/

void Render::SetQueryHandler (const QueryFunction& in_query_handler)
{
  query_handler = in_query_handler;
}

const ICustomSceneRender::QueryFunction& Render::GetQueryHandler ()
{
  return query_handler;
}

/*
    ������ � ���������
*/

void Render::LoadResource (const char* tag, const char* file_name)
{
  try
  {
    const char* MATERIAL_LIBRARY_TAG = "material_library";      

    if (!strcmp (tag, "auto"))
    {
        //�������������� ����������� ����

      if (common::suffix (file_name) == ".xmtl")
        tag = MATERIAL_LIBRARY_TAG;
    }

      //��������������� �����
      
    if (!strcmp (tag, MATERIAL_LIBRARY_TAG))
    {
      LoadMaterialLibrary (file_name);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::Render::LoadResource");
    throw;
  }
}

void Render::LoadMaterialLibrary (const char* file_name)
{
  try
  {
    media::rfx::MaterialLibrary library (file_name, log_handler);
    
    for (media::rfx::MaterialLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
    {
      media::rfx::Material::Pointer material        = *iter;
      media::rfx::SpriteMaterial*   sprite_material = dynamic_cast<media::rfx::SpriteMaterial*> (&*material);

      if (sprite_material)
        InsertMaterial (library.ItemId (iter), sprite_material);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::Render::LoadMaterialLibrary(file_name='%s')", file_name);
    throw;
  }
}

void Render::InsertMaterial (const char* id, const media::rfx::SpriteMaterial::Pointer& material)
{
  materials [id] = material;
}

SpriteMaterial* Render::GetMaterial (const char* name)
{
  static const char* METHOD_NAME = "render::render2d::Render::GetMaterial";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

    //������� ����� ������ � ����

  MaterialMap::iterator iter = materials.find (name);

  if (iter != materials.end ())
    return iter->second.get ();

  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Material not found");
}

RenderableFont* Render::GetFont (const char* name)
{
  static const char* METHOD_NAME = "render::render2d::Render::GetFont";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

    //������� ����� ������ � ����

  RenderableFontMap::iterator iter = fonts.find (name);

  if (iter != fonts.end ())
    return iter->second.get ();

  RenderableFontPtr new_font = RenderableFontPtr (new RenderableFont (name, *this));

  fonts[name] = new_font;

  return fonts[name].get ();
}

/*
    ������ � �����
*/

Renderable* Render::GetRenderable (scene_graph::SpriteModel* entity)
{
    //������� ����� ������ � ����

  RenderableMap::iterator iter = renderables_cache.find (entity);
  
  if (iter != renderables_cache.end ())
    return iter->second.renderable.get ();

    //�������� ������ �������

  RenderablePtr renderable (new RenderableSpriteModel (entity, *this), false);

  InsertRenderable (entity, renderable);

  return &*renderable;
}

Renderable* Render::GetRenderable (scene_graph::TextLine* entity)
{
    //������� ����� ������ � ����

  RenderableMap::iterator iter = renderables_cache.find (entity);
  
  if (iter != renderables_cache.end ())
    return iter->second.renderable.get ();

    //�������� ������ �������

  RenderablePtr renderable (new RenderableTextLine (entity, *this), false);

  InsertRenderable (entity, renderable);

  return &*renderable;
}

void Render::InsertRenderable (scene_graph::Entity* entity, const RenderablePtr& renderable)
{  
  renderables_cache.insert_pair (entity, RenderableHolder (renderable,
    entity->RegisterEventHandler (NodeEvent_AfterDestroy, xtl::bind (&Render::RemoveRenderable, this, entity))));
}

void Render::RemoveRenderable (scene_graph::Entity* entity)
{
  renderables_cache.erase (entity);
}

ITexture* Render::GetTexture (const char* file_name, bool need_alpha, RenderQueryPtr& out_query)
{
    //������� ����� �������� � ����

  TextureMap::iterator iter = textures.find (file_name);
  
  if (iter != textures.end ())
  {
    TextureHolder& holder = iter->second;
    
    out_query = holder.query;
    
      //���� �����-����� �� ��������� - ���������� ������� ��������

    if (!need_alpha)
      return holder.base_texture.get ();

      //���� ��������� ����� ����� � ��������������� �������� ������������ - ���������� �

    if (holder.alpha_texture)
      return holder.alpha_texture.get ();

      //���� �����-�������� ����������� - ������ � � ����������
      
    bool has_alpha = false;
    
    RenderQueryPtr tmp_query;

    holder.alpha_texture = CreateTexture (file_name, true, has_alpha, tmp_query);

    return holder.alpha_texture.get ();
  }

  try
  {        
      //�������� ����� ��������
      
    bool has_alpha = false;
      
    TexturePtr texture = CreateTexture (file_name, need_alpha, has_alpha, out_query);
    
      //���������� �������� � ���

    textures.insert_pair (file_name, TextureHolder (texture, has_alpha ? texture : TexturePtr (), out_query));

    return texture.get ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::Render::GetTexture");
    throw;
  }
}

/*
    �������� �������
*/

TexturePtr Render::CreateTexture (const char* file_name, bool need_alpha, bool& has_alpha, RenderQueryPtr& out_query)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
      
      //�������� ����� ������������ ��������      
      
    if (!xtl::xstrncmp (file_name, DYNAMIC_STRING_MARKER, strlen (DYNAMIC_STRING_MARKER)))
    {
      has_alpha = true;

      return CreateDynamicTexture (file_name, out_query);
    }
    
      //���������� ������������ �������� - ���������� ������� ��������� ����������
    
    out_query = 0;
    
      //�������� ��������
      
    bool need_alpha_generation = false;

    media::Image image (file_name);

    switch (image.Format ())
    {
      case media::PixelFormat_BGR8:
        image.Convert (need_alpha ? media::PixelFormat_RGBA8 : media::PixelFormat_RGB8);
        need_alpha_generation = need_alpha;
        break;
      case media::PixelFormat_BGRA8:
        image.Convert (media::PixelFormat_RGBA8);
        has_alpha = true;
        break;
      case media::PixelFormat_RGBA8:
      case media::PixelFormat_A8:      
      case media::PixelFormat_L8:
      case media::PixelFormat_LA8:
        has_alpha = true;
        break;
      case media::PixelFormat_RGB8:
        if (need_alpha)
          image.Convert (media::PixelFormat_RGBA8);

        need_alpha_generation = need_alpha;

        break;
      default:
        throw xtl::format_operation_exception ("", "Unknown texture format=%d", image.Format ());
    }
    
    /*
      ���� ����� ��������� �����-������ - ���������� ����� ����� �������, ��� ��� ������� � ������ ������� ������
      ����� alpha = 0, � ��������� - alpha = 255
    */

    if (need_alpha_generation)
    {           
      #pragma pack(1)      

      struct RGBA
      {
        unsigned char red, green, blue, alpha;
      };

      RGBA* pixel       = reinterpret_cast<RGBA*> (image.Bitmap ());
      RGBA  alpha_color = *pixel;
      
      for (size_t count=image.Width () * image.Height (); count--; pixel++)
      {
        if (pixel->red == alpha_color.red && pixel->green == alpha_color.green && pixel->blue == alpha_color.blue)
          pixel->alpha = 0;
        else
          pixel->alpha = 255;
      }
      
      has_alpha = true;
    }
    

      //�������� ��������
      
    return TexturePtr (renderer->CreateTexture (image), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::Render::CreateTexture(file_name='%s', renderer='%s')", file_name, renderer->GetDescription ());
    throw;
  }
}

TexturePtr Render::CreateDynamicTexture (const char* name, RenderQueryPtr& out_query)
{
    //������ ������� ������������� ����������

  common::StringArray tokens = common::parse (name , "dynamic *\\( *([[:digit:]]*) *, *([[:digit:]]*) *, *'(.*)' *\\).*");

  if (tokens.Size () != 4)
    throw xtl::make_argument_exception ("render::render2d::Render::CreateDynamicTexture", "name", name,
      "Wrong format. Expected: dynamic(<width>,<height>,'<query_string>')");      

  size_t texture_width  = xtl::io::get<size_t> (tokens [1]),
         texture_height = xtl::io::get<size_t> (tokens [2]);

  const char* query_string = tokens [3];
  
    //�������� ������������ �������� � ���������������� ������ �������

  TexturePtr       texture (renderer->CreateTexture (texture_width, texture_height, render::mid_level::PixelFormat_RGBA8), false);    
  RenderTargetPtr  depth_stencil_buffer (renderer->CreateDepthStencilBuffer (texture_width, texture_height), false);
                  
    //�������� �������

  out_query = RenderQueryPtr (query_handler (texture.get (), depth_stencil_buffer.get (), query_string), false);
  
  return texture;
}

/*
    ���������� ����� �� ���������    
*/

void Render::AddFrame (IFrame* frame)
{
  renderer->AddFrame (frame);
}
