#include <sg/entity.h>
#include <stl/string>
#include <xtl/signal.h>
#include <xtl/visitor.h>
#include <common/exception.h>

using namespace sg;
using namespace math;
using namespace common;

/*
    �������� ���������� Entity
*/

struct Entity::Impl
{
  sg::Scene*  scene;                            //�����, ������� ����������� ������
  stl::string name;                             //��� �������
  size_t      name_hash;                        //��� �����
  size_t      ref_count;                        //���������� ������ �� ������
  Entity*     parent;                           //������������ ����
  Entity*     first_child;                      //������ �������
  Entity*     last_child;                       //��������� �������
  Entity*     prev_child;                       //���������� �������
  Entity*     next_child;                       //��������� �������
  vec3f       color;                            //���� �������
  Signal      signals [EntityEvent_Num];        //�������
  bool        signal_process [EntityEvent_Num]; //����� ��������� ��������
  vec3f       local_position;                   //��������� ��������� � ������������
  quatf       local_orientation;                //��������� ���������� �������
  vec3f       local_scale;                      //��������� ������� �������
  mat4f       local_tm;                         //������� ��������� ��������������
  vec3f       world_position;                   //������� ��������� � ������������
  quatf       world_orientation;                //������� ���������� �������
  vec3f       world_scale;                      //������� ������� �������
  mat4f       world_tm;                         //������� ������� ��������������
  bool        orientation_inherit;              //���� ������������ ������������ ����������
  bool        scale_inherit;                    //���� ������������ ������������� ��������
  bool        need_world_transform_update;      //����, ��������������� � ������������� ��������� ������� ��������������
  bool        need_world_tm_update;             //����, ��������������� � ������������� ��������� ������� ������� ��������������
  bool        need_local_tm_update;             //����, ��������������� � ������������� ��������� ������� ��������� ��������������
  size_t      update_lock;                      //������� �������� ���������� ����������
  bool        update_notify;                    //����, ��������������� � ������������� ���������� �� ����������� �� ���������� ���������� ����������
};

/*
    ����������� / ����������
*/

Entity::Entity ()
  : impl (new Impl)
{
  impl->scene         = 0;    //�� ��������� ������ �� ����������� �� ����� �����
  impl->ref_count     = 1;
  impl->parent        = 0;
  impl->first_child   = 0;
  impl->last_child    = 0;
  impl->prev_child    = 0;
  impl->next_child    = 0;
  impl->update_lock   = 0;
  impl->update_notify = false;
  impl->name_hash     = strhash (impl->name.c_str ());

    //������� �� ���������
    
  impl->local_scale = 1.0f;
  impl->world_scale = 1.0f;

    //�� ��������� ������ ��������� ��� �������������� ��������
  
  impl->orientation_inherit = true;
  impl->scale_inherit       = true;
  
    //�������������� ����������

  impl->need_world_transform_update = false;
  impl->need_local_tm_update        = false;
  impl->need_world_tm_update        = false;  
}

Entity::~Entity ()
{
    //��������� ������������� �����

  UnbindAllChildren ();
  Unbind ();
  
    //���������� �� �������� �������
    
  Notify (EntityEvent_AfterDestroy);

    //������� ���������� �������

  delete impl;
}

/*
    �������� �������
*/

Entity* Entity::Create ()
{
  return new Entity;
}

/*
    ����������� �����, ������� ����������� ������
*/

Scene* Entity::Scene ()
{
  return impl->scene;
}

const Scene* Entity::Scene () const
{
  return impl->scene;
}

/*
    ��� �������
*/

const char* Entity::Name () const
{
  return impl->name.c_str ();
}

void Entity::SetName (const char* name)
{
  if (!name)
    RaiseNullArgument ("sg::Entity::SetName", "name");

  impl->name      = name;
  impl->name_hash = strhash (name);
  
  UpdateNotify ();
}

/*
    ������� ������
*/

void Entity::AddRef ()
{
  impl->ref_count++;
}

