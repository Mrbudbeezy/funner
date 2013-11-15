#include "shared.h"

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;

/*
    �������� ���������� Camera
*/

struct Camera::Impl: public xtl::instance_counter<Camera>
{
  mat4f       proj_matrix;     //������� �������������
  plane_listf frustum;         //�������� ���������
  bool        dirty_pm;        //��� true ������� ������������� ��������� � �����������
  bool        dirty_frustum;   //��� true �������� ��������� ��������� � �����������
};

/*
    ����������� / ����������
*/

Camera::Camera ()
  : impl (new Impl)
{
  impl->dirty_pm      = true;
  impl->dirty_frustum = true;

  static const size_t FRUSTUM_PLANES_COUNT = 6;

  impl->frustum.reserve (FRUSTUM_PLANES_COUNT);
}

Camera::~Camera ()
{
  delete impl;
}

/*
    ������� ��������
*/

const mat4f& Camera::ProjectionMatrix () const
{
  if (impl->dirty_pm)
  {
    const_cast<Camera&> (*this).ComputeProjectionMatrix (impl->proj_matrix);
    
    impl->dirty_pm = false;
  }

  return impl->proj_matrix;
}

/*
    ��������� �������� ���������
*/

const plane_listf& Camera::Frustum () const
{
  if (impl->dirty_frustum)
  {
    impl->frustum.clear ();

    add_frustum (ProjectionMatrix () * math::inverse (WorldTM ()), impl->frustum);

    impl->dirty_frustum = false;
  }
  
  return impl->frustum;
}

/*
    ������ ���������� �������
*/

void Camera::UpdateCameraNotify ()
{
  impl->dirty_pm      = true;
  impl->dirty_frustum = true;

  UpdateNotify ();
}

void Camera::AfterUpdateWorldTransformEvent ()
{
  impl->dirty_frustum = true;
}

/*
    �����, ���������� ��� ��������� �������
*/

void Camera::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
