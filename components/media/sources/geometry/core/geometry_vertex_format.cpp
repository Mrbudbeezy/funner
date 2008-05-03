#include "shared.h"

using namespace media::geometry;
using namespace common;

/*
    �������� ���������� ������� ������
*/

struct VertexFormat::Impl
{
  VertexAttribute attributes [VertexAttributeSemantic_Num]; //�������� �������
  size_t          attributes_count;                         //���������� ���������
  size_t          attributes_hash;                          //��� �� ������� ���������
  bool            need_hash_update;                         //��������� �������� ���� ���������
  size_t          min_vertex_size;                          //����������� ������ �������
  bool            need_vertex_size_update;                  //��������� �������� ������� �������
  
  size_t Hash ();
  
  Impl () : attributes_count (0), need_hash_update (true), need_vertex_size_update (true) {}
};

/*
    ����������� / ����������
*/

VertexFormat::VertexFormat ()
  : impl (new Impl)
  {}

VertexFormat::VertexFormat (const VertexFormat& vf)
  : impl (new Impl (*vf.impl))
  {}

VertexFormat::~VertexFormat ()
{
  delete impl;
}

/*
    ������������
*/

VertexFormat& VertexFormat::operator = (const VertexFormat& vf)
{
  VertexFormat (vf).Swap (*this);
  
  return *this;
}
  
/*
    ���������� ���������
*/

size_t VertexFormat::AttributesCount () const
{
  return impl->attributes_count;
}

/*
    ��������� ������� ���������
*/

const VertexAttribute* VertexFormat::Attributes () const
{
  return impl->attributes;
}

/*
    ��������� ��������
*/

const VertexAttribute& VertexFormat::Attribute (size_t index) const
{
  if (index >= impl->attributes_count)
    RaiseOutOfRange ("media::geometry::VertexFormat::Attribute", "index", index, impl->attributes_count);
    
  return impl->attributes [index];
}

VertexAttribute& VertexFormat::Attribute (size_t index)
{
  return const_cast<VertexAttribute&> (const_cast<const VertexFormat&> (*this).Attribute (index));
}

/*
    ����� �������� �� ���������
*/

const VertexAttribute* VertexFormat::FindAttribute (VertexAttributeSemantic semantic) const
{
  for (VertexAttribute* attribute=impl->attributes, *end=attribute+impl->attributes_count; attribute != end; attribute++)
    if (attribute->semantic == semantic)
      return attribute;
      
  return 0;
}

VertexAttribute* VertexFormat::FindAttribute (VertexAttributeSemantic semantic)
{
  return const_cast<VertexAttribute*> (const_cast<const VertexFormat&> (*this).FindAttribute (semantic));
}

/*
    ���������� ���������
*/

size_t VertexFormat::AddAttribute (VertexAttributeSemantic semantic, VertexAttributeType type, size_t offset)
{
  if (semantic < 0 || semantic >= VertexAttributeSemantic_Num)
    RaiseInvalidArgument ("media::geometry::VertexFormat::AddAttribute", "semantic", semantic);
    
  if (type < 0 || type >= VertexAttributeType_Num)
    RaiseInvalidArgument ("media::geometry::VertexFormat::AddAttribute", "type", type);
    
    //�������� ������������� ���� �������� � ��� ����������
    
  if (!is_compatible (semantic, type))
    RaiseNotSupported ("media::geometry::VertexFormat::AddAttribute", "Semantic '%s' is not compatible with type '%s'",
                       get_semantic_name (semantic), get_type_name (type));  
  
    //�������� ������� �������� � �������

  if (FindAttribute (semantic))
    RaiseNotSupported ("media::geometry::VertexFormat::AddAttribute", "Attribute with semantic '%s' has been already inserted",
                       get_semantic_name (semantic));
    
    //���������� ��������
     
  VertexAttribute& attribute = impl->attributes [impl->attributes_count];
  
  attribute.semantic = semantic;
  attribute.type     = type;
  attribute.offset   = offset;

  impl->attributes_count++;
  
  impl->need_hash_update        = true;
  impl->need_vertex_size_update = true;
  
  return impl->attributes_count - 1;
}

