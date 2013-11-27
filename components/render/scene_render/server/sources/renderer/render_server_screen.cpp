#include "shared.h"

using namespace render;
using namespace render::scene::server;

//TODO: screen background

namespace
{

/*
    ���������
*/

const size_t RESERVED_VIEWPORTS_COUNT                     = 8;                        //������������� ���������� �������� ������
const char*  COLOR_BINDING_PROPERTY_NAME                  = "color_binding";          //��� �������� �������� ������ �����
const char*  DEPTH_STENCIL_BINDING_PROPERTY_NAME          = "depth_stencil_binding";  //��� �������� �������� ������ �������
const char*  DEFAULT_COLOR_BINDING_PROPERTY_VALUE         = "default_color";          //�������� �� ��������� �������� �������� ������ �����
const char*  DEFAULT_DEPTH_STENCIL_BINDING_PROPERTY_VALUE = "default_depth_stencil";  //�������� �� ��������� ������ �������

/*
    ��������������� ���������
*/

/// ���������� ������� ������
struct ViewportDesc: public xtl::reference_counter, public IViewportListener
{
  Viewport    viewport;     //������� ������
  int         zorder;       //������� ���������
  object_id_t id;           //������������� ������� ������
  bool&       need_reorder; //���� ������������� ���������� �������� ������

/// �����������
  ViewportDesc (const Viewport& in_viewport, object_id_t in_id, bool& in_need_reorder)
    : viewport (in_viewport)
    , zorder (viewport.ZOrder ())
    , id (in_id)
    , need_reorder (in_need_reorder)
  {
    viewport.AttachListener (this);

    need_reorder = true;
  }

/// ����������
  ~ViewportDesc ()
  {
    viewport.DetachListener (this);
  }

/// ���������� �� ��������� ������� ���������
  void OnViewportZOrderUpdated (int in_zorder)
  {
    zorder       = in_zorder;
    need_reorder = true;
  }
};

typedef xtl::intrusive_ptr<ViewportDesc> ViewportPtr;
typedef stl::vector<ViewportPtr>         ViewportDescList;

}

/*
    �������� ���������� ������
*/

struct Screen::Impl: public xtl::reference_counter
{
  RenderManager    render_manager;    //�������� ����������
  RenderTargetMap  render_targets;    //���� ����������
  stl::string      name;              //��� ������
  bool             active;            //���������� ������
  Rect             area;              //������� ������
  bool             background_state;  //��������� ������� ����
  math::vec4f      background_color;  //���� ������� ����
  bool             need_reorder;      //������������� �������������� �������� ������
  ViewportManager  viewport_manager;  //�������� �������� ������
  ViewportDescList viewports;         //������� ������
  ViewportDrawList drawing_viewports; //�������������� ������� ������
  manager::Window* window;            //��������� ����

/// �����������
  Impl (const char* in_name, const char* init_string, WindowManager& window_manager, const RenderManager& in_render_manager, const ViewportManager& in_viewport_manager)
    : render_manager (in_render_manager)
    , name (in_name)
    , active (true)
    , area (0, 0, 100, 100)
    , background_state (true)
    , background_color (0, 0, 0, 1.0f)
    , need_reorder (false)
    , viewport_manager (in_viewport_manager)
    , drawing_viewports (viewport_manager.DrawingViewports ())
    , window ()
  {
    viewports.reserve (RESERVED_VIEWPORTS_COUNT);

    InitRenderTargets (window_manager, init_string);
  }

/// ����������
  ~Impl ()
  {
    for (ViewportDescList::iterator iter=viewports.begin (), end=viewports.end (); iter!=end; ++iter)
      viewport_manager.RemoveViewport ((*iter)->id);

    viewports.clear ();
  }

/// ������������� ����� ���������
  void InitRenderTargets (WindowManager& window_manager, const char* init_string)
  {
    try
    {
      this->window = 0;

        //����� ����

      manager::Window& window = window_manager.GetWindow (name.c_str ());

        //���������� ��������� ���� ���������

      common::PropertyMap properties = common::parse_init_string (init_string);

      const char *color_binding         = properties.IsPresent (COLOR_BINDING_PROPERTY_NAME) ? properties.GetString (COLOR_BINDING_PROPERTY_NAME) : DEFAULT_COLOR_BINDING_PROPERTY_VALUE,
                 *depth_stencil_binding = properties.IsPresent (DEPTH_STENCIL_BINDING_PROPERTY_NAME) ? properties.GetString (DEPTH_STENCIL_BINDING_PROPERTY_NAME) : DEFAULT_DEPTH_STENCIL_BINDING_PROPERTY_VALUE;

      render_targets.Add (color_binding, window.ColorBuffer (), area);
      render_targets.Add (depth_stencil_binding, window.DepthStencilBuffer (), area);

      this->window = &window;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Screen::Impl::InitRenderTargets");
      throw;
    }
  }

/// ���������� �������� ������
  struct ViewComparator
  {
    bool operator () (const ViewportPtr& viewport1, const ViewportPtr& viewport2) const
    {
      return viewport1->zorder > viewport2->zorder;
    }
  };

/// ���������� �������� ������
  void Reorder ()
  {
    if (!need_reorder)
      return;

    stl::sort (viewports.begin (), viewports.end (), ViewComparator ());

    need_reorder = false;
  }
};

