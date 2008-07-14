#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    �����������
*/

SystemMemoryInputBuffer::SystemMemoryInputBuffer (const ContextManager& context_manager, GLenum in_target, const BufferDesc& in_desc)
  : SystemMemoryBuffer (context_manager, in_desc),
    target (in_target)
  {}

/*
    ��������� ������ � �������� OpenGL
*/

void SystemMemoryInputBuffer::Bind ()
{
  size_t& current_id = GetContextDataTable (Stage_Input)[InputStageCache_BindedVboBuffer];

  if (current_id == GetId ())
    return;

  MakeContextCurrent ();
  
  const ContextCaps& caps = GetCaps ();

  if (caps.has_arb_vertex_buffer_object)
  {
    caps.glBindBuffer_fn (target, 0);

    CheckErrors ("render::low_level::opengl::SystemMemoryInputBuffer::Bind");
  }

    //��������� ���-����������

  current_id = GetId ();  
}

/*
    ��������� ������ ����� ���������
*/

void SystemMemoryInputBuffer::SetDataCore (size_t offset, size_t size, const void* data)
{
  SystemMemoryBuffer::SetDataCore (offset, size, data);

  StageRebindNotify (Stage_Input);
}
