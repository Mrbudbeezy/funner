#ifndef MODELER_CORE_HEADER
#define MODELER_CORE_HEADER

#include <math.h>

#include <vector>

/*
    ��������� �������
*/

struct ModelData
{
  double  A, B, C;     //������ ������� A,B,C
  double  h, g;        //���������� �������������� h,g
  double  mx,my,mz;    //���������� ������ �������
  int     ini;         //???
  double  len, dm;     //����� ��������� �������������� � �������
};

/*
    �������� ������ �� �����
*/

void LoadModelData (const char* file_name, ModelData& model_data);

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

typedef Vec3<float>    Vec3f;
typedef Color4<float>  Color4f;

/*
    ��������������� ����� �����������
*/

struct DrawVertex
{
  Vec3f   position; //��������� ����� � ������������
  Color4f color;    //���� �����
};

/*
    ��������������� ��������
*/

enum PrimitiveType
{
  PrimitiveType_LineList
};

struct DrawPrimitive
{
  PrimitiveType type;  //��� ���������
  size_t        first; //����� ������� �������� (������� / ��������)
  size_t        count; //���������� ��������� (������ / ��������)
};


/*
    ���������� ����������
*/

typedef std::vector<DrawVertex>    DrawVertexArray;
typedef std::vector<DrawPrimitive> DrawPrimitiveArray;

void BuildTrajectory (const ModelData& model_data, double nu1, double nu2, double nu3, size_t vertices_count, DrawVertexArray& out_vertices, DrawPrimitiveArray& out_primitives);

#endif
