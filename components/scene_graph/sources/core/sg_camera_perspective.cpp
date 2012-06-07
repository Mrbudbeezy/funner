#include "shared.h"

using namespace scene_graph;
using namespace math;
using namespace common;
using namespace bound_volumes;

const float EPS = 1e-6f;

/*
    �������� ���������� PerspectiveCamera
*/

struct PerspectiveCamera::Impl: public xtl::instance_counter<PerspectiveCamera>
{
  anglef fov_x;  //���� ������ �� �����������
  anglef fov_y;  //���� ������ �� ���������
  float  z_near; //������� ��������� ���������
  float  z_far;  //������� ��������� ���������
};

/*
    ����������� / ����������
*/

PerspectiveCamera::PerspectiveCamera ()
  : impl (new Impl)
{
  impl->fov_x  = degree (1.0f);
  impl->fov_y  = degree (1.0f);
  impl->z_near = 0;
  impl->z_far  = 1;
}

PerspectiveCamera::~PerspectiveCamera ()
{
  delete impl;
}

/*
    �������� ������
*/

PerspectiveCamera::Pointer PerspectiveCamera::Create ()
{
  return Pointer (new PerspectiveCamera, false);
}

/*
    ���������
*/

void PerspectiveCamera::SetFovX (const math::anglef& fov_x)
{
  impl->fov_x = fov_x;
  UpdateBoundsNotify ();
  UpdateNotify ();
}

const math::anglef& PerspectiveCamera::FovX () const
{
  return impl->fov_x;
}

void PerspectiveCamera::SetFovY (const math::anglef& fov_y)
{
  impl->fov_y = fov_y;
  UpdateBoundsNotify ();
  UpdateNotify ();
}

const math::anglef& PerspectiveCamera::FovY () const
{
  return impl->fov_y; 
}

void PerspectiveCamera::SetZNear (float z_near)
{
  impl->z_near = z_near;
  UpdateBoundsNotify ();
  UpdateNotify ();
}

float PerspectiveCamera::ZNear () const
{
  return impl->z_near;
}

void PerspectiveCamera::SetZFar  (float z_far)
{
  impl->z_far = z_far;
  UpdateBoundsNotify ();
  UpdateNotify ();
}

float PerspectiveCamera::ZFar () const
{
  return impl->z_far;
}

void PerspectiveCamera::ComputeProjectionMatrix (math::mat4f& proj_matrix)
{
  float width  = 2.f * tan (impl->fov_x * 0.5f) * impl->z_near,
        height = 2.f * tan (impl->fov_y * 0.5f) * impl->z_near,
        depth  = impl->z_far - impl->z_near;

  if (fabs (width)  < EPS) throw xtl::format_operation_exception ("scene_graph::PerspectiveCamera::ComputeProjectionMatrix", "Zero camera width");
  if (fabs (height) < EPS) throw xtl::format_operation_exception ("scene_graph::PerspectiveCamera::ComputeProjectionMatrix", "Zero camera height");
  if (fabs (depth) < EPS)  throw xtl::format_operation_exception ("scene_graph::PerspectiveCamera::ComputeProjectionMatrix", "Zero camera depth");

  proj_matrix [0] = vec4f (2.0f * impl->z_near / width, 0, 0, 0);
  proj_matrix [1] = vec4f (0, 2.0f * impl->z_near / height, 0, 0);
  proj_matrix [2] = vec4f (0, 0, (impl->z_far + impl->z_near) / depth, -2.0f * impl->z_near * impl->z_far / depth); //�� �����, ��������� ��� ����������� ������������ ��������������� ������� ����  
  proj_matrix [3] = vec4f (0, 0, 1, 0);
}

/*
   ������� ��������������� ������
*/

void PerspectiveCamera::UpdateBoundsCore ()
{
  float half_width  = tan (impl->fov_x / 2) * stl::max (fabs (impl->z_near), fabs (impl->z_far)), 
        half_height = tan (impl->fov_y / 2) * stl::max (fabs (impl->z_near), fabs (impl->z_far));

  SetBoundBox (axis_aligned_box <float> (-half_width, -half_height, -impl->z_far,
                                          half_width,  half_height, -impl->z_near));
}

/*
    ������������ ���������������
*/

void PerspectiveCamera::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Camera::AcceptCore (visitor);
}

/*
    ���������� �������
*/

void PerspectiveCamera::BindProperties (common::PropertyBindingMap& bindings)
{
  Camera::BindProperties (bindings);

  bindings.AddProperty ("FovX", xtl::bind (&PerspectiveCamera::FovX, this), xtl::bind (&PerspectiveCamera::SetFovX, this, _1));
  bindings.AddProperty ("FovY", xtl::bind (&PerspectiveCamera::FovY, this), xtl::bind (&PerspectiveCamera::SetFovY, this, _1));
  bindings.AddProperty ("ZNear", xtl::bind (&PerspectiveCamera::ZNear, this), xtl::bind (&PerspectiveCamera::SetZNear, this, _1));
  bindings.AddProperty ("ZFar", xtl::bind (&PerspectiveCamera::ZFar, this), xtl::bind (&PerspectiveCamera::SetZFar, this, _1));
}