size_t VertexFormat::AddAttributes (const VertexFormat& format)
{
  for (size_t i=0, count=format.impl->attributes_count; i<count; i++)
    if (FindAttribute (format.impl->attributes [i].semantic))
      RaiseInvalidArgument ("media::geometry::VertexFormat::AddAttributes", "semantic",
                            format.impl->attributes [i].semantic, "Attribute has been already inserted");
                            
  for (size_t i=0, count=format.impl->attributes_count; i<count; i++)
  {
    const VertexAttribute& attribute = format.impl->attributes [i];
    
    AddAttribute (attribute.semantic, attribute.type, attribute.offset);
  }

  return impl->attributes_count - 1;
}

/*
    �������� ���������
*/

void VertexFormat::RemoveAttribute (size_t position)
{
  if (position >= impl->attributes_count)
    return;
    
  memcpy (impl->attributes + position, impl->attributes + position + 1, sizeof (VertexAttribute) * (impl->attributes_count - position - 1));
  
  impl->attributes_count--;
  
  impl->need_hash_update        = true;
  impl->need_vertex_size_update = true;
}

void VertexFormat::RemoveAttribute (VertexAttributeSemantic semantic)
{
  VertexAttribute* attribute = FindAttribute (semantic);
  
  if (!attribute)
    return;
    
  RemoveAttribute (attribute - impl->attributes);
}

void VertexFormat::RemoveAttributes (const VertexFormat& format)
{
  for (size_t i=0, count=format.impl->attributes_count; i<count; i++)
    RemoveAttribute (format.impl->attributes [i].semantic);
}

void VertexFormat::Clear ()
{
  impl->attributes_count = 0;
}

/*
    ���������� ����������
*/

VertexFormat& VertexFormat::operator += (const VertexFormat& format)
{
  AddAttributes (format);
  return *this;
}

VertexFormat& VertexFormat::operator -= (const VertexFormat& format)
{
  RemoveAttributes (format);
  return *this;
}

VertexFormat VertexFormat::operator + (const VertexFormat& format) const
{
  return VertexFormat (*this) += format;
}

VertexFormat VertexFormat::operator - (const VertexFormat& format) const
{
  return VertexFormat (*this) -= format;
}

/*
    ������ ������� �������
*/

size_t VertexFormat::GetMinimalVertexSize () const
{
  if (!impl->need_vertex_size_update)
    return impl->min_vertex_size;

  if (impl->attributes_count)
  {
    size_t max_offset = 0;
    
    for (VertexAttribute* attribute=impl->attributes, *end=attribute+impl->attributes_count; attribute != end; attribute++)
    {
      size_t offset = attribute->offset + get_type_size (attribute->type);

      if (offset > max_offset)
        max_offset = offset;
    }
        
    impl->min_vertex_size = max_offset;
  }
  else impl->min_vertex_size = 0;

  impl->need_vertex_size_update = false;

  return impl->min_vertex_size;
}

/*
    �����
*/

void VertexFormat::Swap (VertexFormat& vf)
{
  Impl* tmp = impl;
  impl      = vf.impl;
  vf.impl   = tmp;
}

namespace media
{

namespace geometry
{

void swap (VertexFormat& vf1, VertexFormat& vf2)
{
  vf1.Swap (vf2);
}

}

}

/*
    ������ ���� ������
*/

size_t VertexFormat::Impl::Hash ()
{
  if (need_hash_update)
  {
    attributes_hash  = crc32 (attributes, sizeof (VertexAttribute) * attributes_count);
    need_hash_update = false;
  }

  return attributes_hash;
}

/*
    ���������
*/

bool VertexFormat::operator == (const VertexFormat& vf) const
{
  return impl->attributes_count == vf.impl->attributes_count && impl->Hash () == vf.impl->Hash ();
}

bool VertexFormat::operator != (const VertexFormat& vf) const
{
  return !(*this == vf);
}

