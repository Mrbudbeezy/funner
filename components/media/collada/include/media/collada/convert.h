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

namespace physics
{

class PhysicsLibrary;

}

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������-������ � ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
void convert (const collada::Model& source, geometry::MeshLibrary& destination, bool fix_zero_tangents = false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������-������ � ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
void convert (const collada::Model& source, animation::AnimationLibrary& destination);
void convert (const collada::Model& source, animation::AnimationLibrary& destination, const char* merge_animation);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������-������ � ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
void convert_triangle_meshes (const collada::Model& source, physics::PhysicsLibrary& destination);

}

}

#endif
