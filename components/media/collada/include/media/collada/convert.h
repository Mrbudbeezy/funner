#ifndef MEDIALIB_COLLADA_CONVERT_HEADER
#define MEDIALIB_COLLADA_CONVERT_HEADER

namespace media
{

//forward declarations

namespace geometry
{

class MeshLibrary;

}

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������-������ � ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
void convert (const collada::Model& source, geometry::MeshLibrary& destination);

}

}

#endif
