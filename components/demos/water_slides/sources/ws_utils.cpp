#include "shared.h"

//�������� ���������� ����� � ������
stl::string load_text_file (const char* file_name)
{
  common::InputFile file (file_name);
  
  stl::string buffer (file.Size (), ' ');

  file.Read (&buffer [0], file.Size ());

  return buffer;
}

//��������� ��������������� ������� ��������
math::mat4f get_ortho_proj (float left, float right, float bottom, float top, float znear, float zfar)
{
  math::mat4f proj_matrix;
  
  float width  = right - left,
        height = top - bottom,
        depth  = zfar - znear;  

    //������� ������� �������������, ������������ gluOrtho2D

  proj_matrix [0] = math::vec4f (2.0f / width, 0, 0, - (right + left) / width);
  proj_matrix [1] = math::vec4f (0, 2.0f / height, 0, - (top + bottom) / height);
  proj_matrix [2] = math::vec4f (0, 0, -2.0f / depth, - (znear + zfar) / depth);
  proj_matrix [3] = math::vec4f (0, 0, 0, 1);
  
  return proj_matrix;
}

//��������� ������������� ������� ��������
math::mat4f get_perspective_proj (float fov_x, float fov_y, float znear, float zfar)
{
  math::mat4f proj_matrix;

  float width  = tan (math::deg2rad (fov_x)) * znear,
        height = tan (math::deg2rad (fov_y)) * znear,
        depth  = zfar - znear;

  proj_matrix [0] = math::vec4f (2.0f * znear / width, 0, 0, 0);
  proj_matrix [1] = math::vec4f (0, 2.0f * znear / height, 0, 0);
  proj_matrix [2] = math::vec4f (0, 0, -(zfar + znear) / depth, -2.0f * znear * zfar / depth);
  proj_matrix [3] = math::vec4f (0, 0, -1, 0);
  
  return proj_matrix;
}
