#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    ��������� ��������� �������
*/

void Object::GetFamily (ObjectFamily& family)
{
  family.id     = (size_t)&typeid (Object);
  family.sub_id = 0;
}
