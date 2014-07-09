#ifndef RENDER_MANAGER_ENTITY_HEADER
#define RENDER_MANAGER_ENTITY_HEADER

#include <common/property_map.h>

#include <math/matrix.h>

#include <render/manager/bounds.h>
#include <render/manager/primitive.h>

namespace render
{

namespace manager
{

//implementation forwards
class EntityImpl;
class Wrappers;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Entity
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Entity  (const Entity&);
    ~Entity ();
    
    Entity& operator = (const Entity&);   

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetProperties (const common::PropertyMap&);
    const common::PropertyMap& Properties    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����-����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetShaderOptions (const common::PropertyMap&);
    const common::PropertyMap& ShaderOptions    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetWorldMatrix     (const math::mat4f&);
    const math::mat4f& WorldMatrix        () const;
    const math::mat4f& InverseWorldMatrix () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetUserData (void* data);
    void* UserData    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������� (��� ���������)
///  �������������� ���������� �� ������� Entity::Transformation � ������ ���� ��� �� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetJointsCount         (size_t count);
    size_t             JointsCount            () const;
    void               SetJointTransformation (size_t joint_index, const math::mat4f&);
    const math::mat4f& JointTransformation    (size_t joint_index) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t LodsCount () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    manager::Primitive Primitive           (size_t level_of_detail = 0) const;
    const char*        PrimitiveName       (size_t level_of_detail = 0) const;
    void               SetPrimitive        (const manager::Primitive&, size_t level_of_detail = 0);
    void               SetPrimitive        (const char* name, size_t level_of_detail = 0);
    void               ResetPrimitive      (size_t level_of_detail = 0);
    bool               HasPrimitive        (size_t level_of_detail = 0) const;
    void               ResetAllPrimitives  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� � ��������� ������� ��������� ������� ��� ������� ����������� �� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
   void               SetLodPoint (const math::vec3f&);
   const math::vec3f& LodPoint    () const;
   
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
   void           SetScissor      (const BoxArea& scissor);
   const BoxArea& Scissor         () const;
   void           SetScissorState (bool state);
   void           EnableScissor   () { SetScissorState (true); }
   void           DisableScissor  () { SetScissorState (false); }
   bool           ScissorState    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Entity&);
    
  private:
    Entity (EntityImpl*);
    
  private:
    EntityImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Entity&, Entity&);

}

}

#endif
