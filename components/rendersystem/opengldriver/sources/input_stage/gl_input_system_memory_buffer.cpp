#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

SystemMemoryBuffer::SystemMemoryBuffer (const BufferDesc& desc)
  : Buffer (desc)
{
  buffer = NULL;
}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
}

///������ � ������� ������
void SystemMemoryBuffer::SetData (size_t offset, size_t size, const void* data)
{
  RaiseNotImplemented ("render::low_level::opengl::SystemMemoryBuffer::SetData");
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
  RaiseNotImplemented ("render::low_level::opengl::SystemMemoryBuffer::GetData");
}

///��������� ������ � �������� OpenGL
void SystemMemoryBuffer::Bind ()
{
  //��������� �� ���������
}

///��������� �� ������ ������
void* SystemMemoryBuffer::GetDataPointer ()
{
  return buffer;
}
