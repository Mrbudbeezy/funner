#include "shared.h"

namespace
{

/*
    ���������
*/

const size_t PROPERTY_ARRAY_RESERVE_SIZE = 8; //������������� ���������� �������

/*
    �������� ����
*/

struct NodeProperty
{
  stl::string      name;      //��� ��������
  size_t           name_hash; //��� �����
  NodePropertyType type;      //��� ��������
  
///�����������
  NodeProperty (const char* in_name, NodePropertyType in_type)
    : name (in_name)
    , name_hash (strhash (in_name))
    , type (in_type)
  {
  }
};

template <NodePropertyType type> struct NodePropertyTypeMap;
template <>                      struct NodePropertyTypeMap { typedef stl::string type; };
template <>                      struct NodePropertyTypeMap { typedef int         type; };
template <>                      struct NodePropertyTypeMap { typedef float       type; };
template <>                      struct NodePropertyTypeMap { typedef math::vec4f type; };
template <>                      struct NodePropertyTypeMap { typedef math::mat4f type; };

template <NodePropertyType type> struct NodePropertyImpl: public NodeProperty
{
  typedef typename NodePropertyTypeMap<type>::type value_type;

  value_type value; //��������

///�����������
  NodePropertyImpl (const char* name)
    : NodeProperty (name, type)
    , value ()
  {    
  }
};

typedef stl::vector<NodeProperty*> PropertyArray;

}

/*
    �������� ���������� ������� ����
*/

struct NodeProperties::Impl
{
  xtl::reference_counter ref_count;        //������� ������
  bool                   need_hash_update; //���������� �� �������� ���
  size_t                 hash;             //��� ����������
  PropertyArray          properties;       //��������

///�����������
  Impl ()  
    : need_hash_update (true)
    , hash (0)
  {
    properties.reserve (PROPERTY_ARRAY_RESERVE_SIZE);
  }
  
///��������� ������� ��������
  size_t GetIndex (const char* name)
  {
    static const char* METHOD_NAME = "scene_graph::NodeProperties::Impl::GetIndex";
    
    if (!name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "name");
      
    size_t hash = strhash (name);
    size_t index = 0;  

    for (PropertyArray::iterator iter=properties.begin (), end=properties.end (); iter!=end; ++iter, ++index)
      if ((*iter)->name_hash == hash)
        return index;
        
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Property has not registered");
  }
  
///���������� ���������
  void AddProperty (stl::auto_ptr<NodeProperty> proeprty)
  {
    properties.push_back (property.get ());
    
    proeprty.release ();
  }
  
///���������� ����
  void UpdateHash ()
  {
//????  
  
    need_hash_update = true;       
  }
};

/*
    ����������� / ����������
*/

NodeProperties::NodeProperties ()
  : impl (new Impl)
{
}

NodeProperties::~NodeProperties ()
{
  RemoveAll ();

  delete impl;
}

/*
    �������� ������� ����
*/

NodeProperties::Pointer NodeProperties::Create ()
{
  return Pointer (new NodeProperties, false);
}

/*
    ���������� �������
*/

size_t NodeProperties::Size () const
{
  return impl->properties.size ();
}

/*
    ��� ����������
*/

size_t NodeProperties::Hash () const
{
  if (!impl->need_hash_update)
    return impl->hash;
    
  impl->UpdateHash ();
  
  return impl->hash;
}

/*
    ��� ��������
*/

NodePropertyType NodeProperties::Type (size_t index) const
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::Type";
  
  if (index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "index", index, impl->properties.size ());
    
  return impl->properties [index]->type;
}

NodePropertyType NodeProperties::Type (const char* name) const
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::Type";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  int index = IndexOf (name);

  if (index == -1)
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Property has not registered");
    
  return impl->properties [index]->type;
}

/*
    ����� ����
*/

void NodeProperties::SetType (size_t index, NodePropertyType type)
{
}

void NodeProperties::SetType (const char* name, NodePropertyType type)
{
  try
  {
    SetType (impl->GetIndex (name), type);
  } 
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::SetType");
    throw;
  }
}

/*
    ����� ������� ��������
*/

int NodeProperties::IndexOf (const char* name) const
{
  if (!name)
    return -1;
    
  size_t hash = strhash (name);
  int    index = 0;  

  for (PropertyArray::iterator iter=impl->properties.begin (), end=impl->properties.end (); iter!=end; ++iter, ++index)
    if ((*iter)->name_hash == hash)
      return index;
      
  return index;
}

bool NodeProperties::IsPresent (const char* name) const
{
  return IndexOf (name) != -1;
}

/*
    ��������� �������� �� �����
*/

void NodeProperties::SetProperty (const char* name, const char* value)
{
}

void NodeProperties::SetProperty (const char* name, float value);   
{
}

void NodeProperties::SetProperty (const char* name, int value)
{
}

