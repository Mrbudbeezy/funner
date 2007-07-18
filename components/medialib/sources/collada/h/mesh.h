#include <stl/vector>
#include <stl/string>
#include <math/mathlib.h>
#include <media/collada.h>

using namespace stl;
using namespace math;
using namespace medialib;

////////////////////////////////////////////////////////////////////////////////////////
///����� ⥪���୮�� ������
////////////////////////////////////////////////////////////////////////////////////////
struct TexData
{
  vector <ColladaTexVertex> tex_vertices;  //���ᨢ ⥪������ ������
  string                    channel_name;  //��� ������� ⥪���୮�� ������
};

namespace medialib
{

////////////////////////////////////////////////////////////////////////////////////////
///����७��� ������� �����孮��
////////////////////////////////////////////////////////////////////////////////////////
class ColladaSurfaceImpl : public ColladaEntityImpl
{
  public:
    vector <ColladaVertex> vertices;      //���ᨢ ���設
    vector <TexData>       tex_data;      //���ᨢ ⥪������ ������
    vector <vec4f>         colors;        //梥� ���設, ����� �����⢮����
    vector <size_t>        indexes;       //���ᨢ �����ᮢ ��� ����஥���
    ColladaPrimitiveType   type;          //⨯ �ਬ�⨢��, ��⠢����� �����孮���
    string                 material;      //��� ���ਠ��
};

////////////////////////////////////////////////////////////////////////////////////////
///����७��� ������� ���
////////////////////////////////////////////////////////////////////////////////////////
class ColladaMeshImpl : public ColladaEntityImpl
{
  public:
    vector <ColladaSurfaceImpl> surfaces;  //���ᨢ ���୨� �����孮�⥩
};

}
