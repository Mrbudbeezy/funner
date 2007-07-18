#include <stl/string>
#include <media/collada.h>

namespace medialib
{

class ColladaCameraImpl : public ColladaEntityImpl
{
  public:
    ColladaCameraType type;
    float             x, y, aspect_ratio, znear, zfar;  // ��� ���ᯥ�⨢� x � y - xfov yfov, � ��� ��⮣�䨨 - xmag ymag
};

}
