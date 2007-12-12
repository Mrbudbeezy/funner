#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� - SystemMemoryBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////

SystemMemoryBuffer::SystemMemoryBuffer (const BufferDesc&)
{
  buffer = NULL;
}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
}

///������ � ������� ������
void SystemMemoryBuffer::SetData (size_t offset, size_t size, const void* data)
{
  common::RaiseNotImplemented("render::low_level::opengl::SystemMemoryBuffer::SetData");
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
  common::RaiseNotImplemented("render::low_level::opengl::SystemMemoryBuffer::GetData");
}

///��������� ������ � �������� OpenGL
void SystemMemoryBuffer::Bind ()
{
  common::RaiseNotImplemented("render::low_level::opengl::SystemMemoryBuffer::Bind");
}

///��������� �� ������ ������
void* GetDataPointer ()
{
  common::RaiseNotImplemented("render::low_level::opengl::SystemMemoryBuffer::GetDataPointer");
  return buffer;
}
