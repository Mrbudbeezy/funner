#include <stl/vector>
#include <stl/string>
#include <math/mathlib.h>
#include <media/collada_geometry.h>

using namespace stl;
using namespace math;

////////////////////////////////////////////////////////////////////////////////////////
///����७��� ������� ᯫ����
////////////////////////////////////////////////////////////////////////////////////////
class ColladaSplineImpl
{
  public:
    bool                          closed;         //�����뢠��, ������� �� ᥣ����, ᮥ�����騩 ����� � ��᫥���� ����஫�� ���設�. �� 㬮�砭�� false (�����).
    vector <vec4f>                cvpos;          //����樨 ����஫��� ���設
    vector <ColladaInterpolation> interpolations; //��� ���௮��樨
};
