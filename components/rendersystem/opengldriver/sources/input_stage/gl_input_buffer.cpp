#include "shared.h"

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
