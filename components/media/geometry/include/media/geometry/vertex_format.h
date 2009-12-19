#ifndef MEDIALIB_GEOMETRY_VERTEX_FORMAT_HEADER
#define MEDIALIB_GEOMETRY_VERTEX_FORMAT_HEADER

#include <cstddef>
#include <common/singleton.h>

namespace media
{

namespace geometry
{

//////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum VertexAttributeSemantic
{
  VertexAttributeSemantic_Position,  //���������
  VertexAttributeSemantic_Normal,    //�������
  VertexAttributeSemantic_Color,     //����
  VertexAttributeSemantic_Tangent,   //�����������
  VertexAttributeSemantic_Binormal,  //���������
  VertexAttributeSemantic_TexCoord0, //������ ���������� ���������
  VertexAttributeSemantic_TexCoord1,
  VertexAttributeSemantic_TexCoord2,
  VertexAttributeSemantic_TexCoord3,
  VertexAttributeSemantic_TexCoord4,
  VertexAttributeSemantic_TexCoord5,
  VertexAttributeSemantic_TexCoord6,  
  VertexAttributeSemantic_TexCoord7,
  VertexAttributeSemantic_Influence, //������� ������� ��������� �����
  
  VertexAttributeSemantic_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum VertexAttributeType
{
  VertexAttributeType_Float2,    //������ �� 2-� ������������ �������
  VertexAttributeType_Float3,    //������ �� 3-� ������������ �������
  VertexAttributeType_Float4,    //������ �� 4-� ������������ �������
  VertexAttributeType_Short2,    //������ �� 2-� ������������� �������
  VertexAttributeType_Short3,    //������ �� 3-� ������������� ������� 
  VertexAttributeType_Short4,    //������ �� 4-� ������������� �������
  VertexAttributeType_UByte4,    //������ �� 4-� ����������� ����
  VertexAttributeType_Influence, //��. ��������� VertexInfluence
  
  VertexAttributeType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VertexAttribute
{
  VertexAttributeSemantic semantic; //���������
  VertexAttributeType     type;     //��� ���������
  size_t                  offset;   //�������� �� ������ �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class VertexFormat
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    VertexFormat  ();
    VertexFormat  (const VertexFormat&);
    ~VertexFormat ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    VertexFormat& operator = (const VertexFormat&);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t AttributesCount () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const VertexAttribute* Attributes () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const VertexAttribute& Attribute (size_t index) const;
          VertexAttribute& Attribute (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t AddAttribute  (VertexAttributeSemantic semantic, VertexAttributeType type, size_t offset); //return: ������ ���������� ��������
    size_t AddAttributes (const VertexFormat&); //return: ������ ���������� ������������ ��������

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveAttribute  (size_t position); //nothrow
    void RemoveAttribute  (VertexAttributeSemantic); //nothrow
    void RemoveAttributes (const VertexFormat&);
    void Clear            (); //�������� ���� ���������, nothrow

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    VertexFormat& operator += (const VertexFormat&);
    VertexFormat& operator -= (const VertexFormat&);
    
    VertexFormat operator + (const VertexFormat&) const;
    VertexFormat operator - (const VertexFormat&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const VertexAttribute* FindAttribute (VertexAttributeSemantic) const; //nothrow
          VertexAttribute* FindAttribute (VertexAttributeSemantic); //nothrow

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetMinimalVertexSize () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (VertexFormat&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const VertexFormat&) const;
    bool operator != (const VertexFormat&) const;

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (VertexFormat&, VertexFormat&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
const char* get_semantic_name    (VertexAttributeSemantic); //��� ���������
const char* get_type_name        (VertexAttributeType);     //��� ����
size_t      get_type_size        (VertexAttributeType);     //������ ���� �������� ������ � ������
size_t      get_components_count (VertexAttributeType);     //���������� �����������
bool        is_compatible        (VertexAttributeSemantic, VertexAttributeType); //�������� �������������

//��������� ��������� �� �����
VertexAttributeSemantic get_vertex_attribute_semantic (const char* name, VertexAttributeSemantic default_semantic=VertexAttributeSemantic_Num);

//��������� ���� �� �����
VertexAttributeType get_vertex_attribute_type (const char* name, VertexAttributeType default_type=VertexAttributeType_Num);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Vertex>
const VertexFormat& get_vertex_format ();

#include <media/geometry/detail/vertex_format.inl>

}

}

#endif
