#ifndef RENDER_LOW_LEVEL_BUFFER_HEADER
#define RENDER_LOW_LEVEL_BUFFER_HEADER

#include <render/lowlevel/object.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum BufferUsageMode
{
  BufferUsageMode_Default,  //��� ������ ���������� � ������������ � �������������� ���������� ���������
  BufferUsageMode_Static,   //��������� ����� �� ����� ���� �������
  BufferUsageMode_Dynamic,  //����� ����� �������� ������ ����
  BufferUsageMode_Stream,   //����� ������ ��� ������ CPU<->GPU
  
  BufferUsageMode_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum BufferType
{
  BufferType_VertexBuffer, //��������� �����
  BufferType_IndexBuffer,  //��������� �����
  BufferType_Texture,      //��������
  BufferType_RenderTarget, //������� ����� ���������
  BufferType_DepthStencil, //����� �������/��������

  BufferType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum BufferAccessFlag
{
  BufferAccessFlag_Read  = 1, //������ �� ������
  BufferAccessFlag_Write = 2 //������ �� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct BufferDesc
{
  size_t          size;         //������ ������
  BufferUsageMode usage_mode;   //����� ������������� ������
  size_t          access_flags; //����� ������� � ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
class IBuffer: virtual public IDeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t GetSize () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual BufferType GetType () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GetDesc (BufferDesc&) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ / ������ �� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetData (size_t offset, size_t size, const void* data) = 0;
    virtual void GetData (size_t offset, size_t size, void* data) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ ����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void CopyTo   (IBuffer& destination, size_t offset, size_t size) = 0;
            void CopyFrom (IBuffer& source, size_t offset, size_t size);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���� ������������ �������
///  ������� ������� ������ ��� ������������ �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class IIndexBuffer:  virtual public IBuffer {};
class IVertexBuffer: virtual public IBuffer {};

#include <render/lowlevel/detail/buffer.inl>

}

}

#endif