/*
    ������������ / ���������� / ������������
*/

Screen::Screen (const char* name, const char* init_string, WindowManager& window_manager, const RenderManager& render_manager, const ViewportManager& viewport_manager)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    if (!init_string)
      init_string = "";

    impl = new Impl (name, init_string, window_manager, render_manager, viewport_manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Screen::Screen");
    throw;
  }
}

Screen::Screen (const Screen& s)
  : impl (s.impl)
{
  addref (impl);
}

Screen::~Screen ()
{
  release (impl);
}

Screen& Screen::operator = (const Screen& s)
{
  Screen tmp (s);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    ��� ������
*/

const char* Screen::Name () const
{
  return impl->name.c_str ();
}

/*
    ���������� ������
*/

void Screen::SetActive (bool active)
{
  impl->active = active;
}

bool Screen::IsActive () const
{
  return impl->active;
}

/*
    ������� ������
*/

void Screen::SetArea (const Rect& rect)
{
  try
  {
    impl->area = rect;

    impl->render_targets.SetArea (rect);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Screen::SetArea");
    throw;
  }
}

const Rect& Screen::Area () const
{
  return impl->area;
}

/*
    ������ ���
*/

void Screen::SetBackground (bool state, const math::vec4f& color)
{
  impl->background_state = state;
  impl->background_color = color;

//??????????????????
}

bool Screen::BackgroundState () const
{
  return impl->background_state;
}

const math::vec4f& Screen::BackgroundColor () const
{
  return impl->background_color;
}

/*
    ���������� � ��������� ������
*/

Viewport Screen::AttachViewport (object_id_t id)
{
  try
  {
    ViewportPtr viewport (new ViewportDesc (Viewport (impl->render_manager, impl->render_targets, impl->viewport_manager.DrawingViewports (), impl->viewport_manager.MaxDrawDepth ()), id, impl->need_reorder), false);

    impl->viewport_manager.AddViewport (id, viewport->viewport);

    try
    {
      impl->viewports.push_back (viewport);

      return viewport->viewport;
    }
    catch (...)
    {
      impl->viewport_manager.RemoveViewport (id);
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Screen::AttachViewport");
    throw;
  }
}

void Screen::DetachViewport (object_id_t id)
{
  for (ViewportDescList::iterator iter=impl->viewports.begin (), end=impl->viewports.end (); iter!=end; ++iter)
    if ((*iter)->id == id)
    {
      impl->viewports.erase (iter);
      break;
    }

  impl->viewport_manager.RemoveViewport (id);
}

/*
    ����������
*/

void Screen::Update (manager::Frame* parent_frame)
{
  try
  {
    if (!impl->active)
      return;

      //���������� �������� ������

    if (impl->need_reorder)
      impl->Reorder ();

      //����� �������� ������

    for (ViewportDescList::iterator iter=impl->viewports.begin (), end=impl->viewports.end (); iter!=end; ++iter)
    {
      Viewport& viewport = (*iter)->viewport;

      try
      {
        viewport.Update (parent_frame);
      }
      catch (std::exception& e)
      {
        impl->render_manager.Log ().Printf ("%s\n    at render::scene::server::Screen::Update(screen='%s')", e.what (), impl->name.c_str ());
      }
      catch (...)
      {
        impl->render_manager.Log ().Printf ("unknown exception\n    at render::scene::server::Screen::Update(screen='%s')", impl->name.c_str ());
      }      
    }

      //������� ������

    if (!parent_frame)
      impl->drawing_viewports.CleanupViewports ();

     //����� �� �����

    if (impl->window)
      impl->window->SwapBuffers ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Screen::Update");
    throw;
  }
}
