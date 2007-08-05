#include <common/exception.h>
#include <sg/camera.h>
#include <xtl/visitor.h>

using namespace sg;
using namespace math;
using namespace common;

const float EPS = 0.01f;

/*
    �������� ���������� Camera
*/

struct Camera::Impl
{
  mat4f proj_matrix;  //������� �������������
  int   dirty_pm;     //��� true ������� ������������� ��������� � �����������
};

/*
    ����������� / ����������
*/

Camera::Camera ()
  : impl (new Impl)
{
  impl->proj_matrix = 1;
  impl->dirty_pm    = 1;
}

Camera::~Camera ()
{
  delete impl;
}

/*
    ������� ��������
*/

const mat4f& Camera::ProjectionMatrix ()
{
  if (impl->dirty_pm)
  {
    ComputeProjectionMatrix (impl->proj_matrix);
    impl->dirty_pm = 0;
  }
  return impl->proj_matrix;
}

/*
    ������ ���������� �������
*/

void Camera::UpdateNotify ()
{
  impl->dirty_pm = 1;
}

/*
    ������������ ���������������    
*/

void Camera::AcceptCore (Visitor& visitor)
{
  visitor (*this);
}

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
        height = tan (deg2rad (impl->fov_y)) * impl->z_near;

  if (abs(width) < EPS)
    Raise <Exception> ("PerspectiveCamera::ComputeProjectionMatrix", "Zero camera width");
  if (abs(height) < EPS)
    Raise <Exception> ("PerspectiveCamera::ComputeProjectionMatrix", "Zero camera height");
  if (abs(impl->z_far - impl->z_near) < EPS)
    Raise <Exception> ("PerspectiveCamera::ComputeProjectionMatrix", "Zero camera depth");

  proj_matrix [0] = vec4f (2.0f * impl->z_near / width, 0, 0, 0);
  proj_matrix [1] = vec4f (0, 2.0f * impl->z_near / height, 0, 0);
  proj_matrix [2] = vec4f (0, 0, -(impl->z_far + impl->z_near) / (impl->z_far - impl->z_near), -2.0f * impl->z_near * impl->z_far / (impl->z_far - impl->z_near));
  proj_matrix [3] = vec4f (0, 0, -1, 0);
}

/*
    ������������ ���������������    
*/

void PerspectiveCamera::AcceptCore (Visitor& visitor)
{
  TrackAccept (*this, visitor);
}

/*
    �������� ���������� OrthoCamera
*/

struct OrthoCamera::Impl
{
  float left;   //����� ������� ������
  float right;  //������ ������� ������
  float top;    //������� ������� ������
  float bottom; //������ ������� ������
  float z_near; //������� ��������� ���������
  float z_far;  //������� ��������� ���������
};

/*
    ����������� / ����������
*/

OrthoCamera::OrthoCamera ()
  : impl (new Impl)
{
  impl->left   = 0;
  impl->right  = 1;
  impl->top    = 1;
  impl->bottom = 0;
  impl->z_near = 0;
  impl->z_far  = 1;
}

OrthoCamera::~OrthoCamera ()
{
  delete impl;
}

/*
    �������� ������
*/

OrthoCamera* OrthoCamera::Create ()
{
  return new OrthoCamera;
}

/*
    ���������
*/

void OrthoCamera::SetLeft (float left)
{
  impl->left = left;
}

float OrthoCamera::Left () const
{
  return impl->left;
}

void OrthoCamera::SetRight (float right)
{
  impl->right = right;
}

float OrthoCamera::Right () const
{
  return impl->right; 
}

void OrthoCamera::SetTop (float top)
{
  impl->top = top;
}

float OrthoCamera::Top () const
{
  return impl->top; 
}

void OrthoCamera::SetBottom (float bottom)
{
  impl->bottom = bottom;
}

float OrthoCamera::Bottom () const
{
  return impl->bottom; 
}

void OrthoCamera::SetZNear (float z_near)
{
  impl->z_near = z_near;
}

float OrthoCamera::ZNear () const
{
  return impl->z_near;
}

void OrthoCamera::SetZFar  (float z_far)
{
  impl->z_far = z_far;
}

float OrthoCamera::ZFar () const
{
  return impl->z_far;
}

void OrthoCamera::ComputeProjectionMatrix (math::mat4f& proj_matrix)
{
  if (abs(impl->right - impl->left) < EPS)
    Raise <Exception> ("OrthoCamera::ComputeProjectionMatrix", "Zero camera width");
  if (abs(impl->top - impl->bottom) < EPS)
    Raise <Exception> ("OrthoCamera::ComputeProjectionMatrix", "Zero camera height");
  if (abs(impl->z_far - impl->z_near) < EPS)
    Raise <Exception> ("OrthoCamera::ComputeProjectionMatrix", "Zero camera depth");
  
  proj_matrix [0] = vec4f (2.0f / (impl->right - impl->left), 0, 0, - (impl->right + impl->left) / (impl->right - impl->left));
  proj_matrix [1] = vec4f (0, 2.0f / (impl->top - impl->bottom), 0, - (impl->top + impl->bottom) / (impl->top - impl->bottom));
  proj_matrix [2] = vec4f (0, 0, -2.0f / (impl->z_far - impl->z_near), (impl->z_near + impl->z_far) / (-impl->z_far - impl->z_near));
  proj_matrix [3] = vec4f (0, 0, 0, 1);
}

/*
    ������������ ���������������    
*/

void OrthoCamera::AcceptCore (Visitor& visitor)
{
  TrackAccept (*this, visitor);
}
