#include <sg/camera.h>
#include <xtl/visitor.h>
#include <common/exception.h>

using namespace scene_graph;
using namespace math;
using namespace common;

const float EPS = 1e-6f;

/*
    �������� ���������� PerspectiveCamera
*/

struct PerspectiveCamera::Impl
{
  float fov_x;  //���� ������ �� �����������
  float fov_y;  //���� ������ �� ���������
  float z_near; //������� ��������� ���������
  float z_far;  //������� ��������� ���������
};

/*
    ����������� / ����������
*/

PerspectiveCamera::PerspectiveCamera ()
  : impl (new Impl)
{
  impl->fov_x  = 1;
  impl->fov_y  = 1;
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

PerspectiveCamera* PerspectiveCamera::Create ()
{
  return new PerspectiveCamera;
}

/*
    ���������
*/

void PerspectiveCamera::SetFovX (float fov_x)
{
  impl->fov_x = fov_x;
}

float PerspectiveCamera::FovX () const
{
  return impl->fov_x;
}

void PerspectiveCamera::SetFovY (float fov_y)
{
  impl->fov_y = fov_y;
}

float PerspectiveCamera::FovY () const
{
  return impl->fov_y; 
}

void PerspectiveCamera::SetZNear (float z_near)
{
  impl->z_near = z_near;
}

float PerspectiveCamera::ZNear () const
{
  return impl->z_near;
}

void PerspectiveCamera::SetZFar  (float z_far)
{
  impl->z_far = z_far;
}

float PerspectiveCamera::ZFar () const
{
  return impl->z_far;
}

void PerspectiveCamera::ComputeProjectionMatrix (math::mat4f& proj_matrix)
{
  float width  = tan (deg2rad (impl->fov_x)) * impl->z_near, 
        height = tan (deg2rad (impl->fov_y)) * impl->z_near,
        depth  = impl->z_far - impl->z_near;

  if (fabs (width)  < EPS) Raise <Exception> ("scene_graph::PerspectiveCamera::ComputeProjectionMatrix", "Zero camera width");
  if (fabs (height) < EPS) Raise <Exception> ("scene_graph::PerspectiveCamera::ComputeProjectionMatrix", "Zero camera height");
  if (fabs (depth) < EPS)  Raise <Exception> ("scene_graph::PerspectiveCamera::ComputeProjectionMatrix", "Zero camera depth");

  proj_matrix [0] = vec4f (2.0f * impl->z_near / width, 0, 0, 0);
  proj_matrix [1] = vec4f (0, 2.0f * impl->z_near / height, 0, 0);
  proj_matrix [2] = vec4f (0, 0, -(impl->z_far + impl->z_near) / depth, -2.0f * impl->z_near * impl->z_far / depth);
  proj_matrix [3] = vec4f (0, 0, -1, 0);
}

/*
    ������������ ���������������    
*/

void PerspectiveCamera::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Camera::AcceptCore (visitor);
}
