#include "shared.h"

using namespace render;
using namespace render::scene::server;

/*
    ���������
*/

const size_t LISTENER_ARRAY_RESERVE_SIZE = 4;  //������������� ���������� ����������
const size_t TECNIQUE_ARRAY_RESERVE_SIZE = 16; //������������� ���������� ������

/*
    �������� ���������� ������� ������
*/

namespace
{

struct RenderTargetEntry
{
  RenderTargetDesc desc;
  bool             need_update;

  RenderTargetEntry (const RenderTargetDesc& in_desc) : desc (in_desc), need_update (true) {}
};

typedef stl::hash_map<const RenderTargetDesc*, RenderTargetEntry> RenderTargetEntryMap;
typedef stl::vector<IViewportListener*>                           ListenerArray;
typedef stl::vector<TechniquePtr>                                 TechniqueArray;

}

struct Viewport::Impl: public xtl::reference_counter, public IRenderTargetMapListener, public IRenderManagerListener
{
  RenderManager        render_manager;             //�������� ����������
  RenderTargetMap      render_targets;             //���� ����������
  RenderTargetEntryMap render_target_entries;      //����������� ����� ����������
  stl::string          name;                       //��� ������� ������
  stl::string          technique_name;             //��� �������
  TechniqueArray       sub_techniques;             //�������
  Rect                 area;                       //������� ������
  float                min_depth;                  //����������� �������
  float                max_depth;                  //������������ �������
  int                  zorder;                     //������� ������
  bool                 is_active;                  //������� �� ������� ���������
  manager::Frame       frame;                      //�������������� ����
  math::vec4f          background_color;           //���� ����
  bool                 background_state;           //��������� ����
  common::PropertyMap  properties;                 //�������� ������� ������
  server::Camera       camera;                     //������
  ListenerArray        listeners;                  //��������� ������� ������� ������
  bool                 need_reconfiguration;       //������������ ��������
  bool                 need_update_technique;      //���������� �������
  bool                 need_update_area;           //���������� �������
  bool                 need_update_render_targets; //��������� �������� ������ ���������
  bool                 need_update_background;     //��������� �������� ��������� �������
  bool                 need_update_camera;         //��������� �������� ������
  bool                 need_update_properties;     //��������� ���������� �������

/// �����������
  Impl (const RenderManager& in_render_manager, const RenderTargetMap& in_render_targets)
    : render_manager (in_render_manager)
    , render_targets (in_render_targets)
    , area (0, 0, 100, 100)
    , min_depth (0.0f)
    , max_depth (1.0f)
    , zorder (0)
    , is_active (true)
    , frame (render_manager.Manager ().CreateFrame ())
    , background_state (false)
    , need_reconfiguration (true)
    , need_update_area (true)
    , need_update_render_targets (true)
    , need_update_background (true)
    , need_update_camera (true)
    , need_update_properties (true)
  {
    listeners.reserve (LISTENER_ARRAY_RESERVE_SIZE);
    sub_techniques.reserve (TECNIQUE_ARRAY_RESERVE_SIZE);

    render_targets.AttachListener (this);

    try
    {
      render_manager.AttachListener (this);
    }
    catch (...)
    {
      render_targets.DetachListener (this);
      throw;
    }
  }

/// ����������
  ~Impl ()
  {
    sub_techniques.clear ();

    render_manager.DetachListener (this);
    render_targets.DetachListener (this);
  }

/// ���������� � ���������� ����� ������� ������
  void OnRenderTargetAdded (const RenderTargetDesc& desc)
  {
    RenderTargetEntryMap::iterator iter = render_target_entries.find (&desc);

    if (iter != render_target_entries.end ())
      return;

    render_target_entries.insert_pair (&desc, RenderTargetEntry (desc));

    need_update_render_targets = true;
    need_reconfiguration       = true;
  }

/// ���������� �� ��������� ������� ������
  void OnRenderTargetUpdated (const RenderTargetDesc& desc)
  {
    RenderTargetEntryMap::iterator iter = render_target_entries.find (&desc);

    if (iter == render_target_entries.end ())
      return;

    iter->second.need_update   = true;    
    need_update_render_targets = true;
    need_reconfiguration       = true;
  }

/// ���������� �� �������� ������� ������
  void OnRenderTargetRemoved (const RenderTargetDesc& desc)
  {
    render_target_entries.erase (&desc);

    need_update_render_targets = true;
    need_reconfiguration       = true;    
  }

/// ���������� �� ��������� ������������
  void OnRenderManagerConfigurationChanged (const common::ParseNode&)
  {
    sub_techniques.clear ();

    need_update_technique = true;
    need_reconfiguration  = true;
  }

/// �������� ������
  void CreateSubTechniques ()
  {
    try
    {
      common::Log&      log           = render_manager.Log ();
      common::ParseNode configuration = render_manager.Manager ().Configuration ();

      for (common::Parser::NamesakeIterator iter=configuration.First ("technique"); iter; ++iter)
      {
        const char* technique = common::get<const char*> (*iter, "", "");        
        
        if (xtl::xstrcmp (technique, technique_name.c_str ()))
          continue;

        common::ParseNode parent_technique_node = *iter;

        for (common::Parser::Iterator iter=parent_technique_node.First (); iter; ++iter)
        {
          try
          {
            TechniquePtr technique = TechniqueManager::CreateTechnique (iter->Name (), render_manager, *iter);
            
            if (!technique)
              throw xtl::format_operation_exception ("", "Can't create technique '%s'", iter->Name ());
            
            technique->SetName (iter->Name ());

            sub_techniques.push_back (technique);
          }
          catch (std::exception& e)
          {
            log.Printf ("%s\n    at create sub-technique '%s' for technique '%s' at viewport '%s'\n    at render::scene::server::Viewport::Impl::CreateSubTechniques",
              e.what (), iter->Name (), technique_name.c_str (), name.c_str ());
          }
          catch (...)
          {
            log.Printf ("unknown exception\n    at create sub-technique '%s' for technique '%s' at viewport '%s'\n    at render::scene::server::Viewport::Impl::CreateSubTechniques",
              iter->Name (), technique_name.c_str (), name.c_str ());
          }
        }

        return;
      }

      throw xtl::format_operation_exception ("", "Technique '%s' has not been found in the render manager configuration file '%s'", technique_name.c_str (), common::get<const char*> (configuration, "", ""));
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Viewport::Impl::CreateSubTechniques(technique='%s')", technique_name.c_str ());
      throw;
    }
  }

/// ���������������� �������
  void ReconfigureTechnique ()
  {
    try
    {
      render_manager.Log ().Printf ("Initializing technique '%s' for viewport '%s'", technique_name.c_str (), name.c_str ());

      CreateSubTechniques ();

      need_update_properties = true;
      //TODO: need_update_camera = true
      need_update_technique  = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Viewport::Impl::ReconfigureTechnique");
      throw;
    }
  }

/// ���������������� ���������� �������
  void ReconfigureBackground ()
  {
    try
    {
      if (background_state)
      {
        frame.SetClearColor (background_color);
        frame.SetClearFlags (manager::ClearFlag_All | manager::ClearFlag_ViewportOnly);
      }
      else
      {
        frame.SetClearFlags (0u);
      }

      need_update_background = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Viewport::Impl::ReconfigureBackground");
      throw;
    }
  }

/// ���������������� ������� ������� ���������
  void ReconfigureRenderTargets ()
  {
    try
    {
      const Rect& viewport_area = area;
                              
      for (RenderTargetEntryMap::iterator iter=render_target_entries.begin (), end=render_target_entries.end (); iter!=end; ++iter)
      {
        RenderTargetEntry& entry = iter->second;

        if (!need_update_area && !entry.need_update)
          continue;

        const Rect& screen_area = entry.desc.Area ();

        const manager::RenderTarget& target = entry.desc.Target ();

        const size_t target_width = target.Width (), target_height = target.Height ();

        const double x_scale = screen_area.width ? double (target_width) / screen_area.width : 0.0,
                     y_scale = screen_area.height ? double (target_height) / screen_area.height : 0.0;

        Rect target_rect (int ((viewport_area.x - screen_area.x) * x_scale),
                          int ((viewport_area.y - screen_area.y) * y_scale),
                          size_t (ceil (viewport_area.width * x_scale)),
                          size_t (ceil (viewport_area.height * y_scale)));                               

        frame.SetRenderTarget (entry.desc.Name (), target, manager::Viewport (target_rect, min_depth, max_depth));

        entry.need_update = false;
      }
      
      need_update_render_targets = false;
      need_update_area           = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Viewport::Impl::ReconfigureRenderTargets");
      throw;
    }
  }

/// ���������������� �������
  void ReconfigureProperties ()
  {
    try
    {
      for (TechniqueArray::iterator iter=sub_techniques.begin (), end=sub_techniques.end (); iter!=end;)
      {
        try
        {
          (*iter)->SetProperties (properties);
          
           ++iter;
           
           continue;
        }
        catch (std::exception& e)
        {
          render_manager.Log ().Printf ("%s\n    at update sub-technique '%s' properties for technique '%s' at viewport '%s'\n    at render::scene::server::Viewport::Impl::ReconfigureProperties", e.what (), (*iter)->Name (), technique_name.c_str (), name.c_str ());
        }
        catch (...)
        {
          render_manager.Log ().Printf ("unknown exception\n    at update sub-technique '%s' properties for technique '%s' at viewport '%s'\n    at render::scene::server::Viewport::Impl::ReconfigureProperties", (*iter)->Name (), technique_name.c_str (), name.c_str ());
        }
        
        sub_techniques.erase (iter);
        
        end = sub_techniques.end ();
      }            

      need_update_properties = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Viewport::Impl::ReconfigureProperties");
      throw;
    }
  }

/// ����������������
  void Reconfigure ()
  {
    try
    {
      if (!need_reconfiguration)
        return;

        //���������������� �������

      if (need_update_technique)
        ReconfigureTechnique ();

        //���������������� ���������� �������
        
      if (need_update_background)
        ReconfigureBackground ();

        //���������������� ������� ������� ���������
        
      if (need_update_render_targets || need_update_area)
        ReconfigureRenderTargets ();

        //���������������� �������

      if (need_update_properties)
        ReconfigureProperties ();

        //���������� ������

      need_reconfiguration = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Viewport::Impl::Reconfigure");
      throw;
    }
  }
};

