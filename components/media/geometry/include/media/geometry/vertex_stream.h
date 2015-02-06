#ifndef MEDIALIB_GEOMETRY_VERTEX_STREAM_HEADER
#define MEDIALIB_GEOMETRY_VERTEX_STREAM_HEADER

#include <stdint.h>

#include <media/geometry/defs.h>
#include <media/geometry/vertex_format.h>
#include <xtl/intrusive_ptr.h>

namespace media
{

namespace geometry
{

//forward declarations
class VertexBuffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class VertexDeclaration
{
  public:
    VertexDeclaration (const VertexFormat&);
    VertexDeclaration (const VertexFormat&, uint32_t vertex_size);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const VertexFormat& Format     () const;
    uint32_t            VertexSize () const;

  private:
    const VertexFormat* format;
    uint32_t            vertex_size;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Vertex> VertexDeclaration make_vertex_declaration ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class VertexStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    VertexStream  (const VertexDeclaration& declaration);
    VertexStream  (uint32_t vertices_count, const VertexDeclaration& declaration);
    VertexStream  (const VertexBuffer& source);
    VertexStream  (const VertexStream& source, const VertexDeclaration& declaration);
    VertexStream  (const VertexBuffer& source, const VertexDeclaration& declaration);
    VertexStream  (const VertexStream&);
    ~VertexStream ();
    
    template <class Vertex> VertexStream (uint32_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    VertexStream& operator = (const VertexStream&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    VertexStream Clone () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ������ (����������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const VertexFormat& Format () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t VertexSize () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void* Data () const;
          void* Data ();
          
    template <class Vertex> const Vertex* Data () const;
    template <class Vertex>       Vertex* Data ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t Size   () const;
    void     Resize (uint32_t vertices_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t Capacity () const;
    void     Reserve  (uint32_t vertices_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (VertexStream&);

  private:
    void Convert (const VertexStream&);
    void Convert (const VertexBuffer&);

  private:
    struct Impl;
    
    VertexStream (Impl*);
    
  private:
    xtl::intrusive_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (VertexStream&, VertexStream&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class VertexWeightStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    VertexWeightStream  ();
    VertexWeightStream  (uint32_t weights_count);
    VertexWeightStream  (const VertexWeightStream&);
    ~VertexWeightStream ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    VertexWeightStream& operator = (const VertexWeightStream&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    VertexWeightStream Clone () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ������ (����������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const VertexWeight* Data () const;
          VertexWeight* Data ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t Size   () const;
    void     Resize (uint32_t weights_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t Capacity () const;
    void     Reserve  (uint32_t weights_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (VertexWeightStream&);

  private:
    struct Impl;
    
    VertexWeightStream (Impl*);
    
  private:
    xtl::intrusive_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (VertexWeightStream&, VertexWeightStream&);

#include <media/geometry/detail/vertex_stream.inl>

}

}

#endif
