#ifndef RENDER_GL_DRIVER_INPUT_STAGE_SHARED_HEADER
#define RENDER_GL_DRIVER_INPUT_STAGE_SHARED_HEADER

#include <xtl/uninitialized_storage.h>

#include <common/exception.h>

#include <shared/input_stage.h>
#include <shared/context_object.h>
#include <shared/object.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Buffer: virtual public IBuffer, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Buffer(const ContextManager&, const BufferDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (BufferDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������  �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Bind () = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ������ ������ (��� ������� � ��������� ������) / �������� �� ������ ������ (��� ���������� �������)
///////////////////////////////////////////////////////////////////////////////////////////////////    
    virtual void* GetDataPointer () = 0;
 
  protected:
    BufferDesc  buffer_desc; // ���������� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� � ��������� � ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SystemMemoryBuffer: public Buffer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SystemMemoryBuffer  (const ContextManager&, const BufferDesc&);
    ~SystemMemoryBuffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t offset, size_t size, const void* data);
    void GetData (size_t offset, size_t size, void* data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void* GetDataPointer ();

  private:
    xtl::uninitialized_storage <char> buffer;     //����� � ��������� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� � ��������� � ����������� � �������������� ���������� GL_ARB_vertex_buffer_object
///////////////////////////////////////////////////////////////////////////////////////////////////
class VboBuffer: public Buffer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    VboBuffer  (const ContextManager&, GLenum target, const BufferDesc&);
    ~VboBuffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t offset, size_t size, const void* data);
    void GetData (size_t offset, size_t size, void* data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///C������� �� ������ ������ (��� ���������� �������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* GetDataPointer ();

  private:
    GLenum target;     //������� ��� ����������� ������ (GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER)
    GLuint buffer_id;  //����� ������ � ��������� OpenGL
};

}

}

}

#endif
