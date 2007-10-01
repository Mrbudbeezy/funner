#ifndef MEDIALIB_COLLADA_GEOMETRY_HEADER
#define MEDIALIB_COLLADA_GEOMETRY_HEADER

#include <media/collada/collection.h>
#include <media/clone.h>
#include <math/mathlib.h>
#include <xtl/intrusive_ptr.h>

namespace media
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Vertex
{
  math::vec3f coord;    //���������� �������
  math::vec3f normal;   //������� � �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TexVertex
{
  math::vec3f coord;    //���������� ���������� �������
  math::vec3f tangent;  //����������� � texture space
  math::vec3f binormal; //��������� � texture space
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ����������� ����������� �� ������� (������������ ��������� � Skin)
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VertexInfluence
{
  size_t first_weight;  //������ ������� ���� ���������� ������������ �� �������
  size_t weights_count; //���������� ����� ���������� ����������� �� �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PrimitiveType
{
  PrimitiveType_LineList,      //������ ��������
  PrimitiveType_LineStrip,     //������� ��������
  PrimitiveType_TriangleList,  //������ �������������
  PrimitiveType_TriangleStrip, //������� �������������
  PrimitiveType_TriangleFan    //����� �������������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Surface
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������ �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> class IChannelList
    {
      public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////      
        virtual size_t Size    () const = 0;
        virtual bool   IsEmpty () const = 0;
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
        virtual size_t Create (const char* name) = 0;        
        virtual void   Remove (size_t channel) = 0;
        virtual void   Clear  () = 0;
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const char* Name (size_t channel) const = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ �� �����. ���������� ����� ������, ��� -1 � ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int Find (const char* name) const = 0;
                
///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
        virtual       T* Data (size_t channel) = 0;
        virtual const T* Data (size_t channel) const = 0;

      protected:
        virtual ~IChannelList () {}
    };
    
    typedef IChannelList<TexVertex>       TexVertexChannelList;
    typedef IChannelList<math::vec3f>     ColorChannelList;
    typedef IChannelList<VertexInfluence> InfluenceChannelList;   
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Surface  (collada::PrimitiveType type, size_t verts_count, size_t indices_count);
    Surface  (const Surface&, media::CloneMode mode = media::CloneMode_Instance);
    ~Surface ();

    Surface& operator = (const Surface&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������� ����������� (������ ��� ������ ������������ � MaterialBinds::FindMaterial)
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Material    () const;
    void        SetMaterial (const char* id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    collada::PrimitiveType PrimitiveType () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t VerticesCount () const;
    size_t IndicesCount  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
          Vertex* Vertices ();
    const Vertex* Vertices () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
          size_t* Indices ();
    const size_t* Indices () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
          ColorChannelList& ColorChannels ();
    const ColorChannelList& ColorChannels () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� ���������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
          TexVertexChannelList& TexVertexChannels ();
    const TexVertexChannelList& TexVertexChannels () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
          InfluenceChannelList& InfluenceChannels ();
    const InfluenceChannelList& InfluenceChannels () const;

  private:
    struct Impl;
    xtl::intrusive_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���
///////////////////////////////////////////////////////////////////////////////////////////////////
class Mesh
{
  public:
    typedef ICollection<Surface> SurfaceList;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Mesh  ();
    Mesh  (const Mesh&, media::CloneMode mode = CloneMode_Instance);
    ~Mesh ();

    Mesh& operator = (const Mesh&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Id    () const;
    void        SetId (const char* id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
          SurfaceList& Surfaces ();
    const SurfaceList& Surfaces () const;

  private:
    struct Impl;
    xtl::intrusive_ptr<Impl> impl;
};

}

}

#endif
