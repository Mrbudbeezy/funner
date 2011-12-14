#include <cstdio>
#include <exception>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/iterator.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/log.h>

#include <sg/camera.h>
#include <sg/scene.h>

#include <render/scene_render.h>
#include <render/custom_render.h>
#include <render/mid_level/renderer.h>

using namespace render;
using namespace render::obsolete;
using namespace scene_graph;

typedef xtl::com_ptr<mid_level::IRenderTarget> RenderTargetPtr;
typedef xtl::com_ptr<IRenderQuery>             RenderQueryPtr;

///������� �뢮�� ७���
class MyRenderView: public IRenderView, public xtl::reference_counter
{
  public:
    MyRenderView (const RenderQueryPtr& in_query) : query (in_query)
    {
      static size_t global_id = 1;

      id = global_id++;

      printf ("MyRenderView #%u: Create view\n", id);
    }

    ~MyRenderView ()
    {
      printf ("MyRenderView #%u: Destroy view\n", id);
    }

///������ ����� ७��ਭ��
    void SetRenderTargets (mid_level::IRenderTarget* render_target, mid_level::IRenderTarget* depth_stencil_target)
    {
      printf ("MyRenderView #%u: SetRenderTargets(%s, %s)\n", id, render_target ? "render-target" : "null",
        depth_stencil_target ? "depth-stencil-target" : "null");
    }

    mid_level::IRenderTarget* GetRenderTarget ()
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetRenderTarget");
    }

    mid_level::IRenderTarget* GetDepthStencilTarget ()
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetDepthStencilTarget");
    }

///��⠭���� ������ �뢮��
    void SetViewport (const Rect& rect)
    {
      printf ("MyRenderView #%u: SetViewport(%d, %d, %u, %u)\n", id, rect.left, rect.top, rect.width, rect.height);
    }

    void GetViewport (Rect&)
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetViewport");
    }

///��⠭���� ������
    void SetCamera (scene_graph::Camera* camera)
    {
      printf ("MyRenderView #%u: SetCamera(%s)\n", id, camera ? camera->Name () : "null");
    }

    scene_graph::Camera* GetCamera ()
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetCamera");
    }

///��⠭���� / �⥭�� ᢮���
    void SetProperty (const char* name, const char* value)
    {
      printf ("MyRenderView #%u: SetProperty(%s, %s)\n", id, name, value);
    }

    void GetProperty (const char*, size_t, char*)
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetProperty");
    }

///��ᮢ����
    void Draw ()
    {
      printf ("MyRenderView #%u: Draw view\n", id);

      if (query)
        query->Update ();
    }

///������� ��뫮�
    void AddRef ()
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

  private:
    size_t         id;
    RenderQueryPtr query;
};

///������ �業�
class MySceneRender: public ICustomSceneRender, public xtl::reference_counter
{
  public:
    MySceneRender (mid_level::IRenderer* in_renderer) : need_create_render_query (true), renderer (in_renderer)
    {
      printf ("MySceneRender::MySceneRender\n");
    }

    ~MySceneRender ()
    {
      printf ("MySceneRender::~MySceneRender\n");
    }

///�������� �����⥩ �뢮��
    IRenderView* CreateRenderView (scene_graph::Scene* scene)
    {
      printf ("CreateRenderView('%s')\n", scene ? scene->Name () : "null");

      if (need_create_render_query)
      {
        RenderTargetPtr render_target (renderer->CreateRenderBuffer (400, 400), false),
                        depth_stencil_target (renderer->CreateDepthStencilBuffer (400, 400), false);

        RenderQueryPtr query (query_handler (render_target.get (), depth_stencil_target.get (), "test_query"), false);

        need_create_render_query = false;

        return new MyRenderView (query);
      }
      else return new MyRenderView (RenderQueryPtr ());
    }

///����� � ����ᠬ�
    void LoadResource (const char* tag, const char* file_name)
    {
      printf ("LoadResource(%s, %s)\n", tag, file_name);
    }

    void UnloadResource (const char* tag, const char* file_name)
    {
      printf ("UnloadResource(%s, %s)\n", tag, file_name);
    }

///��⠭���� �㭪樨 �⫠��筮�� ��⮪���஢����
    void SetLogHandler (const ICustomSceneRender::LogFunction&) {}

    const ICustomSceneRender::LogFunction& GetLogHandler ()
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetLogHandler");
    }

    void SetQueryHandler (const QueryFunction& in_handler)
    {
      query_handler = in_handler;
    }

    const QueryFunction& GetQueryHandler () { return query_handler; }

///������� ��뫮�
    void AddRef ()
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

///�������� ७���
    static ICustomSceneRender* Create (mid_level::IRenderer* renderer, const char* path_name)
    {
      printf ("MySceneRender::Create(%s, %s)\n", renderer ? renderer->GetDescription () : "null", path_name);

      return new MySceneRender (renderer);
    }

  private:
    bool                  need_create_render_query;
    mid_level::IRenderer* renderer;
    QueryFunction         query_handler;
};
