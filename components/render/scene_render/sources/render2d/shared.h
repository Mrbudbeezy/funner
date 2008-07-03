#ifndef SCENE_GRAPH_RENDER2D_HEADER
#define SCENE_GRAPH_RENDER2D_HEADER

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>
#include <xtl/intrusive_ptr.h>

#include <common/component.h>

#include <sg/scene.h>
#include <sg/camera.h>
#include <sg/sprite.h>

#include <render/mid_level/renderer2d.h>

#include <render/custom_render.h>

namespace render
{

namespace render2d
{

/*///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� - ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class 

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� - ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SpritePrimitive
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SpritePrimitive  (scene_graph::Sprite*);
    ~SpritePrimitive ();
    
  private:
    scene_graph::Sprite* sprite; //������
};*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Render: public ICustomSceneRender, public xtl::reference_counter
{
  typedef mid_level::renderer2d::IPrimitive IPrimitive;
  typedef mid_level::renderer2d::IRenderer  IRenderer;
  typedef xtl::com_ptr<IPrimitive>          PrimitivePtr; 
  typedef xtl::com_ptr<IRenderer>           RendererPtr;
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
///��������� ����� ������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetBackgroundColor (const math::vec4f& color);
    void GetBackgroundColor (math::vec4f& color);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetLogHandler (const LogFunction&);
    const LogFunction& GetLogHandler ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();
    void Release ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
//    Primitive* FindPrimitive   (scene_graph::Entity*);
//    void       InsertPrimitive (scene_graph::Sprite*);
//    void       RemovePrimitive (scene_graph::Entity*);

  private:
//    typedef stl::hash_map<scene_graph::Entity*, PrimitivePtr> PrimitiveMap;

  private:
    math::vec4f  clear_color;      //���� �������
    LogFunction  log_handler;      //������� ����������������    
    RendererPtr  renderer;         //������� ����������
//    PrimitiveMap primitives_cache; //��� ����������
};

}

}

#endif
