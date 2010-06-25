#ifndef MEDIALIB_COLLADA_CONVERT_HEADER
#define MEDIALIB_COLLADA_CONVERT_HEADER

namespace media
{

//forward declarations

namespace geometry
{

class MeshLibrary;

}

namespace animation
{

class AnimationLibrary;

}

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������-������ � ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
void convert (const collada::Model& source, geometry::MeshLibrary& destination);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������-������ � ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
void convert (const collada::Model& source, animation::AnimationLibrary& destination);
void convert (const collada::Model& source, animation::AnimationLibrary& destination, const char* merge_animation);

}

}

#endif
