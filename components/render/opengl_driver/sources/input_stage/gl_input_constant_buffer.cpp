#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    �����������
*/

ConstantBuffer::ConstantBuffer (const ContextManager& context_manager, const BufferDesc& in_desc)
  : SystemMemoryBuffer (context_manager, in_desc)
  {}

/*
    ��������� ������ ����� ���������
*/

void ConstantBuffer::SetDataCore (size_t offset, size_t size, const void* data)
{
  SystemMemoryBuffer::SetDataCore (offset, size, data);

  StageRebindNotify (Stage_Shading);
}
