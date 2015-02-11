#ifndef MEDIALIB_COLLADA_GEOMETRY_HEADER
#define MEDIALIB_COLLADA_GEOMETRY_HEADER

#include <media/collada/collection.h>
#include <math/vector.h>
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
  unsigned int first_weight;  //������ ������� ���� ���������� ������������ �� �������
  unsigned int weights_count; //���������� ����� ���������� ����������� �� �������
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
    template <class T, class KeyT> class IChannelList
    {
      public:
        typedef KeyT Key;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////      
        virtual unsigned int Size    () const = 0;
        virtual bool         IsEmpty () const = 0;
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
        virtual unsigned int Create (Key name) = 0;
        virtual void         Remove (unsigned int channel) = 0;
        virtual void         Clear  () = 0;
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Key Name (unsigned int channel) const = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ �� �����. ���������� ����� ������, ��� -1 � ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int Find (Key name) const = 0;
                
///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
        virtual       T* Data (unsigned int channel) = 0;
        virtual const T* Data (unsigned int channel) const = 0;

      protected:
        virtual ~IChannelList () {}
    };
    
    typedef IChannelList<TexVertex, unsigned int>      TexVertexChannelList;
    typedef IChannelList<math::vec3f, unsigned int>    ColorChannelList;
    typedef IChannelList<VertexInfluence, const char*> InfluenceChannelList;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Surface  (collada::PrimitiveType type, unsigned int verts_count, unsigned int indices_count);
    Surface  (const Surface&);
    ~Surface ();

    Surface& operator = (const Surface&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Surface Clone () const;

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
    unsigned int VerticesCount () const;
    unsigned int IndicesCount  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
          Vertex* Vertices ();
    const Vertex* Vertices () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
          unsigned int* Indices ();
    const unsigned int* Indices () const;

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
    
    Surface (Impl*);
    
  private:
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
    Mesh  (const Mesh&);
    ~Mesh ();

    Mesh& operator = (const Mesh&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Mesh Clone () const;

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
    
    Mesh (Impl*);
    
  private:
    xtl::intrusive_ptr<Impl> impl;
};

}

}

#endif
