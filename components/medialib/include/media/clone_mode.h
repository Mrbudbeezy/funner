#ifndef MEDIALIB_CLONE_MODE_HEADER
#define MEDIALIB_CLONE_MODE_HEADER

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CloneMode
{
  CloneMode_Copy,     //�����������
  CloneMode_Instance, //��������������� (���������� ����� ������)
  
  CloneMode_Default = CloneMode_Copy
};

}

#endif