void Entity::Release ()
{
    //������ �� ���������� �������� � �����������

  if (impl->signal_process [EntityEvent_BeforeDestroy])
    return;            

  if (!--impl->ref_count)
  {
      //��������� �������� �� �������� �������

    Notify (EntityEvent_BeforeDestroy);

    delete this;
  }
}

size_t Entity::UseCount () const
{
  return impl->ref_count;
}

/*
    ���� �������
*/

void Entity::SetColor (const vec3f& color)
{
  impl->color = color;
  
  UpdateNotify ();
}

void Entity::SetColor (float red, float green, float blue)
{
  SetColor (vec3f (red, green, blue));
}

const vec3f& Entity::Color () const
{
  return impl->color;
}

/*
    ����� ��� ����������� �� ������� ������ ��������
*/

Entity* Entity::Parent ()
{
  return impl->parent;
}

const Entity* Entity::Parent () const
{
  return impl->parent;
}

Entity* Entity::FirstChild ()
{
  return impl->first_child;
}

const Entity* Entity::FirstChild () const
{
  return impl->first_child;
}

Entity* Entity::LastChild ()
{
  return impl->last_child;
}

const Entity* Entity::LastChild () const
{
  return impl->last_child;
}

Entity* Entity::PrevChild ()
{
  return impl->prev_child;
}

const Entity* Entity::PrevChild () const
{
  return impl->prev_child;
}

Entity* Entity::NextChild ()
{
  return impl->next_child;
}

const Entity* Entity::NextChild () const
{
  return impl->next_child;
}

/*
    ������������� ���� � ��������
*/

void Entity::Unbind (EntityTransformSpace invariant_space)
{
  BindToParentImpl (0, EntityBindMode_Capture, invariant_space);
}

void Entity::BindToParent (Entity& parent, EntityBindMode mode, EntityTransformSpace invariant_space)
{
  BindToParentImpl (&parent, mode, invariant_space);
}

void Entity::BindToParentImpl (Entity* parent, EntityBindMode mode, EntityTransformSpace invariant_space)
{
    //������ �� ������ Bind � ������������ ��������������� �������

  if (impl->signal_process [EntityEvent_AfterBind] || impl->signal_process [EntityEvent_BeforeUnbind])
    return;

    //���������� ������������ ������������ ��������������

  switch (invariant_space)
  {
    case EntityTransformSpace_Parent:
    case EntityTransformSpace_Local:
      break;
    case EntityTransformSpace_World:
      RaiseNotImplemented ("sg::Entity::BindToParent (invariant_space=EntityTransformSpace_World)");
      break;
    default:
      RaiseInvalidArgument ("sg::Entity::BindToParent", "invariant_space", invariant_space);
      break;
  }
  
    //��������� ������������ ������ �������������
    
  switch (mode)
  {
    case EntityBindMode_AddRef:
    case EntityBindMode_Capture:
      break;
    default:
      RaiseInvalidArgument ("sg::Entity::BindToParent", "mode", mode);
      break;
  }
  
    //���� �������� �� ���������� ��� ������������� � �����������

  if (parent == impl->parent)
    return;      

    //�������� �� �������������� �� ������ � ������ �������
    
  for (Entity* entity=parent; entity; entity=entity->impl->parent)
    if (entity == this)
      RaiseInvalidArgument ("sg::Entity::BindToParent", "parent", "Attempt to bind object to one of it's child");
      
    //��������� �������� �� ������������ ������� �� ��������
      
  Notify (EntityEvent_BeforeUnbind);
  
    //���� � ������ ��� ���� �������� ����������� �������� �� ��� ������������
    
  if (impl->parent)
  {
      //���������� ������ �� ��������    
    
    if (impl->prev_child) impl->prev_child->impl->next_child = impl->next_child;
    else                  impl->parent->impl->first_child    = impl->next_child;
    
    if (impl->next_child) impl->next_child->impl->prev_child = impl->prev_child;
    else                  impl->parent->impl->last_child     = impl->prev_child;    
    
      //����������� ��������

    impl->parent->Release ();
  }

    //��������� ������ � ����� ���������

  impl->parent = parent;

  if (!parent)
  {
    impl->prev_child = impl->next_child = 0;
    
    if (impl->scene)
      SetScene (0);
      
    BeginUpdate ();
      
      //������������ �������������� ������� ���������
  
    UpdateWorldTransformNotify ();
      
      //��������� �������� � ������������� ������� � ������ ��������

    Notify (EntityEvent_AfterBind);
    
    EndUpdate ();

    return;
  }

    //����������� ����� ������ ���� ����� ������� ����� �������������

  if (mode == EntityBindMode_AddRef)
    parent->AddRef ();

    //������������ ������ � ������ �������� ��������
    
  Impl* parent_impl = parent->impl;

  impl->prev_child   = parent_impl->last_child;
  impl->next_child   = 0;
  parent->impl->last_child = this;

  if (impl->prev_child) impl->prev_child->impl->next_child = this;
  else                  parent_impl->first_child          = this;

    //���������� ��������� �� ����� � ��������

  if (impl->scene != parent_impl->scene)
    SetScene (parent_impl->scene);
    
  BeginUpdate ();
      
      //������������ �������������� ������� ���������
  
  UpdateWorldTransformNotify ();    

      //��������� �������� � ������������� ������� � ������ ��������

  Notify (EntityEvent_AfterBind);
  
  EndUpdate ();
}

