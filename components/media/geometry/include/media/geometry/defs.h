#ifndef MEDIALIB_GEOMETRY_DEFINES_HEADER
#define MEDIALIB_GEOMETRY_DEFINES_HEADER

#include <cstddef>

namespace media
{

namespace geometry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VertexWeight
{
  uint32_t joint_index;  //������ ����������
  float    joint_weight; //��� ����������
  
  VertexWeight ();
  VertexWeight (uint32_t joint_index, float joint_weight);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VertexInfluence
{
  uint32_t first_weight, weights_count;
  
  VertexInfluence ();
  VertexInfluence (uint32_t first_weight, uint32_t weights_count);
};

#include <media/geometry/detail/defs.inl>

}

}

#endif
