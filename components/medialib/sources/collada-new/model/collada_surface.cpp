#include <media/collada/geometry.h>

#include <stl/string>
#include <stl/vector>

using namespace medialib::collada;
using namespace common;

namespace
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class ChannelListImpl: public Surface::IChannelList<T>
{
  enum { DEFAULT_CHANNELS_RESERVE = 8 };
  public:
    ChannelListImpl (size_t in_vertices_count) : vertices_count (in_vertices_count)
    {
      channels.reserve (DEFAULT_CHANNELS_RESERVE);
    }
    
    ~ChannelListImpl ()
    {
      Clear ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� / �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////      
    size_t Size    () const { return channels.size (); }
    bool   IsEmpty () const { return channels.empty (); }
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Create (const char* name)
    {
      if (!name)
        RaiseNullArgument ("medialib::collada::Surface::IChannelList::Create", "name");
        
      Channel* channel = new Channel (vertices_count, name);
      
      try
      {    
        channels.push_back (channel);
      
        return channels.size () - 1;
      }
      catch (...)
      {
        delete channel;
        throw;
      }
    }
    
    void Remove (size_t channel)
    {
      if (channel >= channels.size ())
        RaiseOutOfRange ("medialib::collada::Surface::IChannelList::Remove", "channel", channel, channels.size ());
        
      delete channels [channel];
      
      channels.erase (channels.begin () + channel);
    }
    
    void Clear ()
    {
      for (ChannelArray::iterator i=channels.begin (); i!=channels.end (); ++i)
        delete *i;
        
      channels.clear ();      
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name (size_t channel) const
    {
      if (channel >= channels.size ())
        RaiseOutOfRange ("medialib::collada::Surface::IChannelList::Name", "channel", channel, channels.size ());
        
      return channels [channel]->name.c_str ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ �� �����. ���������� ����� ������, ��� -1 � ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    int Find (const char* name) const
    {
      if (!name)
        RaiseNullArgument ("media::collada::Surface::IChannelList::Find", "name");
        
      for (ChannelArray::const_iterator i=channels.begin (), end=channels.end (); i!=end; ++i)
        if ((*i)->name == name)
          return i - channels.begin ();

      return -1;
    }
                
///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const T* Data (size_t channel) const
    {
      if (channel >= channels.size ())
        RaiseOutOfRange ("medialib::collada::Surface::IChannelList::Data", "channel", channel, channels.size ());
        
      return channels [channel]->data;
    }

    T* Data (size_t channel)
    {
      return const_cast<T*> (const_cast<const ChannelListImpl&> (*this).Data (channel));
    }
  
  private:
    struct Channel
    {
      T*          data;
      stl::string name;
      
      Channel  (size_t verts_count, const char* in_name) :
        data ((T*)::operator new (sizeof (T) * verts_count)), name (in_name) {}

      ~Channel () { ::operator delete (data); }
    };
    
    typedef stl::vector<Channel*> ChannelArray;
    
   private:
     ChannelArray channels;
     size_t       vertices_count;
};

typedef ChannelListImpl<TexVertex>       TexVertexChannelListImpl;
typedef ChannelListImpl<math::vec3f>     ColorChannelListImpl;
typedef ChannelListImpl<VertexInfluence> InfluenceChannelListImpl;

}

/*
    ���������� surface
*/


struct Surface::Impl
{
  stl::string               material_name;      //��� ���������
  collada::PrimitiveType    primitive_type;     //��� ����������
  size_t                    vertices_count;     //���������� ������
  size_t                    indices_count;      //���������� ��������
  Vertex*                   vertices;           //�������
  size_t*                   indices;            //�������
  ColorChannelListImpl      color_channels;     //������ ��������� ������
  TexVertexChannelListImpl  texvertex_channels; //������ ���������������
  InfluenceChannelListImpl  influence_channels; //������ ��������� �����
  
  Impl (collada::PrimitiveType in_primitive_type, size_t in_vertices_count, size_t in_indices_count) :
    vertices_count (in_vertices_count), indices_count (in_indices_count), primitive_type (in_primitive_type),
    color_channels (in_vertices_count), texvertex_channels (in_vertices_count), influence_channels (in_vertices_count)
  {
    switch (primitive_type)
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
    ::operator delete (vertices);
    ::operator delete (indices);
  }
};

/*
    ����������� / ����������
*/

Surface::Surface (medialib::collada::PrimitiveType type, size_t verts_count, size_t indices_count)
  : impl (new Impl (type, verts_count, indices_count))
  {}

Surface::~Surface ()
{
  delete impl;
}

/*
    ��� ��������� �����������
*/

const char* Surface::MaterialName () const
{
  return impl->material_name.c_str ();
}

void Surface::SetMaterialName (const char* name)
{
  if (!name)
    RaiseNullArgument ("medialib::collada::Surface::SetMaterialName", "name");
    
  impl->material_name = name;
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

/*
    ������ � �������� ��������� ������
*/

Surface::ColorChannelList& Surface::ColorChannels ()
{
  return impl->color_channels;
}

const Surface::ColorChannelList& Surface::ColorChannels () const
{
  return impl->color_channels;
}

/*
    ������ � �������� ���������������� ������
*/

Surface::TexVertexChannelList& Surface::TexVertexChannels ()
{
  return impl->texvertex_channels;
}

const Surface::TexVertexChannelList& Surface::TexVertexChannels () const
{
  return impl->texvertex_channels;
}

/*
    ������ � �������� ��������� �����
*/

Surface::InfluenceChannelList& Surface::InfluenceChannels ()
{
  return impl->influence_channels;
}

const Surface::InfluenceChannelList& Surface::InfluenceChannels () const
{
  return impl->influence_channels;
}