void Entity::UnbindChild (const char* name, EntityTransformSpace invariant_space)
{
  UnbindChild (name, EntitySearchMode_OnNextSublevel, invariant_space);
}

void Entity::UnbindChild (const char* name, EntitySearchMode mode, EntityTransformSpace invariant_space)
{
  if (!name)
    RaiseNullArgument ("sg::Entity::UnbindChild", "name");

  Entity* child = FindChild (name, mode);
  
  if (!child)
    return;
    
  child->Unbind (invariant_space);
}

void Entity::UnbindAllChildren ()
{
  while (impl->last_child)
    impl->last_child->Unbind ();
}

//��������� ��������� �� ����� ��� ������� � ���� ��� ��������
void Entity::SetScene (sg::Scene* scene)
{
  impl->scene = scene;
  
  for (Entity* entity=impl->first_child; entity; entity=entity->impl->next_child)
    entity->SetScene (scene);
}

/*
    ����� ������� �� �����
*/

Entity* Entity::FindChild (const char* name, EntitySearchMode mode) //no throw
{
  return const_cast<Entity*> (const_cast<const Entity&> (*this).FindChild (name, mode));
}

const Entity* Entity::FindChild (const char* name, EntitySearchMode mode) const //no throw
{
  if (!name)
    RaiseNullArgument ("sg::Entity::FindChild", "name");    
    
  size_t name_hash = strhash (name);
    
  switch (mode)
  {
    case EntitySearchMode_OnNextSublevel:
      for (Entity* entity=impl->first_child; entity; entity=impl->next_child)
        if (entity->impl->name_hash == name_hash && entity->impl->name == name)
          return entity;

      break;
    case EntitySearchMode_OnAllSublevels:
      for (Entity* entity=impl->first_child; entity; entity=impl->next_child)
      {
        if (entity->impl->name_hash == name_hash && name == entity->impl->name)
          return entity;

        Entity* child = entity->FindChild (name, mode);

        if (child)
          return child;
      }

      break;
    default:
      RaiseInvalidArgument ("sg::Entity::FindChild", "mode", mode);
      break;
  }

  return 0;  
}

/*
    ��������� ������� � ������������ ���������������� �� ���� (���������� �������� Visitor)
*/

void Entity::Accept (Visitor& visitor) const
{
  const_cast<Entity&> (*this).AcceptCore (visitor);
}

void Entity::AcceptCore (Visitor& visitor)
{
  visitor (*this);
}

/*
    ����� ��������
*/

void Entity::Traverse (const TraverseFunction& fn, EntityTraverseMode mode)
{
  switch (mode)
  {
    case EntityTraverseMode_BottomToTop:
      break;
    case EntityTraverseMode_TopToBottom:
      fn (*this);
      break;
    default:
      RaiseInvalidArgument ("sg::Entity::Traverse", "mode", mode);
      break;
  }  

  for (Entity* entity=impl->first_child; entity; entity=entity->impl->next_child)
    entity->Traverse (fn, mode);
    
  if (mode == EntityTraverseMode_BottomToTop)
    fn (*this);
}