/*
    ������������ / ���������� / ������������
*/

Viewport::Viewport (const RenderManager& render_manager, const RenderTargetMap& render_target_map)
{
  try
  {
    impl = new Impl (render_manager, render_target_map);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Viewport::Viewport");
    throw;
  }
}

Viewport::Viewport (const Viewport& vp)
  : impl (vp.impl)
{
  addref (impl);
}

Viewport::~Viewport ()
{
  release (impl);
}

Viewport& Viewport::operator = (const Viewport& vp)
{
  Viewport tmp (vp);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    ��� ������� ������
*/

void Viewport::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene::server::Viewport::SetName", "name");

  impl->name = name;
}

const char* Viewport::Name () const
{
  return impl->name.c_str ();
}

/*
    �������
*/

void Viewport::SetArea (const Rect& rect)
{
  if (rect == impl->area)
    return;

  impl->area                 = rect;
  impl->need_update_area     = true;
  impl->need_reconfiguration = true;
}

const Rect& Viewport::Area () const
{
  return impl->area;
}

/*
    �������� ������� ��� ������� ������
*/

void Viewport::SetMinDepth (float value)
{
  if (value == impl->min_depth)
    return;

  impl->min_depth            = value;
  impl->need_update_area     = true;
  impl->need_reconfiguration = true;
}