void NodeProperties::SetProperty (const char* name, const math::vec4f& value);  
{
}

void NodeProperties::SetProperty (const char* name, const math::mat4f& value)
{
}

/*
    ��������� �������� �� �������
*/

void NodeProperties::SetProperty (size_t property_index, const char* value)
{
  static const char* METHOD_NAME = "scene_graph::SetProperty(size_t,const char*)";
  
  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());
    
  
  
}

void NodeProperties::SetProperty (size_t property_index, float value)
{
}

void NodeProperties::SetProperty (size_t property_index, int value)
{
}

void NodeProperties::SetProperty (size_t property_index, const math::vec4f& value);   
{
}

void NodeProperties::SetProperty (size_t property_index, const math::mat4f& value)
{
}

/*
    ������ �������� �� �����
*/

void NodeProperties::GetProperty (const char* name, stl::string& result)
{
}

void NodeProperties::GetProperty (const char* name, int& result)
{
}

void NodeProperties::GetProperty (const char* name, float& result)
{
}

void NodeProperties::GetProperty (const char* name, math::vec4f& result)
{
}

void NodeProperties::GetProperty (const char* name, math::mat4f& result)
{
}

const char* NodeProperties::GetString (const char* name) const
{  
  try
  {
    return GetString (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetString");
    throw;
  }
}

int NodeProperties::GetInteger (const char* name) const
{
  try
  {
    return GetInteger (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetInteger");
    throw;
  }
}

float NodeProperties::GetFloat (const char* name) const
{
  try
  {
    return GetFloat (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetFloat");
    throw;
  }
}

const math::vec4f& NodeProperties::GetVector (const char* name) const
{
  try
  {
    return GetVector (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetVector");
    throw;
  }
}

const math::mat4f& NodeProperties::GetMatrix (const char* name) const
{
  try
  {
    return GetMatrix (impl->GetIndex (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::NodeProperties::GetMatrix");
    throw;
  }
}

/*
    ������ �������� �� �������
*/

void NodeProperties::GetProperty (size_t property_index, stl::string& result)
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::GetProperty(size_t,stl::string&)";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());
    
  NodeProperty& property = impl->properties [property_index];
  
  switch (property.type)
  {
    case NodePropertyType_String:
      result = static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value;
      break;
    case NodePropertyType_Integer:
      to_string (result, static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value);
      break;
    case NodePropertyType_Float:
      to_string (result, static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value);
      break;
    case NodePropertyType_Vector:    
      to_string (result, static_cast<NodePropertyImpl<NodePropertyType_Vector>&> (property).value);
      break;    
    case NodePropertyType_Matrix:    
      to_string (result, static_cast<NodePropertyImpl<NodePropertyType_Matrix>&> (property).value);
      break;    
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

void NodeProperties::GetProperty (size_t property_index, int& result)
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::GetProperty(size_t,int&)";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());
    
  NodeProperty& property = impl->properties [property_index];
  
  switch (property.type)
  {
    case NodePropertyType_String:
      result = atoi (static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value.c_str ());
      break;
    case NodePropertyType_Integer:
      result = static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value;
      break;
    case NodePropertyType_Float:
      result = static_cast<int> (static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value);
      break;
    case NodePropertyType_Vector:    
    case NodePropertyType_Matrix:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Integer));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

void NodeProperties::GetProperty (size_t property_index, float& result)
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::GetProperty(size_t,float&)";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());
    
  NodeProperty& property = impl->properties [property_index];
  
  switch (property.type)
  {
    case NodePropertyType_String:
      result = static_cast<float> (atof (static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value.c_str ()));
      break;
    case NodePropertyType_Integer:
      result = static_cast<float> (static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value);
      break;
    case NodePropertyType_Float:
      result = static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value;
      break;
    case NodePropertyType_Vector:    
    case NodePropertyType_Matrix:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Float));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

void NodeProperties::GetProperty (size_t property_index, math::vec4f& result)
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::GetProperty(size_t,math::vec4f&)";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());
    
  NodeProperty& property = impl->properties [property_index];
  
  switch (property.type)
  {
    case NodePropertyType_String:
    {
      const char* value = static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value.c_str ();
    
      if (!read_and_cast<math::vec4f> (value, result))
        throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property %s='%s' from '%s' to %s", property.name.c_str (), get_name (property.type), value, get_name (NodePropertyType_Vector));

      break;
    }
    case NodePropertyType_Integer:
      result = static_cast<float> (static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value);
      break;
    case NodePropertyType_Float:
      result = static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value;
      break;
    case NodePropertyType_Vector:    
      result = static_cast<NodePropertyImpl<NodePropertyType_Vector>&> (property).value;
      break;
    case NodePropertyType_Matrix:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Vector));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

void NodeProperties::GetProperty (size_t property_index, math::mat4f& result)
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::GetProperty(size_t,math::mat4f&)";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());
    
  NodeProperty& property = impl->properties [property_index];
  
  switch (property.type)
  {
    case NodePropertyType_String:
    {
      const char* value = static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value.c_str ();

      if (!read_and_cast<math::mat4f> (value, result))
        throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property %s='%s' from '%s' to %s", property.name.c_str (), get_name (property.type), value, get_name (NodePropertyType_Matrix));

      break;
    }
    case NodePropertyType_Integer:
      result = static_cast<float> (static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value);
      break;
    case NodePropertyType_Float:
      result = static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value;
      break;
    case NodePropertyType_Matrix:
      result = static_cast<NodePropertyImpl<NodePropertyType_Matrix>&> (property).value;
      break;
    case NodePropertyType_Vector:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Matrix));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

const char* NodeProperties::GetString (size_t property_index) const
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::GetString";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());
    
  NodeProperty& property = impl->properties [property_index];
  
  switch (property.type)
  {
    case NodePropertyType_String:
      return static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value.c_str ();
    case NodePropertyType_Integer:
    case NodePropertyType_Float:
    case NodePropertyType_Vector:
    case NodePropertyType_Matrix:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_String));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

int NodeProperties::GetInteger (size_t property_index) const
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::GetInteger";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());

  NodeProperty& property = impl->properties [property_index];

  switch (property.type)
  {
    case NodePropertyType_String:
      return atoi (static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value.c_str ());
    case NodePropertyType_Integer:
      return static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value;
    case NodePropertyType_Float:
      return static_cast<int> (static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value);
    case NodePropertyType_Vector:
    case NodePropertyType_Matrix:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Integer));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

float NodeProperties::GetFloat (size_t property_index) const
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::GetFloat";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());

  NodeProperty& property = impl->properties [property_index];

  switch (property.type)
  {
    case NodePropertyType_String:
      return static_cast<float> (atof (static_cast<NodePropertyImpl<NodePropertyType_String>&> (property).value.c_str ()));
    case NodePropertyType_Integer:
      return static_cast<float> (static_cast<NodePropertyImpl<NodePropertyType_Integer>&> (property).value);
    case NodePropertyType_Float:
      return static_cast<NodePropertyImpl<NodePropertyType_Float>&> (property).value;
    case NodePropertyType_Vector:
    case NodePropertyType_Matrix:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Float));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

