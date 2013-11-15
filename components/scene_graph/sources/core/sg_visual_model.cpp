#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    �������� ���������� ���������� ������
*/

typedef xtl::signal<void (VisualModel& sender, VisualModelEvent event_id)> VisualModelSignal;

struct VisualModel::Impl: public xtl::instance_counter<VisualModel>
{
  scene_graph::Scissor* scissor;                        //������� ���������
  xtl::auto_connection  on_scissor_destroy_connection;  //���������� � �������� �������� ������� ���������
  VisualModelSignal     signals [VisualModelEvent_Num]; //������� ������ 
  bool                  need_release_scissor;           //����� �� ����������� ������� ���������

  Impl () : scissor (), need_release_scissor () {}

  ~Impl ()
  {
    if (need_release_scissor)    
      scissor->Release ();
  }

  const char* ScissorName ()
  {
    if (!scissor)
      return "";

    return scissor->Name ();
  }

///���������� � �������
  void Notify (VisualModel& sender, VisualModelEvent event_id)
  {
    try
    {
      if (!signals [event_id])
        return;

      signals [event_id] (sender, event_id);
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }

/// ������������ ������� ������
  void UnbindScissor ()
  {
    scissor              = 0;
    need_release_scissor = false;

    on_scissor_destroy_connection.disconnect ();
  }
};

/*
    ����������� / ����������
*/

VisualModel::VisualModel ()
  : impl (new Impl)
{
}

VisualModel::~VisualModel ()
{
}

/*
    ������� ���������
*/

void VisualModel::SetScissor (scene_graph::Scissor* scissor, NodeBindMode mode)
{
    //��������� ������������ ������ �������������

  bool need_release_scissor = false;
    
  switch (mode)
  {
    case NodeBindMode_AddRef:
      need_release_scissor = true;
      break;
    case NodeBindMode_WeakRef:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::VisualModel::SetScissor", "mode", mode);
  }

  if (scissor == impl->scissor)
    return;

  scene_graph::Scissor* old_scissor = impl->scissor;

    //��������� ������� ���������  

  xtl::auto_connection on_destroy;

  if (scissor)
  {
    on_destroy = scissor->RegisterEventHandler (NodeEvent_AfterDestroy, xtl::bind (&Impl::UnbindScissor, &*impl));
  }

  if (impl->need_release_scissor)
  {
    old_scissor->Release ();

    impl->need_release_scissor = false;
  }

  impl->scissor                       = scissor;
  impl->on_scissor_destroy_connection = on_destroy;

  xtl::connection ().swap (on_destroy);

  if (scissor)
  {  
    impl->need_release_scissor = need_release_scissor;
   
    if (need_release_scissor)
      scissor->AddRef ();
  }

  impl->Notify (*this, VisualModelEvent_AfterScissorUpdate);
}

Scissor* VisualModel::Scissor ()
{
  return impl->scissor;
}

const Scissor* VisualModel::Scissor () const
{
  return impl->scissor;
}

/*
    ������������ ���������������
*/

void VisualModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}

/*
    �������� �� ������� ������
*/

xtl::connection VisualModel::RegisterEventHandler (VisualModelEvent event, const EventHandler& handler)
{
  switch (event)
  {
    case VisualModelEvent_AfterScissorUpdate:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::VisualModel::RegisterEventHandler", "event", event);
  }
  
  return impl->signals [event].connect (handler);
}

/*
    ���������� �������
*/

void VisualModel::BindProperties (common::PropertyBindingMap& bindings)
{
  Entity::BindProperties (bindings);

  bindings.AddProperty ("Scissor", xtl::bind (&VisualModel::Impl::ScissorName, &*impl));
}
