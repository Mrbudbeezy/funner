#ifndef RENDER_MANAGER_PRIMITIVE_HEADER
#define RENDER_MANAGER_PRIMITIVE_HEADER

//TODO: �������� ��������� ����������� ����������!!!

#include <math/angle.h>

#include <render/manager/material.h>

namespace media
{

namespace geometry
{

//forward declarations
class VertexStream;
class VertexBuffer;
class IndexBuffer;
class Mesh;
class MeshLibrary;

}

}

namespace render
{

namespace manager
{

//implementation forwards
class PrimitiveBuffersImpl;
class PrimitiveImpl;
class Wrappers;

template <class T> class DynamicPrimitiveList;
template <class T> class DynamicPrimitiveListImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MeshBufferUsage
{
  MeshBufferUsage_Static,  //�� ����������� �����
  MeshBufferUsage_Dynamic, //����������� �����
  MeshBufferUsage_Stream,  //����� ����������� �����
  
  MeshBufferUsage_Default = MeshBufferUsage_Dynamic
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Sprite
{
  math::vec3f position;   //��������� ������ �������
  math::vec2f size;       //������ �������
  math::vec4f color;      //���� �������
  math::vec2f tex_offset; //�������� ������ ������� � �������� [0;1]
  math::vec2f tex_size;   //������ ������� � �������� [0;1]
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct BillboardSprite: public Sprite {};

struct OrientedSprite: public Sprite
{
  math::vec3f  normal;   //�������
  math::anglef rotation; //������� ������������ �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LinePoint
{
  math::vec3f position;   //��������� ����� � ������������
  math::vec4f color;      //���� �����
  math::vec2f tex_offset; //�������� ����� � �������� [0;1]
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Line
{
  LinePoint point [2]; //����� �����
};

typedef DynamicPrimitiveList<BillboardSprite> BillboardSpriteList;
typedef DynamicPrimitiveList<OrientedSprite>  OrientedSpriteList;
typedef DynamicPrimitiveList<Line>            LineList;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveBuffers
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PrimitiveBuffers  (const PrimitiveBuffers&);
    ~PrimitiveBuffers ();
    
    PrimitiveBuffers& operator = (const PrimitiveBuffers&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Add (const media::geometry::VertexStream& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
    void Add (const media::geometry::VertexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);    
    void Add (const media::geometry::IndexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void Update (const media::geometry::VertexStream& buffer);
    void Update (const media::geometry::IndexBuffer& buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////        
    void Remove    (const media::geometry::VertexStream& buffer);
    void Remove    (const media::geometry::VertexBuffer& buffer);    
    void Remove    (const media::geometry::IndexBuffer& buffer);
    void RemoveAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������� ��� ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   ReserveDynamicBuffers (size_t vertices_count, size_t indices_count);
    size_t DynamicVerticesCount  () const;
    size_t DynamicIndicesCount   () const;
      
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (PrimitiveBuffers&);
    
  protected:
    PrimitiveBuffers (PrimitiveBuffersImpl*);

  private:
    PrimitiveBuffersImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (PrimitiveBuffers&, PrimitiveBuffers&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���
///////////////////////////////////////////////////////////////////////////////////////////////////
class Primitive
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Primitive  (const Primitive&);
    ~Primitive ();

    Primitive& operator = (const Primitive&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name    () const;
    void        SetName (const char* id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PrimitiveBuffers Buffers () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t MeshesCount     () const;
    size_t AddMesh         (const media::geometry::Mesh&, MeshBufferUsage vb_usage = MeshBufferUsage_Default, MeshBufferUsage ib_usage = MeshBufferUsage_Default);
    void   RemoveMeshes    (size_t first_mesh, size_t meshes_count);
    void   RemoveAllMeshes ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t              SpriteListsCount                 () const;
    BillboardSpriteList AddStandaloneBillboardSpriteList (const math::vec3f& up, MeshBufferUsage vb_usage = MeshBufferUsage_Default,  MeshBufferUsage ib_usage = MeshBufferUsage_Default);
    OrientedSpriteList  AddStandaloneOrientedSpriteList  (const math::vec3f& up, MeshBufferUsage vb_usage = MeshBufferUsage_Default,  MeshBufferUsage ib_usage = MeshBufferUsage_Default);
    BillboardSpriteList AddBatchingBillboardSpriteList   (const math::vec3f& up);
    OrientedSpriteList  AddBatchingOrientedSpriteList    (const math::vec3f& up);
    void                RemoveSpriteList                 (OrientedSpriteList&);
    void                RemoveSpriteList                 (BillboardSpriteList&);
    void                RemoveAllSpriteLists             ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t   LineListsCount        () const;
    LineList AddStandaloneLineList (MeshBufferUsage vb_usage = MeshBufferUsage_Default,  MeshBufferUsage ib_usage = MeshBufferUsage_Default);
    LineList AddBatchingLineList   ();
    void     RemoveLineList        (LineList&);
    void     RemoveAllLineLists    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Primitive&);
    
  private:
    Primitive (PrimitiveImpl*);

  private:
    PrimitiveImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Primitive&, Primitive&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class DynamicPrimitiveList
{
  friend class Wrappers;
  friend class Primitive;
  public:
    typedef T Item;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DynamicPrimitiveList  (const DynamicPrimitiveList&);
    ~DynamicPrimitiveList ();

    DynamicPrimitiveList& operator = (const DynamicPrimitiveList&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetMaterial (const char* material);
    const char* Material    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Add    (size_t count, const Item* items);
    void   Update (size_t first, size_t count, const Item* items);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (size_t first, size_t count = 1);
    void Clear  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   Reserve  (size_t count);
    size_t Capacity () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �� ���������   
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveFromPrimitive ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (DynamicPrimitiveList&);  

  private:
    typedef DynamicPrimitiveListImpl<Item> Impl;

    DynamicPrimitiveList (Impl*);

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void swap (DynamicPrimitiveList<T>&, DynamicPrimitiveList<T>&);

}

}

#endif