void Viewport::SetMaxDepth (float value)
{
  if (value == impl->max_depth)
    return;

  impl->max_depth            = value;
  impl->need_update_area     = true;
  impl->need_reconfiguration = true;
}

float Viewport::MinDepth () const
{
  return impl->min_depth;
}

float Viewport::MaxDepth () const
{
  return impl->max_depth;
}

/*
    ���������� ������� ������
*/

void Viewport::SetActive (bool active)
{
  impl->is_active = active;
}

bool Viewport::IsActive () const
{
  return impl->is_active;
}

/*
    ������� ������
*/

void Viewport::SetZOrder (int order)
{
  if (order == impl->zorder)
    return;

  impl->zorder = order;

  for (ListenerArray::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
  {
    try
    {
      (*iter)->OnViewportZOrderUpdated (order);
    }
    catch (...)
    {
    }
  }
}

int Viewport::ZOrder () const
{
  return impl->zorder;
}

/*
    ������ ���
*/

void Viewport::SetBackground (bool state, const math::vec4f& color)
{
  if (state == impl->background_state && color == impl->background_color)
    return;

  impl->background_state       = state;
  impl->background_color       = color;
  impl->need_update_background = true;
  impl->need_reconfiguration   = true;
}

bool Viewport::BackgroundState () const
{
  return impl->background_state;
}

const math::vec4f& Viewport::BackgroundColor () const
{
  return impl->background_color;
}

/*
    ������� ���������
*/

void Viewport::SetTechnique (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene::server::Viewport::SetTechnique", "name");

  if (name == impl->technique_name)
    return;

  impl->sub_techniques.clear ();

  impl->technique_name        = name;
  impl->need_update_technique = true;
  impl->need_reconfiguration  = true;
}

const char* Viewport::Technique () const
{
  return impl->technique_name.c_str ();
}

/*
    �������� ���������
*/

void Viewport::SetProperties (const common::PropertyMap& properties)
{
  impl->properties             = properties;
  impl->need_update_properties = true;
  impl->need_reconfiguration   = true;
}

const common::PropertyMap& Viewport::Properties () const
{
  return impl->properties;
}

/*
    ������
*/

Camera* Viewport::Camera ()
{
  return &impl->camera;
}

const Camera* Viewport::Camera () const
{
  return &impl->camera;
}

/*
    �������������� ����
*/

manager::Frame& Viewport::Frame () const
{
  return impl->frame;
}

/*
    ����������
*/

void Viewport::Update (manager::Frame* parent_frame)
{
  try
  {
      //��������� ������ � ������ ���������� ������� ������
    
    if (!impl->is_active)
      return;

      //���������������� � ������ ��������� ������������ ����������

    if (impl->need_reconfiguration)
      impl->Reconfigure ();

      //���������� ���� ����������� ������ �����
     
    Context context (impl->frame); 

      //���������� �����

    for (TechniqueArray::iterator iter=impl->sub_techniques.begin (), end=impl->sub_techniques.end (); iter!=end;)
    {
      try
      {
        (*iter)->UpdateFrame (context);
        
        ++iter;
        
        continue;
      }
      catch (std::exception& e)
      {
        impl->render_manager.Log ().Printf ("%s\n    at update sub-technique '%s' frame for technique '%s' at viewport '%s'\n    at render::scene::server::Viewport::Update", e.what (), (*iter)->Name (), impl->technique_name.c_str (), impl->name.c_str ());
      }
      catch (...)
      {
        impl->render_manager.Log ().Printf ("unknown exception\n    at update sub-technique '%s' frame for technique '%s' at viewport '%s'\n    at render::scene::server::Viewport::Update", (*iter)->Name (), impl->technique_name.c_str (), impl->name.c_str ());
      }

      impl->sub_techniques.erase (iter);

      end = impl->sub_techniques.end ();
    }

      //���������
      
    manager::Frame& frame = impl->frame;
    
    if (frame.EntitiesCount () || frame.FramesCount ())
    {
      if (parent_frame) parent_frame->AddFrame (frame);
      else              frame.Draw ();    
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Viewport::Update(viewport='%s')", impl->name.c_str ());
    throw;
  }
}

/*
    ���������� ����������
*/

void Viewport::AttachListener (IViewportListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::scene::server::Viewport::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void Viewport::DetachListener (IViewportListener* listener)
{
  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void Viewport::DetachAllListeners ()
{
  impl->listeners.clear ();
}
