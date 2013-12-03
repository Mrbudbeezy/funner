#include "shared.h"

using namespace render;
using namespace render::scene::server;

namespace
{

/*
    ���������
*/

const size_t RESERVED_VIEWPORTS_COUNT                       = 8;                         //������������� ���������� �������� ������
const char*  DEPTH_STENCIL_BINDING_PROPERTY_NAME            = "depth_stencil_binding";   //��� �������� �������� ������ �������
const char*  COLOR_BINDING_PROPERTY_NAME                    = "color_binding";           //��� �������� �������� ������ �����
const char*  CLEARING_EFFECT_BINDING_PROPERTY_NAME          = "clearing_effect_binding"; //��� ������� ������� ������
const char*  DEFAULT_COLOR_BINDING_PROPERTY_VALUE           = "default_color";           //�������� �� ��������� �������� �������� ������ �����
const char*  DEFAULT_DEPTH_STENCIL_BINDING_PROPERTY_VALUE   = "default_depth_stencil";   //�������� �� ��������� ������ �������
const char*  DEFAULT_CLEARING_EFFECT_BINDING_PROPERTY_VALUE = "clearing";                //�������� �� ��������� ����� ������� ������� ������

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
  RenderManager                 render_manager;              //�������� ����������
  RenderTargetMap               render_targets;              //���� ����������
  stl::string                   name;                        //��� ������
  bool                          active;                      //���������� ������
  Rect                          area;                        //������� ������
  bool                          background_state;            //��������� ������� ����
  math::vec4f                   background_color;            //���� ������� ����
  bool                          need_reorder;                //������������� �������������� �������� ������
  ViewportManager               viewport_manager;            //�������� �������� ������
  ViewportDescList              viewports;                   //������� ������
  ViewportDrawList              drawing_viewports;           //�������������� ������� ������
  manager::Window*              window;                      //��������� ����
  xtl::auto_connection          on_window_resize_connection; //���������� � �������� ���������� �� ��������� �������� ����
  xtl::auto_connection          on_window_update_connection; //���������� � �������� ���������� �� ���������� ����
  stl::auto_ptr<manager::Frame> clear_frame;                 //���� �������

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

    common::PropertyMap properties = common::parse_init_string (init_string);

      //������������� ���������� �������

    InitClearing (properties);

      //������������� ����� ����������

    InitRenderTargets (window_manager, properties);
  }

/// ����������
  ~Impl ()
  {
    for (ViewportDescList::iterator iter=viewports.begin (), end=viewports.end (); iter!=end; ++iter)
      viewport_manager.RemoveViewport ((*iter)->id);

    viewports.clear ();
  }

/// ������������� ����� ���������
  void InitRenderTargets (WindowManager& window_manager, const common::PropertyMap& properties)
  {
    try
    {
        //����� ����

      manager::Window& window = window_manager.GetWindow (name.c_str ());

        //����������� ������������ ������� ����

      on_window_resize_connection = window.RegisterEventHandler (manager::WindowEvent_OnResize, xtl::bind (&Impl::OnWindowResize, this));
      on_window_update_connection = window.RegisterEventHandler (manager::WindowEvent_OnUpdate, xtl::bind (&Impl::OnWindowUpdate, this));

        //���������� ��������� ���� ���������

      const char *color_binding         = properties.IsPresent (COLOR_BINDING_PROPERTY_NAME) ? properties.GetString (COLOR_BINDING_PROPERTY_NAME) : DEFAULT_COLOR_BINDING_PROPERTY_VALUE,
                 *depth_stencil_binding = properties.IsPresent (DEPTH_STENCIL_BINDING_PROPERTY_NAME) ? properties.GetString (DEPTH_STENCIL_BINDING_PROPERTY_NAME) : DEFAULT_DEPTH_STENCIL_BINDING_PROPERTY_VALUE;

      render_targets.Add (color_binding, window.ColorBuffer (), area);
      render_targets.Add (depth_stencil_binding, window.DepthStencilBuffer (), area);

//TODO: init clearing for all targets

      if (clear_frame)
      {
        clear_frame->SetRenderTarget (color_binding, window.ColorBuffer ());
        clear_frame->SetRenderTarget (depth_stencil_binding, window.DepthStencilBuffer ());
      }

      this->window = &window;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Screen::Impl::InitRenderTargets");
      throw;
    }
  }

/// ������������� ���������� �������
  void InitClearing (const common::PropertyMap& properties)
  {
    try
    {
      const char* clear_effect = properties.IsPresent (CLEARING_EFFECT_BINDING_PROPERTY_NAME) ? properties.GetString (CLEARING_EFFECT_BINDING_PROPERTY_NAME) : DEFAULT_CLEARING_EFFECT_BINDING_PROPERTY_VALUE;

      if (!*clear_effect)
        return;

      clear_frame.reset (new manager::Frame (render_manager.Manager ().CreateFrame ()));

      clear_frame->SetEffect (clear_effect);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Screen::Impl::InitClearing");
      throw;
    }
  }

/// ���������� �� ��������� �������� ����
  void OnWindowResize ()
  {
    try
    {
      render_targets.ForceUpdateNotify ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Screen::Impl::OnWindowResize");
      throw;
    }
  }

/// ���������� �� ���������� ����
  void OnWindowUpdate ()
  {
    try
    {
      Update (0);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Screen::Impl::OnWindowUpdate");
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

/// ����������
  void Update (manager::Frame* parent_frame)
  {
    try
    {
      if (!active)
        return;

        //���������� �������� ������

      if (need_reorder)
        Reorder ();

        //�������

      if (clear_frame && background_state)
      {
        if (parent_frame) parent_frame->AddFrame (*clear_frame);
        else              clear_frame->Draw ();
      }

        //����� �������� ������

      for (ViewportDescList::iterator iter=viewports.begin (), end=viewports.end (); iter!=end; ++iter)
      {
        Viewport& viewport = (*iter)->viewport;

        try
        {
          viewport.Update (parent_frame);
        }
        catch (std::exception& e)
        {
          render_manager.Log ().Printf ("%s\n    at render::scene::server::Screen::Update(screen='%s')", e.what (), name.c_str ());
        }
        catch (...)
        {
          render_manager.Log ().Printf ("unknown exception\n    at render::scene::server::Screen::Update(screen='%s')", name.c_str ());
        }      
      }

        //������� ������

      if (!parent_frame)
        drawing_viewports.CleanupViewports ();

       //����� �� �����

      if (window)
        window->SwapBuffers ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Screen::Update");
      throw;
    }
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
  try
  {
    impl->background_state = state;
    impl->background_color = color;

    if (!impl->clear_frame)
      return;

    if (state)
    {
      impl->clear_frame->SetClearColor (color);
      impl->clear_frame->SetClearFlags (manager::ClearFlag_All | manager::ClearFlag_ViewportOnly);
    }
    else
    {
      impl->clear_frame->SetClearFlags (0u);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Screen::SetBackground");
    throw;
  }
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
  impl->Update (parent_frame);
}