void Entity::Traverse (const ConstTraverseFunction& fn, EntityTraverseMode mode) const
{
  switch (mode)
  {
    case EntityTraverseMode_BottomToTop:
      break;
    case EntityTraverseMode_TopToBottom:
      fn (*this);
      break;
    default:
      RaiseInvalidArgument ("sg::Entity::Traverse", "mode", mode);
      break;
  }

  for (const Entity* entity=impl->first_child; entity; entity=entity->impl->next_child)
    entity->Traverse (fn, mode);

  if (mode == EntityTraverseMode_BottomToTop)
    fn (*this);
}

void Entity::TraverseAccept (Visitor& visitor, EntityTraverseMode mode) const
{
  switch (mode)
  {
    case EntityTraverseMode_BottomToTop:
      break;
    case EntityTraverseMode_TopToBottom:
      const_cast<Entity&> (*this).AcceptCore (visitor);
      break;
    default:
      RaiseInvalidArgument ("sg::Entity::Traverse", "mode", mode);
      break;
  }

  for (const Entity* entity=impl->first_child; entity; entity=entity->impl->next_child)
    entity->TraverseAccept (visitor, mode);

  if (mode == EntityTraverseMode_BottomToTop)
    const_cast<Entity&> (*this).AcceptCore (visitor);
}

/*
    ���������� � ������������� ��������� ������� �������������� / �������� ������� ��������������
*/

inline void Entity::UpdateWorldTransformNotify ()
{
  UpdateNotify ();

  if (impl->need_world_transform_update)
    return;

  impl->need_world_transform_update = true;
  impl->need_world_tm_update        = true;
  
    //���������� ���� �������� � ������������� ��������� ������� ��������������
  
  for (Entity* entity=impl->first_child; entity; entity=entity->impl->next_child)
    entity->UpdateWorldTransformNotify ();
}

inline void Entity::UpdateLocalTransformNotify ()
{
  impl->need_local_tm_update = true;
  
  UpdateWorldTransformNotify ();
}

void Entity::UpdateWorldTransform () const
{
  if (impl->parent)
  {
    Entity* parent = impl->parent;
    
    const quatf& parent_orientation = parent->WorldOrientation ();
    
    if (impl->orientation_inherit) impl->world_orientation = parent_orientation * impl->local_orientation;
    else                           impl->world_orientation = impl->local_orientation;

    const vec3f& parent_scale = parent->WorldScale ();

    if (impl->scale_inherit) impl->world_scale = parent_scale * impl->local_scale;
    else                     impl->world_scale = impl->local_scale;
    
    impl->world_position = parent_orientation * (parent_scale * impl->local_position) + parent->WorldPosition ();
  }
  else
  {
    impl->world_orientation = impl->local_orientation;
    impl->world_position    = impl->local_position;
    impl->world_scale       = impl->local_scale;
  }

  impl->need_world_transform_update = false;
}

/*
    ��������� �������
*/

void Entity::SetPosition (const vec3f& position)
{
  impl->local_position = position;

  UpdateLocalTransformNotify ();
}

void Entity::SetPosition  (float x, float y, float z)
{
  SetPosition (vec3f (x, y, z));
}

void Entity::ResetPosition ()
{
  SetPosition (vec3f (0.0f));
}

const vec3f& Entity::Position () const
{
  return impl->local_position;
}

const vec3f& Entity::WorldPosition () const
{
  if (impl->need_world_transform_update)
    UpdateWorldTransform ();
    
  return impl->world_position;
}

/*
    ���������� �������
*/

void Entity::SetOrientation (const quatf& orientation)
{
  impl->local_orientation = orientation;
  
  UpdateLocalTransformNotify ();
}

void Entity::SetOrientation (float angle_in_degrees, float axis_x, float axis_y, float axis_z)
{
  SetOrientation (fromAxisAnglef (deg2rad (angle_in_degrees), axis_x, axis_y, axis_z));
}

