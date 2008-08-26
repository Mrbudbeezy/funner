#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    �������� ���������� ���������� ������
*/

typedef xtl::signal<void (SpriteModel& sender, SpriteModelEvent event_id)> SpriteModelSignal;

struct SpriteModel::Impl
{
  stl::string       material;                       //��� ���������
  SpriteModelSignal signals [SpriteModelEvent_Num]; //������� ������ 
  vec3f             pivot_position;                 //��������� ������ � ��������� ������� ���������
  float             pivot_rotation;                 //������� ������ � ��������� ������� ���������
  float             alpha_reference;                //��������, ������������ ��� �����-�����
  bool              need_recalc_world_tm;           //��������� �������� ������� ������� ��������������
  mat4f             world_tm;                       //������� ������� ��������������

  Impl () : need_recalc_world_tm (true), pivot_rotation (0.0f), alpha_reference (0.f) {}

    //���������� � �������
  void Notify (SpriteModel& sender, SpriteModelEvent event_id)
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

SpriteModel::SpriteModel ()
  : impl (new Impl)
{
}

SpriteModel::~SpriteModel ()
{
}

/*
    ��������
*/

void SpriteModel::SetMaterial (const char* in_material)
{
  if (!in_material)
    throw xtl::make_null_argument_exception ("scene_graph::SpriteModel::SetMaterial", "material");

  impl->material = in_material;  

  UpdateNotify ();
}

const char* SpriteModel::Material () const
{
  return impl->material.c_str ();
}

/*
    ��������, ������������ ��� �����-�����
*/

void SpriteModel::SetAlphaReference (float value)
{
  impl->alpha_reference = value;
  
  UpdateNotify ();
}

float SpriteModel::AlphaReference () const
{
  return impl->alpha_reference;
}

/*
    ��������� � ����������� ������ ������
*/

void SpriteModel::SetPivotPosition (const vec3f& position)
{
  SetPivot (position, impl->pivot_rotation);
}

void SpriteModel::SetPivotPosition (float x, float y, float z)
{
  SetPivotPosition (vec3f (x, y, z));
}

//������� ������ ������������ Oz
void SpriteModel::SetPivotRotation (float angle_in_degrees)
{
  SetPivot (impl->pivot_position, angle_in_degrees);
}

void SpriteModel::SetPivot (const vec3f& position, float angle_in_degrees)
{
  impl->pivot_position       = position;
  impl->pivot_rotation       = angle_in_degrees;
  impl->need_recalc_world_tm = true;    

    //���������� �� ����������

  UpdateNotify ();
}

const vec3f& SpriteModel::PivotPosition () const
{
  return impl->pivot_position;
}

float SpriteModel::PivotRotation () const
{
  return impl->pivot_rotation;
}

/*
    ��������� ������ �������������� ����� ���������� pivot-��������������
*/

namespace
{

//������ ������� ������
void eval_pivot_tm (const math::vec3f& position, float angle, math::mat4f& pivot_tm)
{
  pivot_tm = translate (-position) * fromAxisAngle (deg2rad (angle), vec3f (0, 0, 1));
}

}

void SpriteModel::EvalTransformationAfterPivot (NodeTransformSpace space, math::mat4f& result) const
{
  switch (space)
  {
    case NodeTransformSpace_Local:
    {
      eval_pivot_tm (impl->pivot_position, impl->pivot_rotation, result);
      break;
    }
    case NodeTransformSpace_Parent:
    {
      math::mat4f pivot_tm;

      eval_pivot_tm (impl->pivot_position, impl->pivot_rotation, pivot_tm);

      result = LocalTM () * pivot_tm;

      break;
    }
    case NodeTransformSpace_World:
      if (impl->need_recalc_world_tm)
      {        
        math::mat4f pivot_tm;

        eval_pivot_tm (impl->pivot_position, impl->pivot_rotation, pivot_tm);

        impl->world_tm = WorldTM () * pivot_tm;

        impl->need_recalc_world_tm = false;
      }

      result = impl->world_tm;

      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::SpriteModel::EvalTransformationAfterPivot", "space", space);
  }
}

/*
    ���������� �������� / ��������� ������� ��������
*/

size_t SpriteModel::SpriteDescsCount () const
{
  return const_cast<SpriteModel&> (*this).SpriteDescsCountCore ();
}

const SpriteModel::SpriteDesc* SpriteModel::SpriteDescs () const
{
  return const_cast<SpriteModel&> (*this).SpriteDescsCore ();
}

/*
    ������������ ���������������
*/

void SpriteModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}

/*
    �������� �� ������� ������
*/

xtl::connection SpriteModel::RegisterEventHandler (SpriteModelEvent event, const EventHandler& handler)
{
  switch (event)
  {
    case SpriteModelEvent_AfterSpriteDescsUpdate:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::SpriteModel::RegisterEventHandler", "event", event);
  }
  
  return impl->signals [event].connect (handler);
}

/*
    ���������� �� ��������� ������� ������� ��������������
*/

void SpriteModel::AfterUpdateWorldTransformEvent ()
{
  Entity::AfterUpdateWorldTransformEvent ();

  impl->need_recalc_world_tm = true;
}

/*
    ���������� �� ��������� ������ ��������
*/

void SpriteModel::UpdateSpriteDescsNotify ()
{
  impl->Notify (*this, SpriteModelEvent_AfterSpriteDescsUpdate);

  UpdateNotify ();
}
