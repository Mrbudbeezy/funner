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
enum BufferCloneMode
{
  BufferCloneMode_Copy,     //�����������
  BufferCloneMode_Instance, //��������������� (���������� ����� ������)
  BufferCloneMode_Default   //����������� ����� ����������� ���������
};

}

}

#endif
