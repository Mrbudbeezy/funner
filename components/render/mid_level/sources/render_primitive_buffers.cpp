#include "shared.h"

using namespace render;
using namespace render::mid_level;

/*
    �������� ���������� PrimitiveBuffers
*/

struct PrimitiveBuffers::Impl
{
};

/*
    ������������ / ���������� / ������������
*/

PrimitiveBuffers  (const DeviceManagerPtr&);
~PrimitiveBuffers ();

/*
    ���������� �������
*/

void Add (const media::geometry::VertexStream& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
void Add (const media::geometry::VertexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
void Add (const media::geometry::IndexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);

/*
    ���������� �������
*/

void Update (const media::geometry::VertexStream& buffer);
void Update (const media::geometry::VertexBuffer& buffer);
void Update (const media::geometry::IndexBuffer& buffer);

/*
    �������� �������
*/

void Remove (const media::geometry::VertexStream& buffer);
void Remove (const media::geometry::VertexBuffer& buffer);
void Remove (const media::geometry::IndexBuffer& buffer);
void Clear  ();

/*
    �������������� ��������������� ����������
*/

void   ReserveLines    (size_t count);
void   ReserveSprites  (size_t count);
size_t LinesCapacity   ();
size_t SpritesCapacity ();

/*
    ����� ������������� ������� ��������������� ����������
*/

void            SetLinesBufferUsage   (MeshBufferUsage usage);
void            SetSpritesBufferUsage (MeshBufferUsage usage);
MeshBufferUsage LinesBufferUsage      ();
MeshBufferUsage SpritesBufferUsage    ();
