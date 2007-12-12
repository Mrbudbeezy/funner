#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� - VboBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////

///����������� / ����������
VboBuffer::VboBuffer (const ContextManager&, GLenum target, const BufferDesc&)
{
}

VboBuffer::~VboBuffer ()
{
}

///������ � ������� ������
void VboBuffer::SetData (size_t offset, size_t size, const void* data)
{
  common::RaiseNotImplemented("render::low_level::opengl::VboBuffer::SetData");
}

void VboBuffer::GetData (size_t offset, size_t size, void* data)
{
  common::RaiseNotImplemented("render::low_level::opengl::VboBuffer::GetData");
}

///��������� ������ � �������� OpenGL
void VboBuffer::Bind ()
{
  common::RaiseNotImplemented("render::low_level::opengl::VboBuffer::Bind");
}

///C������� �� ������ ������ (��� ���������� �������)
void* VboBuffer::GetDataPointer ()
{
  common::RaiseNotImplemented("render::low_level::opengl::VboBuffer::GetDataPointer");
  return 0;
}
