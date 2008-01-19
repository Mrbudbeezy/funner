#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    �����������
*/

NullFrameBuffer::NullFrameBuffer (const ContextManager& manager)
  : ContextObject (manager)
  {}

/*
    ��������� ������ � �������� OpenGL
*/

void NullFrameBuffer::Bind (bool& color_buffer_state, bool& depth_stencil_buffer_state)
{
    //����� �������� ���������

  MakeContextCurrent ();

    //��������� ������ ����� �� ���������

  static Extension EXT_framebuffer_object = "GL_EXT_framebuffer_object";

  if (IsSupported (EXT_framebuffer_object))
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

    //���������� ������� ��������� � ������

  glDrawBuffer (GL_NONE);
  glReadBuffer (GL_NONE);

  color_buffer_state         = false;
  depth_stencil_buffer_state = false;
}
