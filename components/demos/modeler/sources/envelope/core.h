#ifndef MODELER_CORE_HEADER
#define MODELER_CORE_HEADER

#include <vector>

namespace modeler
{

/*
    ��������� �������
*/

struct ModelData
{
  double  A, B, C;     //������ ������� A,B,C
  double  h, g;        //���������� �������������� h,g
  double  nu1,nu2,nu3; //???
  double  mx,my,mz;    //���������� ������ �������
  int     ini;         //???
  double  len, dm;     //����� ��������� �������������� � �������
};

/*
    ������� ��������� ������
*/

template <class T> struct Vec3
{
  T x, y, z;
};

template <class T> struct Color4
{
  T r, g, b, a;
};

typedef Vec3<double>   Vec3d;
typedef Vec3<float>    Vec3f;
typedef Color4<double> Color4d;
typedef Color4<float>  Color4f;

/*
    ��������������� ����� �����������
*/

struct DrawVertex
{
  Vec3f   position; //��������� ����� � ������������
  Vec3f   normal;   //������� � ����������� � �����
  Color4f color;    //���� �����
};

/*
    ��������������� ��������
*/

enum PrimitiveType
{
  PrimitiveType_TriangleStrip
};

struct DrawPrimitive
{
  PrimitiveType type;  //��� ���������
  size_t        first; //����� ������� �������� (������� / ��������)
  size_t        count; //���������� ��������� (������ / ��������)
};

/*
    �������� ������ �� �����
*/

void LoadModelData (const char* file_name, ModelData& model_data);

/*
    ���������� ����-�����������
*/

typedef std::vector<DrawVertex>    DrawVertexArray;
typedef std::vector<DrawPrimitive> DrawPrimitiveArray;

void BuildMegaSurface (const ModelData& model_data, int uanz, int vanz, DrawVertexArray& out_vertices, DrawPrimitiveArray& out_primitives);

}

#endif
