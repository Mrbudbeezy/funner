#ifndef MEDIALIB_GEOMETRY_DEFINES_HEADER
#define MEDIALIB_GEOMETRY_DEFINES_HEADER

#include <stddef.h>

namespace medialib
{

namespace geometry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CloneMode
{
  CloneMode_Copy,     //�����������
  CloneMode_Instance, //��������������� (���������� ����� ������)
  CloneMode_Source,   //����������� ����� ����������� ���������  
  
  CloneMode_Default = CloneMode_Copy
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VertexWeight
{
  size_t joint_index;  //������ ����������
  float  joint_weight; //��� ����������
  
  VertexWeight ();
  VertexWeight (size_t joint_index, float joint_weight);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VertexInfluence
{
  size_t first_weight, weights_count;
  
  VertexInfluence ();
  VertexInfluence (size_t first_weight, size_t weights_count);
};

#include <media/geometry/detail/defs.inl>

}

}

#endif
