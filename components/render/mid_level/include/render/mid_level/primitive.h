#ifndef RENDER_MID_LEVEL_PRIMITIVE_HEADER
#define RENDER_MID_LEVEL_PRIMITIVE_HEADER

#include <render/mid_level/material.h>

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

namespace mid_level
{

//implementation forwards
class PrimitiveImpl;
class Wrappers;

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
  float     width;     //������ �����
};

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
///������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t MeshesCount     () const;
    size_t AddMesh         (const media::geometry::Mesh&, MeshBufferUsage vb_usage = MeshBufferUsage_Default, MeshBufferUsage ib_usage = MeshBufferUsage_Default);
    void   UpdateMesh      (size_t mesh_index, const media::geometry::Mesh& mesh);
    void   RemoveMeshes    (size_t first_mesh, size_t meshes_count);
    void   RemoveAllMeshes ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������������� �������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddMeshBuffer        (const media::geometry::VertexStream& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
    void AddMeshBuffer        (const media::geometry::VertexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
    void AddMeshBuffer        (const media::geometry::IndexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
    void UpdateMeshBuffer     (const media::geometry::VertexStream& buffer);
    void UpdateMeshBuffer     (const media::geometry::VertexBuffer& buffer);
    void UpdateMeshBuffer     (const media::geometry::IndexBuffer& buffer);
    void RemoveMeshBuffer     (const media::geometry::VertexStream& buffer);
    void RemoveMeshBuffer     (const media::geometry::VertexBuffer& buffer);
    void RemoveMeshBuffer     (const media::geometry::IndexBuffer& buffer);
    void RemoveAllMeshBuffers ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t LinesCount       () const;
    size_t AddLines         (size_t lines_count, const Line* Lines, const Material& material);
    void   UpdateLines      (size_t first_lines, size_t lines_count, const Line* Lines);
    void   SetLinesMaterial (size_t first_lines, size_t lines_count, const Material& material);
    void   RemoveLines      (size_t first_lines, size_t lines_count);
    void   RemoveAllLines   ();
    void   ReserveLines     (size_t lines_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t SpritesCount       () const;
    size_t AddSprites         (size_t sprites_count, const Sprite* sprites, const Material& material);
    void   UpdateSprites      (size_t first_sprite, size_t sprites_count, const Sprite* sprites);
    void   SetSpritesMaterial (size_t first_sprite, size_t sprites_count, const Material& material);
    void   RemoveSprites      (size_t first_sprite, size_t sprites_count);
    void   RemoveAllSprites   ();
    void   ReserveSprites     (size_t sprites_count);

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

}

}

#endif