const math::vec4f& NodeProperties::GetVector (size_t property_index) const
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::GetVector";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());

  NodeProperty& property = impl->properties [property_index];

  switch (property.type)
  {
    case NodePropertyType_Vector:  
      return static_cast<NodePropertyImpl<NodePropertyType_Vector>&>.value;
    case NodePropertyType_String:      
    case NodePropertyType_Integer:      
    case NodePropertyType_Float:          
    case NodePropertyType_Matrix:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Vector));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

const math::mat4f& NodeProperties::GetMatrix (size_t property_index) const
{
  static const char* METHOD_NAME = "scene_graph::NodeProperties::GetMatrix";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());

  NodeProperty& property = impl->properties [property_index];

  switch (property.type)
  {
    case NodePropertyType_Matrix:  
      return static_cast<NodePropertyImpl<NodePropertyType_Matrix>&>.value;
    case NodePropertyType_String:      
    case NodePropertyType_Integer:      
    case NodePropertyType_Float:          
    case NodePropertyType_Vector:
      throw xtl::format_operation_exception (METHOD_NAME, "Could not convert property '%s' from %s to %s", property.name.c_str (), get_name (property.type), get_name (NodePropertyType_Matrix));
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error: wrong property '%s' type %d", property.name.c_str (), property.type);
  }
}

/*
    �������� ����������
*/

void NodeProperties::Remove (const char* name)
{
  int index = IndexOf (name);

  if (index == -1)
    return;
    
  Remove ((size_t)index);
}

void NodeProperties::Remove (size_t index)
{
  if (index >= impl->properties.size ())
    return;
    
  delete impl->properties [index];
  
  impl->properties.erase (impl->properties.begin () + index);
}

void NodeProperties::Clear ()
{
  for (PropertyArray::iterator iter=impl->properties.begin (), end=impl->properties.end (); iter!=end; ++iter)
    delete *iter;

  properties.clear ();
}

/*
    ������� ������
*/

void NodeProperties::AddRef ()
{
  ref_count.increment ();
}

void NodeProperties::Release ()
{
  if (ref_count.decrement ())
    delete this;
}

/*
    ��������� ����� ����
*/

namespace scene_graph
{

const char* get_name (NodePropertyType type)
{
  switch (type)
  {
    case NodePropertyType_String:  return "string";
    case NodePropertyType_Integer: return "integer";
    case NodePropertyType_Vector:  return "vector";
    case NodePropertyType_Matrix:  return "matrix";
    default:
      throw xtl::make_argument_exception ("scene_graph::get_name(NodePropertyType)", "type", type);
  }
}

}
