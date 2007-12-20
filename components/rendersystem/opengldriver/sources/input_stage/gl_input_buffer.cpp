#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///����������� / ����������
Buffer::Buffer (const BufferDesc& desc)
{
  bufferdesc = desc;
}

///��������� �����������
void Buffer::GetDesc (BufferDesc& desc)
{
  desc = bufferdesc;
}
