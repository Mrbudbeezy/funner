#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    �������� ���������� ������ �����
*/

typedef xtl::signal<void (LineModel& sender, LineModelEvent event_id)> LineModelSignal;

struct LineModel::Impl: public xtl::instance_counter<LineModel>
{
  stl::string     material;                     //��� ���������
  size_t          material_hash;                //��� ����� ���������
  LineUsage       usage;                        //����� ������������� �����
  LineModelSignal signals [LineModelEvent_Num]; //������� ������ 

  Impl ()
    : material_hash (0xffffffff)
    , usage (LineUsage_Default)
  {
  }

    //���������� � �������
  void Notify (LineModel& sender, LineModelEvent event_id)
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
};

/*
    ����������� / ����������
*/

LineModel::LineModel ()
  : impl (new Impl)
{
}

LineModel::~LineModel ()
{
}

/*
    ��������
*/

void LineModel::SetMaterial (const char* in_material)
{
  if (!in_material)
    throw xtl::make_null_argument_exception ("scene_graph::LineModel::SetMaterial", "material");

  size_t hash = common::strhash (in_material);

  if (hash == impl->material_hash)
    return;

  impl->material      = in_material;  
  impl->material_hash = hash;

  UpdateNotify ();
}

const char* LineModel::Material () const
{
  return impl->material.c_str ();
}

size_t LineModel::MaterialHash () const
{
  return impl->material_hash;
}

/*
    ����� ������������� �����
*/

void LineModel::SetUsage (LineUsage usage)
{
  switch (usage)
  {
    case LineUsage_Static:
    case LineUsage_Dynamic:
    case LineUsage_Stream:
    case LineUsage_Batching:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::LineModel::SetUsage", "usage", usage);
  }

  if (impl->usage == usage)
    return;

  impl->usage = usage;

  impl->Notify (*this, LineModelEvent_AfterModeUsageUpUpdate);

  UpdateNotify ();
}

LineUsage LineModel::Usage () const
{
  return impl->usage;
}

/*
    ���������� ����� / ������ ������ ����� / ��������� ������� �����
*/

size_t LineModel::LineDescsCount () const
{
  return const_cast<LineModel&> (*this).LineDescsCountCore ();
}

size_t LineModel::LineDescsCapacity () const
{
  return const_cast<LineModel&> (*this).LineDescsCapacityCore ();
}

const LineDesc* LineModel::LineDescs () const
{
  return const_cast<LineModel&> (*this).LineDescsCore ();
}

/*
    ������������ ���������������
*/

void LineModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    VisualModel::AcceptCore (visitor);
}

/*
    �������� �� ������� ������
*/

xtl::connection LineModel::RegisterEventHandler (LineModelEvent event, const EventHandler& handler)
{
  switch (event)
  {
    case LineModelEvent_AfterLineDescsUpdate:
    case LineModelEvent_AfterModeUsageUpUpdate:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::LineModel::RegisterEventHandler", "event", event);
  }
  
  return impl->signals [event].connect (handler);
}

/*
    ���������� �� ��������� ������ �����
*/

void LineModel::UpdateLineDescsNotify ()
{
  impl->Notify (*this, LineModelEvent_AfterLineDescsUpdate);

  UpdateNotify ();
}

/*
    ���������� �������
*/

void LineModel::BindProperties (common::PropertyBindingMap& bindings)
{
  VisualModel::BindProperties (bindings);

  bindings.AddProperty ("Material", xtl::bind (&LineModel::Material, this), xtl::bind (&LineModel::SetMaterial, this, _1));
}
