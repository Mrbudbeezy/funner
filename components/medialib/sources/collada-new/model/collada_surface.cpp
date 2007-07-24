#include <media/collada/geometry.h>

#include <stl/string>
#include <stl/vector>

using namespace medialib::collada;
using namespace common;

/*
    ���������� surface
*/

namespace
{

//����� ��������������� ������
struct TexVertexChannel
{
  TexVertex*  verts; //���������������� �������
  stl::string name;  //��� ������ ���������������
  
  TexVertexChannel  (size_t verts_count, const char* in_name) :
    verts ((TexVertex*)::operator new (sizeof (TexVertex) * verts_count)), name (in_name) {}

  ~TexVertexChannel () { ::operator delete (verts); }
};

typedef stl::vector<TexVertexChannel*> TexVertexChannelArray;

}

struct Surface::Impl
{
  collada::Material&      material;           //��������
  collada::PrimitiveType  primitive_type;     //��� ����������
  size_t                  vertices_count;     //���������� ������
  size_t                  indices_count;      //���������� ��������
  Vertex*                 vertices;           //�������
  size_t*                 indices;            //�������
  math::vec4f*            colors;             //��������� �����
  TexVertexChannelArray   texvertex_channels; //������ ���������������
  
  enum { DEFAULT_TEXVERTEX_CHANNELS_RESERVE = 8 }; //������������� ���������� ������� ���������������
  
  void RemoveAllTextureChannels ()
  {
    for (TexVertexChannelArray::iterator i=texvertex_channels.begin (); i!=texvertex_channels.end (); ++i)
      delete *i;    
      
    texvertex_channels.clear ();
  }
  
  Impl (collada::Material& in_material, collada::PrimitiveType in_primitive_type, size_t in_vertices_count, size_t in_indices_count) : 
    material (in_material), vertices_count (in_vertices_count), indices_count (in_indices_count), colors (0)    
  {
    texvertex_channels.reserve (DEFAULT_TEXVERTEX_CHANNELS_RESERVE);
    
    switch (in_primitive_type)
    {
      case PrimitiveType_LineList:
      case PrimitiveType_LineStrip:
      case PrimitiveType_TriangleList:
      case PrimitiveType_TriangleStrip:
      case PrimitiveType_TriangleFan:
        break;
      default:
        RaiseInvalidArgument ("medialib::collada::Surface::Surface", "primitive_type", in_primitive_type);
        break;
    }
    
    vertices = (Vertex*)::operator new (vertices_count * sizeof (Vertex));
    
    try
    {
      indices  = (size_t*)::operator new (indices_count * sizeof (size_t));
    }
    catch (...)
    {
      ::operator delete (vertices);
      throw;
    }
  }
  
  ~Impl ()
  {
    RemoveAllTextureChannels ();

    ::operator delete (vertices);
    ::operator delete (indices);
    
    if (colors)
      ::operator delete (colors);
  }
};

/*
    ����������� / ����������
*/

Surface::Surface (medialib::collada::Material& material, medialib::collada::PrimitiveType type, size_t verts_count, size_t indices_count)
  : impl (new Impl (material, type, verts_count, indices_count))
  {}

Surface::~Surface ()
{
  delete impl;
}

/*
    �������� �����������
*/

Material& Surface::Material ()
{
  return impl->material;
}

const Material& Surface::Material () const
{
  return impl->material;
}

/*
    ��� ����������
*/

PrimitiveType Surface::PrimitiveType () const
{
  return impl->primitive_type;
}

/*
    ���������� ������ � ��������
*/

size_t Surface::VerticesCount () const
{
  return impl->vertices_count;
}

size_t Surface::IndicesCount () const
{
  return impl->indices_count;
}

/*
    ������ � ���������
*/

Vertex* Surface::Vertices ()
{
  return impl->vertices;
}

const Vertex* Surface::Vertices () const
{
  return impl->vertices;
}

/*
    ������ � ���������� �������
*/

bool Surface::HasVertexColors () const
{
  return impl->colors != 0;
}

void Surface::CreateVertexColors ()
{
  if (impl->colors)
    return;
    
  impl->colors = (math::vec4f*)::operator new (sizeof (math::vec4f) * impl->vertices_count);    
}

void Surface::RemoveVertexColors ()
{
  if (!impl->colors)
    return;
    
  ::operator delete (impl->colors);
  
  impl->colors = 0;
}

math::vec4f* Surface::VertexColors ()
{
  if (!impl->colors)
    RaiseNotSupported ("medialib::collada::Surface::VertexColors", "There is no vertex colors in this surface."
                       " Use Surface::CreateVertexColors first");

  return impl->colors;
}

const math::vec4f* Surface::VertexColors () const
{
  if (!impl->colors)
    RaiseNotSupported ("medialib::collada::Surface::VertexColors", "There is no vertex colors in this surface."
                       " Use Surface::CreateVertexColors first");

  return impl->colors;
}

/*
    ������ � ����������������� ���������
*/

size_t Surface::TextureChannelsCount () const
{
  return impl->texvertex_channels.size ();
}

size_t Surface::CreateTextureChannel (const char* name)
{
  if (!name)
    name = "";
    
  TexVertexChannel* channel = new TexVertexChannel (impl->vertices_count, name);
  
  try
  {    
    impl->texvertex_channels.push_back (channel);
  
    return impl->texvertex_channels.size () - 1;
  }
  catch (...)
  {
    delete channel;
    throw;
  }
}

void Surface::RemoveTextureChannel (size_t channel)
{
  if (channel >= impl->texvertex_channels.size ())
    RaiseOutOfRange ("medialib::collada::Surface::RemoveTextureChannel", "channel", channel, impl->texvertex_channels.size ());
    
  delete impl->texvertex_channels [channel];
  
  impl->texvertex_channels.erase (impl->texvertex_channels.begin () + channel);
}

void Surface::RemoveAllTextureChannels ()
{
  impl->RemoveAllTextureChannels ();
}

const char* Surface::TextureChannelName (size_t channel) const
{
  if (channel >= impl->texvertex_channels.size ())
    RaiseOutOfRange ("medialib::collada::Surface::TextureChannelName", "channel", channel, impl->texvertex_channels.size ());
    
  return impl->texvertex_channels [channel]->name.c_str ();
}

bool Surface::HasTextureChannel (size_t channel) const
{
  return channel < impl->texvertex_channels.size ();
}

int Surface::FindTextureChannel (const char* name) const
{
  if (!name)
    RaiseNullArgument ("media::collada::Surface::FindTextureChannel", "name");
    
  for (TexVertexChannelArray::const_iterator i=impl->texvertex_channels.begin (), end=impl->texvertex_channels.end (); i!=end; ++i)
    if ((*i)->name == name)
      return i - impl->texvertex_channels.begin ();
      
  return -1;
}

const TexVertex* Surface::TextureVertices (size_t channel) const
{
  if (channel >= impl->texvertex_channels.size ())
    RaiseOutOfRange ("medialib::collada::Surface::TextureVertices", "channel", channel, impl->texvertex_channels.size ());
    
  return impl->texvertex_channels [channel]->verts;
}

TexVertex* Surface::TextureVertices (size_t channel)
{
  if (channel >= impl->texvertex_channels.size ())
    RaiseOutOfRange ("medialib::collada::Surface::TextureVertices", "channel", channel, impl->texvertex_channels.size ());
    
  return impl->texvertex_channels [channel]->verts;
}

/*
    ������� ����������
*/

size_t* Surface::Indices ()
{
  return impl->indices;
}

const size_t* Surface::Indices () const
{
  return impl->indices;
}