void Entity::SetOrientation (float pitch_in_degrees, float yaw_in_degrees, float roll_in_degrees)
{
  SetOrientation (fromEulerAnglef (deg2rad (pitch_in_degrees), deg2rad (yaw_in_degrees), deg2rad (roll_in_degrees)));
}

void Entity::ResetOrientation ()
{
  SetOrientation (quatf ());
}

const quatf& Entity::Orientation () const
{
  return impl->local_orientation;
}

const quatf& Entity::WorldOrientation () const
{
  if (impl->need_world_transform_update)
    UpdateWorldTransform ();
    
  return impl->world_orientation;
}

/*
    ������� �������
*/

void Entity::SetScale (const vec3f& scale)
{
  impl->local_scale = scale;
  
  UpdateLocalTransformNotify ();
}

void Entity::SetScale (float sx, float sy, float sz)
{
  SetScale (vec3f (sx, sy, sz));
}

void Entity::ResetScale ()
{
  SetScale (vec3f (1.0f));
}

const vec3f& Entity::Scale () const
{
  return impl->local_scale;
}

const vec3f& Entity::WorldScale () const
{
  if (impl->need_world_transform_update)
    UpdateWorldTransform ();
    
  return impl->world_scale;
}

/*
    ���������� ������������� ��������������
*/

//��������� ����� ������������ ������������ ����������
void Entity::SetOrientationInherit (bool state)
{
  if (state == impl->orientation_inherit)
    return;

  impl->orientation_inherit = state;

  UpdateWorldTransformNotify ();
}

//����������� �� ������������ ����������
bool Entity::IsOrientationInherited () const
{
  return impl->orientation_inherit;
}

//��������� ����� ������������ ������������� ��������
void Entity::SetScaleInherit (bool state)
{
  if (state == impl->scale_inherit)
    return;

  impl->scale_inherit = state;

  UpdateWorldTransformNotify ();
}

//����������� �� ������������ �������
bool Entity::IsScaleInherited () const
{
  return impl->scale_inherit;
}

/*
    ������������ ��������������
*/

void Entity::Translate (const math::vec3f& offset, EntityTransformSpace space)
{
  switch (space) 
  {
    case EntityTransformSpace_Local:
      impl->local_position += impl->local_orientation * offset;
      break;
    case EntityTransformSpace_Parent:
      impl->local_position += offset;
      break;
    case EntityTransformSpace_World:
      if (impl->parent) impl->local_position += invert (impl->parent->WorldOrientation ()) * offset / impl->parent->WorldScale (); 
      else              impl->local_position += offset;

      break;
    default:
      RaiseInvalidArgument ("sg::Entity::Translate", "space", space);
      break;
  }
  
  UpdateLocalTransformNotify ();
}

void Entity::Translate (float offset_x, float offset_y, float offset_z, EntityTransformSpace space)
{
  Translate (vec3f (offset_x, offset_y, offset_z), space);
}

void Entity::Rotate (const math::quatf& q, EntityTransformSpace space)
{
  switch (space)
  {
    case EntityTransformSpace_Local:
      impl->local_orientation *= q;
      break;    
    case EntityTransformSpace_Parent:
      impl->local_orientation = q * impl->local_orientation;
      break;
    case EntityTransformSpace_World:
    {
      const quatf& world_orientation = WorldOrientation ();

      impl->local_orientation = impl->local_orientation * invert (world_orientation) * q * world_orientation;
      break;
    }      
    default:
      RaiseInvalidArgument ("sg::Entity::Rotate", "space", space);
      break;
  }

  UpdateLocalTransformNotify ();
}

void Entity::Rotate (float angle_in_degrees, float axis_x, float axis_y, float axis_z, EntityTransformSpace space)
{
  Rotate (fromAxisAnglef (deg2rad (angle_in_degrees), axis_x, axis_y, axis_z), space);
}

