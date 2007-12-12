#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///����������� / ����������
VboBuffer::VboBuffer (const ContextManager& context_manager, GLenum in_target, const BufferDesc& desc)
  : Buffer (desc), ContextObject (context_manager), target (in_target)
{
}

VboBuffer::~VboBuffer ()
{
}

///������ � ������� ������
void VboBuffer::SetData (size_t offset, size_t size, const void* data)
{
  RaiseNotImplemented ("render::low_level::opengl::VboBuffer::SetData");
}

void VboBuffer::GetData (size_t offset, size_t size, void* data)
{
  RaiseNotImplemented ("render::low_level::opengl::VboBuffer::GetData");
}

///��������� ������ � �������� OpenGL
void VboBuffer::Bind ()
{
  RaiseNotImplemented ("render::low_level::opengl::VboBuffer::Bind");
}

///C������� �� ������ ������ (��� ���������� �������)
void* VboBuffer::GetDataPointer ()
{
  return 0; //������������ �������� �� ������ ������
}