/*
    ��������� �������������
*/

namespace media
{

namespace geometry
{

//������ ���� �������� ������ � ������
size_t get_type_size (VertexAttributeType type)
{
  switch (type)
  {
    case VertexAttributeType_Float2:    return sizeof (float) * 2;
    case VertexAttributeType_Float3:    return sizeof (float) * 3;
    case VertexAttributeType_Float4:    return sizeof (float) * 4;
    case VertexAttributeType_Short2:    return sizeof (short) * 2;
    case VertexAttributeType_Short3:    return sizeof (short) * 3;    
    case VertexAttributeType_Short4:    return sizeof (short) * 4;
    case VertexAttributeType_UByte4:    return sizeof (unsigned char) * 4;
    case VertexAttributeType_Influence: return sizeof (VertexInfluence);
    default:                            RaiseInvalidArgument ("media::get_type_size", "type", type);
  }
  
  return 0;
}

//���������� �����������
size_t get_components_count (VertexAttributeType type)
{
  switch (type)
  {
    case VertexAttributeType_Short2:
    case VertexAttributeType_Float2:    return 2;
    case VertexAttributeType_Short3:
    case VertexAttributeType_Float3:    return 3;
    case VertexAttributeType_Short4:
    case VertexAttributeType_UByte4:
    case VertexAttributeType_Float4:    return 4;
    case VertexAttributeType_Influence: return 1;
    default:                            RaiseInvalidArgument ("media::get_components_count", "type", type);
  }

  return 0;
}

//��� ���������
const char* get_semantic_name (VertexAttributeSemantic semantic)
{
  switch (semantic)
  {
    case VertexAttributeSemantic_Position:  return "position";
    case VertexAttributeSemantic_Normal:    return "normal";
    case VertexAttributeSemantic_Color:     return "color";
    case VertexAttributeSemantic_Tangent:   return "tangent";
    case VertexAttributeSemantic_Binormal:  return "binormal";
    case VertexAttributeSemantic_TexCoord0: return "texcoord0";
    case VertexAttributeSemantic_TexCoord1: return "texcoord1";
    case VertexAttributeSemantic_TexCoord2: return "texcoord2";
    case VertexAttributeSemantic_TexCoord3: return "texcoord3";
    case VertexAttributeSemantic_TexCoord4: return "texcoord4";
    case VertexAttributeSemantic_TexCoord5: return "texcoord5";
    case VertexAttributeSemantic_TexCoord6: return "texcoord6";
    case VertexAttributeSemantic_TexCoord7: return "texcoord7";
    case VertexAttributeSemantic_Influence: return "influence";
    default:                                RaiseInvalidArgument ("media::geometry::get_semantic_name(VertexAttributeSemantic)", "semantic", semantic);
  }

  return "";
}

//��������� ��������� �� �����
VertexAttributeSemantic get_vertex_attribute_semantic (const char* name, VertexAttributeSemantic default_semantic)
{
  if (!name)
    return default_semantic;
    
  struct Map
  {
    stl::hash_key<const char*> name_hash;
    VertexAttributeSemantic    semantic;
  };
  
  static Map map [] = {
    {"position",  VertexAttributeSemantic_Position},
    {"normal",    VertexAttributeSemantic_Normal},
    {"color",     VertexAttributeSemantic_Color},
    {"tangent",   VertexAttributeSemantic_Tangent},
    {"binormal",  VertexAttributeSemantic_Binormal},
    {"texcoord0", VertexAttributeSemantic_TexCoord0},
    {"texcoord1", VertexAttributeSemantic_TexCoord1},
    {"texcoord2", VertexAttributeSemantic_TexCoord2},
    {"texcoord3", VertexAttributeSemantic_TexCoord3},
    {"texcoord4", VertexAttributeSemantic_TexCoord4},
    {"texcoord5", VertexAttributeSemantic_TexCoord5},
    {"texcoord6", VertexAttributeSemantic_TexCoord6},
    {"texcoord7", VertexAttributeSemantic_TexCoord7},
    {"influence", VertexAttributeSemantic_Influence}
  };
  
  static const size_t map_size = sizeof (map) / sizeof (*map);
  
  const Map* map_iter = map;
  size_t     hash     = strhash (name);
  
  for (size_t i=0; i<map_size; i++, map_iter++)
    if (map_iter->name_hash.get_hash () == hash)
      return map_iter->semantic;
  
  return default_semantic;
}

//��� ����
const char* get_type_name (VertexAttributeType type)
{
  switch (type)
  {
    case VertexAttributeType_Float2:    return "float2";
    case VertexAttributeType_Float3:    return "float3";
    case VertexAttributeType_Float4:    return "float4";
    case VertexAttributeType_Short2:    return "short2";
    case VertexAttributeType_Short3:    return "short3";
    case VertexAttributeType_Short4:    return "short4";
    case VertexAttributeType_UByte4:    return "ubyte4";
    case VertexAttributeType_Influence: return "influence";
    default:                            RaiseInvalidArgument ("media::geometry::get_type_name(VertexAttributeType)", "type", type);
  }

  return "";
}

//��������� ���� �� �����
VertexAttributeType get_vertex_attribute_type (const char* name, VertexAttributeType default_type)
{
  if (!name)
    return default_type;

  struct Map
  {
    stl::hash_key<const char*> name_hash;
    VertexAttributeType        type;
  };

  static Map map [] = {
    {"float2",    VertexAttributeType_Float2},
    {"float3",    VertexAttributeType_Float3},
    {"float4",    VertexAttributeType_Float4},
    {"short2",    VertexAttributeType_Short2},
    {"short3",    VertexAttributeType_Short3},
    {"short4",    VertexAttributeType_Short4},
    {"ubyte4",    VertexAttributeType_UByte4},
    {"influence", VertexAttributeType_Influence}
  };

  static const size_t map_size = sizeof (map) / sizeof (*map);

  const Map* map_iter = map;
  size_t     hash     = strhash (name);

  for (size_t i=0; i<map_size; i++, map_iter++)
    if (map_iter->name_hash.get_hash () == hash)
      return map_iter->type;

  return default_type;
}

//�������� �������������
bool is_compatible (VertexAttributeSemantic semantic, VertexAttributeType type)
{
  switch (semantic)
  {
    case VertexAttributeSemantic_Position:
    case VertexAttributeSemantic_TexCoord0:
    case VertexAttributeSemantic_TexCoord1:
    case VertexAttributeSemantic_TexCoord2:
    case VertexAttributeSemantic_TexCoord3:
    case VertexAttributeSemantic_TexCoord4:
    case VertexAttributeSemantic_TexCoord5:
    case VertexAttributeSemantic_TexCoord6:
    case VertexAttributeSemantic_TexCoord7:
      switch (type)
      {
        case VertexAttributeType_Float2:
        case VertexAttributeType_Float3:
        case VertexAttributeType_Float4:
        case VertexAttributeType_Short2:
        case VertexAttributeType_Short3:
        case VertexAttributeType_Short4:
        case VertexAttributeType_UByte4: return true;
        default:                         return false;
      }
    case VertexAttributeSemantic_Normal:
    case VertexAttributeSemantic_Tangent:
    case VertexAttributeSemantic_Binormal:
      switch (type)
      {
        case VertexAttributeType_Float3:
        case VertexAttributeType_Short3: return true;
        default:                         return false;
      }
    case VertexAttributeSemantic_Color:
      switch (type)
      {
        case VertexAttributeType_Float3:
        case VertexAttributeType_Float4:
        case VertexAttributeType_Short3:
        case VertexAttributeType_Short4:
        case VertexAttributeType_UByte4: return true;
        default:                         return false;
      }
    case VertexAttributeSemantic_Influence:
      switch (type)
      {
        case VertexAttributeType_Influence: return true;
        default:                            return false;
      }
  }
  
  return false;
}

}

}