void Entity::Rotate (float pitch_in_degrees, float yaw_in_degrees, float roll_in_degrees, EntityTransformSpace space)
{
  Rotate (fromEulerAnglef (deg2rad (pitch_in_degrees), deg2rad (yaw_in_degrees), deg2rad (roll_in_degrees)), space);
}

void Entity::Scale (const math::vec3f& scale)
{
  impl->local_scale *= scale;  
  
  UpdateLocalTransformNotify ();
}

void Entity::Scale (float sx, float sy, float sz)
{
  Scale (vec3f (sx, sy, sz));
}

/*
    ��������� ������ �������������� �������
*/

namespace
{

//���������� ��������������
void affine_compose (const vec3f& position, const quatf& orientation, const vec3f& scale, mat4f& tm)
{
  tm = math::translate (position) * mat4f (orientation) * math::scale (scale);
}

}

const mat4f& Entity::TransformationMatrix (EntityTransformSpace space) const
{
  static mat4f identity;

  switch (space)
  {
    case EntityTransformSpace_Local:
      if (impl->need_local_tm_update)
      {
        affine_compose (impl->local_position, impl->local_orientation, impl->local_scale, impl->local_tm);

        impl->need_local_tm_update = false;
      }

      return impl->local_tm;
    case EntityTransformSpace_Parent:
      return impl->parent ? impl->parent->TransformationMatrix (EntityTransformSpace_World) : identity;
    case EntityTransformSpace_World:
      if (impl->need_world_tm_update)
      {
        affine_compose (WorldPosition (), WorldOrientation (), WorldScale (), impl->world_tm);

        impl->need_world_tm_update = false;
      }

      return impl->world_tm;
    default:
      RaiseInvalidArgument ("sg::Entity::TransformationMatrix", "space", space);
      return identity;
  } 
}

/*
    ��������� ������� �������������� ������� object � ������� ��������� ������� �������
*/

mat4f Entity::ObjectTM (Entity& object) const
{
  return invert (WorldTM ()) * object.WorldTM ();
}

/*
    �������� �� ������� Entity
*/

Entity::Signal& Entity::Listeners (EntityEvent event)
{
  return const_cast<Signal&> (const_cast<const Entity&> (*this).Listeners (event));
}

const Entity::Signal& Entity::Listeners (EntityEvent event) const
{
  if (event < 0 || event >= EntityEvent_Num)
    RaiseInvalidArgument ("sg::Entity::Listeners", "event", event);
    
  return impl->signals [event];
}

/*
    ���������� �������� � ����������� �������
*/

void Entity::Notify (EntityEvent event)
{
    //��������� ��� �� ������������ ������

  if (impl->signal_process [event])
    return;
    
    //������������� ���� ��������� �������

  impl->signal_process [event] = true;
  
    //�������� ����������� �������

  try
  {
    impl->signals [event] (*this);
  }
  catch (...)
  {
    //��� ���������� ���������� ������������ ������� ������� �����������
  }
  
    //������� ���� ��������� �������
  
  impl->signal_process [event] = false;
}

/*
    ���������� ������������ ����������
*/

void Entity::BeginUpdate ()
{
  impl->update_lock++;
}

void Entity::EndUpdate ()
{
  if (!impl->update_lock)
    RaiseNotSupported ("sg::Entity::EndUpdate", "Attempt to call EndUpdate without previous BeginUpdate call");  
    
  if (!--impl->update_lock)
  {
      //���� �� ����� ���������� ���������� ������ ��������� - ��������� �������� �� ����������
    
    if (impl->update_notify)
    {
      Notify (EntityEvent_AfterUpdate);
      
      impl->update_notify = false;
    }
  }
}

bool Entity::IsInUpdateTransaction () const
{
  return impl->update_lock > 0;  
}

//���������� �� ��������� �������
void Entity::UpdateNotify ()
{
    //���� ������ �� ��������� � ���������� ���������� - ��������� �������� �� ����������

  if (!impl->update_lock)
  {
    Notify (EntityEvent_AfterUpdate);
    return;
  }

    //����� ������������� ����, ��������������� � ������������� ���������� �������� �� ���������� �� ���������� ���������
    //����������

  impl->update_notify = true;    
}
