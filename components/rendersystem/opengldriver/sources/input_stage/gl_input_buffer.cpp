#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����� Buffer
///////////////////////////////////////////////////////////////////////////////////////////////////

///����������� / ����������
Buffer::Buffer (const BufferDesc&)
{
}

///��������� �����������
void Buffer::GetDesc (BufferDesc&)
{
  common::RaiseNotImplemented("render::low_level::opengl::Buffer::GetDesc");
}
