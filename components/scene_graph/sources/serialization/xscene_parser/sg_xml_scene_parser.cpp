#include "shared.h"

using namespace scene_graph;
using namespace common;
using namespace math;

namespace scene_graph
{

template <> void XmlSceneParser::CreateNode<SoundEmitter> (const common::ParseNode& decl, Node& parent, SceneContext& context);

}

/*
    ������� ���������� ����� �����
*/

namespace scene_graph
{

///����������� ���� �� ��������
template <class T> struct Tag2Value
{
  const char* tag;
  T           value;
};

template <class BaseIter> inline bool read (xtl::io::token_iterator<const char*, BaseIter>& iter, TextLineAlignment& alignment)
{
  if (!iter)
    return false;

  static const Tag2Value<TextLineAlignment> tags [] = {
    {"center",   TextLineAlignment_Center},
    {"left",     TextLineAlignment_Left},
    {"right",    TextLineAlignment_Right},
    {"top",      TextLineAlignment_Top},
    {"bottom",   TextLineAlignment_Bottom},
    {"baseline", TextLineAlignment_BaseLine},
  };
  
  static const size_t tags_count = sizeof (tags) / sizeof (*tags);
  
  const char* string = *iter;
  
  for (size_t i=0; i<tags_count; i++)
    if (!strcmp (tags [i].tag, string))
    {
      alignment = tags [i].value;

      ++iter;
      
      return true; 
    }
    
  return false;
}

template <class BaseIter> inline bool read (xtl::io::token_iterator<const char*, BaseIter>& iter, SpriteMode& mode)
{
  if (!iter)
    return false;

  static const Tag2Value<SpriteMode> tags [] = {
    {"billboard",          SpriteMode_Billboard},
    {"oriented",           SpriteMode_Oriented},
    {"oriented_billboard", SpriteMode_OrientedBillboard},
  };
  
  static const size_t tags_count = sizeof (tags) / sizeof (*tags);
  
  const char* string = *iter;
  
  for (size_t i=0; i<tags_count; i++)
    if (!strcmp (tags [i].tag, string))
    {
      mode = tags [i].value;

      ++iter;
      
      return true; 
    }
    
  return false;
}

template <class BaseIter> inline bool read (xtl::io::token_iterator<const char*, BaseIter>& iter, SpriteUsage& usage)
{
  if (!iter)
    return false;

  static const Tag2Value<SpriteUsage> tags [] = {
    {"static",   SpriteUsage_Static},
    {"dynamic",  SpriteUsage_Dynamic},
    {"stream",   SpriteUsage_Stream},
    {"batching", SpriteUsage_Batching},
  };
  
  static const size_t tags_count = sizeof (tags) / sizeof (*tags);
  
  const char* string = *iter;
  
  for (size_t i=0; i<tags_count; i++)
    if (!strcmp (tags [i].tag, string))
    {
      usage = tags [i].value;

      ++iter;
      
      return true; 
    }
    
  return false;
}

}

namespace
{

/*
    ���������
*/

const math::vec3f DEFAULT_BEFORE_NODE_OFFSET (0.0f, 0.0f, 1e-06f);

/*
    �������� ���������� ��������
*/

///�������� ���������� ����
struct NodeDecl: public xtl::reference_counter
{
  struct Pivot
  {
    math::vec3f position;              //����� ��������
    bool        has_orientation_pivot; //����� �������� �� ��������� � �������������� �������
    bool        has_scale_pivot;       //����� ��������������� �� ��������� � �������������� �������    
    
    Pivot ()
      : has_orientation_pivot (true)
      , has_scale_pivot (true) 
    {
    }
  };

  stl::string                name;                //��� ����    
  math::vec3f                position;            //���������
  math::vec3f                scale;               //�������
  math::quatf                orientation;         //����������
  bool                       orientation_inherit; //����������� �� ����������
  bool                       scale_inherit;       //����������� �� �������
  stl::auto_ptr<Pivot>       pivot;               //����� ��������/���������������
  bool                       is_world_transform;  //������������� ������ � ������� �����������
  stl::auto_ptr<stl::string> before_node;         //��� ����, ����� ������� ����� ��������� ������ ����
  stl::auto_ptr<PropertyMap> properties;          //���������������� �������� ����
  stl::auto_ptr<stl::string> parent_name;         //��� ������������� ����

///�����������
  NodeDecl ()
    : scale (1.0f)
    , orientation_inherit (true)
    , scale_inherit (true)
    , is_world_transform (false)
  {
  }  
};

///�������� �������
template <class T> struct Param
{
  T    value;
  bool state;
  
  Param () : value (), state () {}
  
  void Parse (const ParseNode& node, const char* name)
  {
    ParseNode child = node.First (name);
    
    if (!child)
    {
      state = false;
      return;
    }

    value = get<T> (child, "");

    state = true;
  }
};

template <class T, unsigned int Size> struct Param<math::vector<T, Size> >
{
  math::vector<T, Size> value;
  bool                  state;

  Param () : value (), state () {}

  void Parse (const ParseNode& node, const char* name)
  {
    ParseNode child = node.First (name);
    
    if (!child)
    {
      state = false;
      return;
    }

    parse_attribute (child, "", Size, &value [0]);

    state = true;
  }
};

///�������� ���������� ������
struct OrthoCameraDecl: public xtl::reference_counter
{
  enum ParamId
  {
    Left, Right, Top, Bottom, ZNear, ZFar, Param_Num
  };  
  
  Param<float> params [Param_Num];
};

struct PerspectiveCameraDecl: public xtl::reference_counter
{
  enum ParamId
  {
    FovX, FovY, ZNear, ZFar, Param_Num
  };  
  
  Param<float> params [Param_Num];
};

///�������� ���������� ��������� �����
struct LightDecl: public xtl::reference_counter
{
  Param<math::vec3f> light_color;
  Param<math::vec3f> attenuation;
  Param<float>       range;
  Param<float>       intensity;
  Param<float>       exponent;
  Param<float>       radius;
  Param<float>       angle;
};

///�������� ���������� ���������� ������
struct StaticMeshDecl: public xtl::reference_counter
{
  stl::string        source;
  Param<math::vec3f> min_bound;
  Param<math::vec3f> max_bound;  
};

///�������� ���������� �������
struct SpriteDecl: public xtl::reference_counter
{
  stl::string                 material;
  stl::auto_ptr<stl::string>  layout;
  Param<float>                alpha;
  Param<math::vec3f>          color;
  Param<math::vec2f>          tex_offset;
  Param<math::vec2f>          tex_size;
  Param<math::vec3f>          up;
  Param<SpriteMode>           mode;
  Param<SpriteUsage>          usage;
};

///�������� ���������� ��������� ������
struct TextLineDecl: public xtl::reference_counter
{
  stl::string              text;
  stl::string              font;
  Param<math::vec3f>       color;
  Param<float>             alpha;
  Param<TextLineAlignment> horizontal_alignment;
  Param<TextLineAlignment> vertical_alignment;
};

///�������� ���������� ��������� �����
struct SoundEmitterDecl: public xtl::reference_counter
{
  stl::string  source;
  Param<float> gain;
};

typedef xtl::intrusive_ptr<NodeDecl>              NodeDeclPtr;
typedef xtl::intrusive_ptr<OrthoCameraDecl>       OrthoCameraDeclPtr;
typedef xtl::intrusive_ptr<PerspectiveCameraDecl> PerspectiveCameraDeclPtr;
typedef xtl::intrusive_ptr<LightDecl>             LightDeclPtr;
typedef xtl::intrusive_ptr<StaticMeshDecl>        StaticMeshDeclPtr;
typedef xtl::intrusive_ptr<SpriteDecl>            SpriteDeclPtr;
typedef xtl::intrusive_ptr<TextLineDecl>          TextLineDeclPtr;
typedef xtl::intrusive_ptr<SoundEmitterDecl>      SoundEmitterDeclPtr;

PropertyType get_property_type (const common::ParseNode& node)
{
  const char* value = get<const char*> (node, "");

  if (!xtl::xstrcmp (value, "string")) return PropertyType_String;
  if (!xtl::xstrcmp (value, "int"))    return PropertyType_Integer;
  if (!xtl::xstrcmp (value, "float"))  return PropertyType_Float;
  if (!xtl::xstrcmp (value, "vector")) return PropertyType_Vector;
  if (!xtl::xstrcmp (value, "matrix")) return PropertyType_Matrix;

  raise_parser_exception (node, "Unexpected property type '%s'", value);

  return common::PropertyType_String;
}

}

/*
    �������� ���������� �������
*/

struct XmlSceneParser::Impl
{
  ///�������� �������
  struct ParserDesc
  {
    ParseHandler   parse_handler;
    PrepareHandler prepare_handler;
    
    ParserDesc (const ParseHandler& in_parse_handler, const PrepareHandler& in_prepare_handler)
      : parse_handler (in_parse_handler)
      , prepare_handler (in_prepare_handler)
    {
    }
  };

  typedef stl::hash_map<stl::hash_key<const char*>, common::ParseNode>  IncludeMap;
  typedef stl::hash_map<stl::hash_key<const char*>, ParserDesc>         ParserMap;

  common::ParseNode root;        //�������� ����
  SceneParserCache  cache;       //��� �������
  ResourceGroup     resources;   //������� �����
  IncludeMap        includes;    //����� ������������ ������
  ParserMap         parsers;     //�������
  bool              prepared;    //��� �������� ��������������� ������
  
///�����������
  Impl (const ParseNode& in_root)
    : root (in_root)
    , prepared (false)
  {
  }
  
///��������������� ������
  void Prepare (const ParseNode& decl)
  {
    for (Parser::Iterator iter=decl.First (); iter; ++iter)
    {
      try
      {        
        ParseNode decl = *iter;

        const char* type = decl.Name ();

        ParserMap::iterator iter = parsers.find (type);

        if (iter == parsers.end () || !iter->second.prepare_handler)
          continue; //������������� ����������� �����                    

        iter->second.prepare_handler (decl);
      }
      catch (common::ParserException&)
      {
        //�������������, ���������������� ��� �����������
      }
      catch (std::exception& e)
      {
        root.Log ().Error (*iter, "%s", e.what ());
      }
      catch (...)
      {
        root.Log ().Error (*iter, "unknown exception");
      }
    }
    
    prepared = true;
  }

///������ ���� �������� ��������
  void PrepareResourceNode (const ParseNode& decl)
  {
    const char* name = get<const char*> (decl, "name");
    
    if (!name)
      return;      
      
    resources.Add (name);
  }
  
///������ ������������� ����
  void PrepareIncludeNode (const ParseNode& decl)
  {
    const char* source_name = get<const char*> (decl, "source");

    bool ignore_unavailability = strcmp (get<const char*> (decl, "ignore_unavailability", "false"), "true") == 0;
    
    if (!source_name)
      return;
      
      //������� ����� � ������ ��� �����������
      
    IncludeMap::iterator iter = includes.find (source_name);
    
    if (iter != includes.end ())
      return;

      //�������

    if (!FileSystem::IsFileExist (source_name) && ignore_unavailability)
      return;

    common::ParseLog root_log = root.Log ();

    Parser parser (root_log, source_name, "xml");
    
    const ParseNode& xscene_root = parser.Root ().First ();
    
    if (strcmp (xscene_root.Name (), XSCENE_ROOT))
      throw xtl::format_operation_exception ("", "Bad XML scene file '%s'. Root node not found", source_name);
      
    bool partial = strcmp (get<const char*> (xscene_root, "partial", ""), "true") == 0;

    if (!partial)
      throw xtl::format_operation_exception ("", "Bad XML scene file '%s'. Only partial definitions are allowed in this context", source_name);            

      //���������� ������ �� ������������ ����
      
    includes.insert_pair (source_name, xscene_root);
    
      //��������� ������
      
    Prepare (xscene_root);
  }

///��������������� ������
  NodeDeclPtr              PrepareNode              (const ParseNode& decl);
  OrthoCameraDeclPtr       PrepareOrthoCamera       (const ParseNode& decl);
  PerspectiveCameraDeclPtr PreparePerspectiveCamera (const ParseNode& decl);
  LightDeclPtr             PrepareLight             (const ParseNode& decl);
  StaticMeshDeclPtr        PrepareStaticMesh       (const ParseNode& decl);  
  SpriteDeclPtr            PrepareSprite            (const ParseNode& decl);  
  TextLineDeclPtr          PrepareTextLine          (const ParseNode& decl);
  SoundEmitterDeclPtr      PrepareSoundEmitter      (const ParseNode& decl);
};

/*
    ����������� / ����������
*/

XmlSceneParser::XmlSceneParser (const ParseNode& root)
{
  try
  { 
      //�������� ����������            

    impl = new Impl (root);
    
      //����������� ��������
      
    RegisterParser ("node", xtl::bind (&XmlSceneParser::CreateNode<Node>, this, _1, _2, _3), xtl::bind (&XmlSceneParser::Impl::PrepareNode, &*impl, _1));
    RegisterParser ("ortho_camera", xtl::bind (&XmlSceneParser::CreateNode<OrthoCamera>, this, _1, _2, _3), xtl::bind (&XmlSceneParser::Impl::PrepareOrthoCamera, &*impl, _1));
    RegisterParser ("perspective_camera", xtl::bind (&XmlSceneParser::CreateNode<PerspectiveCamera>, this, _1, _2, _3), xtl::bind (&XmlSceneParser::Impl::PreparePerspectiveCamera, &*impl, _1));
    RegisterParser ("spot_light", xtl::bind (&XmlSceneParser::CreateNode<SpotLight>, this, _1, _2, _3), xtl::bind (&XmlSceneParser::Impl::PrepareLight, &*impl, _1));
    RegisterParser ("direct_light", xtl::bind (&XmlSceneParser::CreateNode<DirectLight>, this, _1, _2, _3), xtl::bind (&XmlSceneParser::Impl::PrepareLight, &*impl, _1));
    RegisterParser ("point_light", xtl::bind (&XmlSceneParser::CreateNode<PointLight>, this, _1, _2, _3), xtl::bind (&XmlSceneParser::Impl::PrepareLight, &*impl, _1));
    RegisterParser ("mesh", xtl::bind (&XmlSceneParser::CreateNode<StaticMesh>, this, _1, _2, _3), xtl::bind (&XmlSceneParser::Impl::PrepareStaticMesh, &*impl, _1));
    RegisterParser ("text_line", xtl::bind (&XmlSceneParser::CreateNode<TextLine>, this, _1, _2, _3), xtl::bind (&XmlSceneParser::Impl::PrepareTextLine, &*impl, _1));
    RegisterParser ("sprite", xtl::bind (&XmlSceneParser::CreateNode<Sprite>, this, _1, _2, _3), xtl::bind (&XmlSceneParser::Impl::PrepareSprite, &*impl, _1));
    RegisterParser ("listener", xtl::bind (&XmlSceneParser::CreateNode<Listener>, this, _1, _2, _3));
    RegisterParser ("sound", xtl::bind (&XmlSceneParser::CreateNode<SoundEmitter>, this, _1, _2, _3), xtl::bind (&XmlSceneParser::Impl::PrepareSoundEmitter, &*impl, _1));
    RegisterParser ("include", xtl::bind (&XmlSceneParser::IncludeSubscene, this, _1, _2, _3), xtl::bind (&XmlSceneParser::Impl::PrepareIncludeNode, &*impl, _1));
    RegisterParser ("resource", ParseHandler (), xtl::bind (&XmlSceneParser::Impl::PrepareResourceNode, &*impl, _1));  
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::XmlSceneParser");
    throw;
  }
}

XmlSceneParser::~XmlSceneParser ()
{
}

/*
    �������� �������
*/

ISceneParser* XmlSceneParser::Create (const ParseNode& root)
{
  try
  {
    return new XmlSceneParser (root);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Create");
    throw;
  }
}

/*
    ��������������� ������
*/

void XmlSceneParser::Prepare ()
{
  try
  {
    impl->Prepare (impl->root);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Prepare");
    throw;
  }
}

/*
    ��������� ������ ��������
*/

const ResourceGroup& XmlSceneParser::Resources ()
{
  return impl->resources;
}

/*
    �������� ����
*/

const ParseNode& XmlSceneParser::Root () const
{
  return impl->root;
}

/*
    ���
*/

SceneParserCache& XmlSceneParser::Cache ()
{
  return impl->cache;
}

/*
    ����������� ������� ���� ������������� ����
*/

void XmlSceneParser::RegisterParser (const char* type, const ParseHandler& parse_handler)
{
  RegisterParser (type, parse_handler, PrepareHandler ());
}

void XmlSceneParser::RegisterParser (const char* type, const ParseHandler& parse_handler, const PrepareHandler& prepare_handler)
{
  if (!type)
    throw xtl::make_null_argument_exception ("scene_graph::XmlSceneParser::RegisterParser", "type");
    
  impl->parsers.insert_pair (type, Impl::ParserDesc (parse_handler, prepare_handler));
}

void XmlSceneParser::UnregisterParser (const char* type)
{
  if (!type)
    return;
    
  impl->parsers.erase (type);
}

void XmlSceneParser::UnregisterAllParsers ()
{
  impl->parsers.clear ();
}

/*
    ������ ���������
*/

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::vec2f& value)
{
  try
  {    
    parse_attribute (decl, name, 2, &value [0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::vec2f&)");
    throw;
  }
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::vec3f& value)
{
  try
  {
    parse_attribute (decl, name, 3, &value [0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::vec3f&)");
    throw;
  }    
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::vec4f& value)
{
  try
  {
    parse_attribute (decl, name, 4, &value [0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::vec4f&)");
    throw;
  }  
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::quatf& value)
{
  try
  {
    parse_attribute (decl, name, 4, &value [0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::quatf&)");
    throw;
  }    
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::mat2f& value)
{
  try
  {
    parse_attribute (decl, name, 4, &value [0][0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::mat2f&)");
    throw;
  }    
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::mat3f& value)
{
  try
  {
    parse_attribute (decl, name, 9, &value [0][0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::mat3f&)");
    throw;
  }    
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::mat4f& value)
{
  try
  {
    parse_attribute (decl, name, 16, &value [0][0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::mat4f&)");
    throw;
  }    
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, size_t size, float* value)
{
  try
  {
    parse_attribute (decl, name, size, value);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,size_t,float*)");
    throw;
  }    
}

/*
    �������� �����
*/

namespace
{

void print_log (const SceneManager::LogHandler& log_handler, const char* message)
{
  try
  {
    log_handler (message);
  }
  catch (...)
  {
  }
}

}

void XmlSceneParser::CreateScene (Node& parent, SceneContext& context)
{
  try
  {
    if (!impl->prepared)
      Prepare ();
    
    Parse (impl->root, parent, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::CreateScene");
    throw;
  }
}
    
/*
    ������ ����
*/

template <class T>
void XmlSceneParser::CreateNode (const common::ParseNode& decl, Node& parent, SceneContext& context)
{
  typename T::Pointer node = T::Create ();

  Parse (decl, *node, parent, context);
}

void XmlSceneParser::IncludeSubscene (const common::ParseNode& decl, Node& parent, SceneContext& context)
{
  const char* source_name = "";

  try
  {
    source_name = get<const char*> (decl, "source", "");
    
    Impl::IncludeMap::iterator iter = impl->includes.find (source_name);
    
    if (iter == impl->includes.end ())
      return;
      
    Parse (iter->second, parent, context);
  }
  catch (xtl::exception& e)
  {
    if (source_name) e.touch ("scene_graph::XmlSceneParser::IncludeSubscene");
    else             e.touch ("scene_graph::XmlSceneParser::IncludeSubscene('%s')", source_name);

    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, Node& parent, SceneContext& context)
{
  try
  {
    for (Parser::Iterator iter=decl.First (); iter; ++iter)
    {
      try
      {
        ParseNode decl = *iter;

        const char* type = decl.Name ();

        Impl::ParserMap::iterator iter = impl->parsers.find (type);

        if (iter == impl->parsers.end () || !iter->second.parse_handler)
          continue; //������������� ����������� �����

        iter->second.parse_handler (decl, parent, context);
      }
      catch (std::exception& e)
      {
        if (!context.FilterError (e.what ()))
          throw;
        
        print_log (context.LogHandler (), e.what ());
      }
      catch (...)            
      {
        static const char* ERROR_STRING = "unknown exception";
        
        if (!context.FilterError (ERROR_STRING))
          throw;        

        print_log (context.LogHandler (), ERROR_STRING);
      }
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseDispatch");
    throw;
  }  
}

/*
    �������
*/

NodeDeclPtr XmlSceneParser::Impl::PrepareNode (const ParseNode& decl)
{
  try
  {
      //������� ����� ��������� � ����
      
    if (NodeDeclPtr* node_decl_ptr = cache.FindValue<NodeDeclPtr> (decl))
      return *node_decl_ptr;

    NodeDeclPtr node_decl (new NodeDecl, false);

      //������� ������� �������
    
    node_decl->name = get<const char*> (decl, "id", "");

      //������� ����� ��������

    if (decl.First ("pivot"))      
    {
      stl::auto_ptr<NodeDecl::Pivot> pivot (new NodeDecl::Pivot);

      ParseAttribute (decl, "pivot", pivot->position);

      pivot->has_orientation_pivot = strcmp (get<const char*> (decl, "orientation_pivot", "true"), "true") == 0;
      pivot->has_scale_pivot       = strcmp (get<const char*> (decl, "scale_pivot", "true"), "true") == 0;

      node_decl->pivot = pivot;
    }

      //������� �������������

    node_decl->is_world_transform = strcmp (get<const char*> (decl, "bind_space", "local"), "world") == 0;

    if (ParseNode tm_node = decl.First ("transform"))
    {
      math::mat4f tm;        
      
      ParseAttribute (tm_node, "", tm);
      
      affine_decompose (tm, node_decl->position, node_decl->orientation, node_decl->scale);
    }
    else
    {
      ParseAttribute (decl, "position", node_decl->position);
      ParseAttribute (decl, "scale", node_decl->scale);        
      
      if (ParseNode rotation_node = decl.First ("rotation"))
      {
        float rotation [3] = {0.0f, 0.0f, 0.0f};
        
        ParseAttribute (rotation_node, "", 3, &rotation [0]);
        
        node_decl->orientation = to_quat (degree (rotation [0]), degree (rotation [1]), degree (rotation [2]));
      }
      else
      {
        ParseAttribute (decl, "orientation", node_decl->orientation);
      }
    }
    
    node_decl->orientation_inherit = strcmp (get<const char*> (decl, "orientation_inherit", "true"), "true") == 0;
    node_decl->scale_inherit       = strcmp (get<const char*> (decl, "scale_inherit", "true"), "true") == 0;

      //������� ���������������� �������              

    PropertyMap* properties = 0;
    
    if (decl.First ("property"))
      node_decl->properties.reset (properties = new PropertyMap);

    for (Parser::NamesakeIterator iter = decl.First ("property"); iter; ++iter)
    {
      ParseNode property_decl = *iter;

      const char*  name  = get<const char*> (property_decl, "name");
      PropertyType type  = get_property_type (property_decl.First ("type"));

      size_t property_index = properties->AddProperty (name, type, 1);
      
      stl::string value = get<const char*> (property_decl, "value");
      
      properties->SetProperty (property_index, value.c_str ());
    }

      //������� after node
      
    if (ParseNode before_node_decl = decl.First ("before_node"))
    {
      node_decl->before_node.reset (new stl::string);
      
      *node_decl->before_node = get<const char*> (before_node_decl, "");
    }
            
      //������� �������� � ��������
      
    if (ParseNode parent_name_decl = decl.First ("parent"))
    {
      node_decl->parent_name.reset (new stl::string);
      
      *node_decl->parent_name = get<const char*> (parent_name_decl, "");
    }

      //����������� ����������� ����
      
    cache.SetValue (decl, node_decl);
    
    return node_decl;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Impl::PrepareNode");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, Node& node, Node& default_parent, SceneContext& context)
{
  try
  {
      //���������� ����

    NodeDeclPtr node_decl = impl->PrepareNode (decl);

      //��������� ����

    if (!node_decl->name.empty ())
      node.SetName (node_decl->name.c_str ());
      
    if (node_decl->pivot)
    {
      node.SetPivotPosition (node_decl->pivot->position);
      node.SetOrientationPivotEnabled (node_decl->pivot->has_orientation_pivot);
      node.SetScalePivotEnabled (node_decl->pivot->has_scale_pivot);
    }
      
    node.SetPosition (node_decl->position);
    node.SetOrientation (node_decl->orientation);
    node.SetScale (node_decl->scale);
    
    node.SetOrientationInherit (node_decl->orientation_inherit);
    node.SetScaleInherit (node_decl->scale_inherit);
    
    if (node_decl->properties)
      node.SetProperties (node_decl->properties->Clone ());
      
    if (node_decl->before_node && !node_decl->before_node->empty ()) //TODO: ������� � ���� �����������!!!
    {
      math::vec3f offset = DEFAULT_BEFORE_NODE_OFFSET;
      
      int property_index = context.Properties ().IndexOf ("BeforeNodeOffset");
      
      if (property_index != -1)
        offset = context.Properties ().GetVector (property_index);        
        
      for (Node* search_base = &default_parent; search_base; search_base = &*search_base->Parent ())
      {
        if (Node::Pointer before_node = search_base->FindChild (node_decl->before_node->c_str (), NodeSearchMode_OnAllSublevels))
        {
          offset *= -before_node->WorldOrtZ ();
          
          node.SetPosition (node.Position () + offset);
        }
      }
    }

    Node::Pointer parent = &default_parent;
    
    if (node_decl->parent_name && !node_decl->parent_name->empty ())
    {
      for (Node* search_base = &default_parent; (search_base = &*search_base->Parent ());)
      {
        if (Node::Pointer node = search_base->FindChild (node_decl->parent_name->c_str (), NodeSearchMode_OnAllSublevels))
        {
          parent = node;
          break;
        }
      }
    }    

    node.BindToParent (*parent, NodeBindMode_AddRef, node_decl->is_world_transform ? NodeTransformSpace_World : NodeTransformSpace_Local);

      //������ ��������� �����
      
    Parse (decl, node, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,Node&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, Entity& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Node&> (node), parent, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,Entity&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, Camera& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Entity&> (node), parent, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,Camera&,Node&,SceneContext&)");
    throw;
  }
}

OrthoCameraDeclPtr XmlSceneParser::Impl::PrepareOrthoCamera (const ParseNode& decl)
{
  try
  {
      //������� ����� ��������� � ����      

    if (OrthoCameraDeclPtr* node_decl_ptr = cache.FindValue<OrthoCameraDeclPtr> (decl))
      return *node_decl_ptr;
      
    OrthoCameraDeclPtr node_decl (new OrthoCameraDecl, false);
    
    node_decl->params [OrthoCameraDecl::Left].Parse (decl, "left");
    node_decl->params [OrthoCameraDecl::Right].Parse (decl, "right");
    node_decl->params [OrthoCameraDecl::Top].Parse (decl, "top");
    node_decl->params [OrthoCameraDecl::Bottom].Parse (decl, "bottom");
    node_decl->params [OrthoCameraDecl::ZNear].Parse (decl, "znear");
    node_decl->params [OrthoCameraDecl::ZFar].Parse (decl, "zfar");
    
      //����������� ����������� ����
      
    cache.SetValue (decl, node_decl);
    
    return node_decl;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Impl::PrepareOrthoCamera");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, OrthoCamera& node, Node& parent, SceneContext& context)
{
  try
  { 
      //��������������� ������
      
    OrthoCameraDeclPtr node_decl = impl->PrepareOrthoCamera (decl);

      //��������� ������
      
    if (node_decl->params [OrthoCameraDecl::Left].state)    node.SetLeft (node_decl->params [OrthoCameraDecl::Left].value);
    if (node_decl->params [OrthoCameraDecl::Right].state)   node.SetRight (node_decl->params [OrthoCameraDecl::Right].value);
    if (node_decl->params [OrthoCameraDecl::Top].state)     node.SetTop (node_decl->params [OrthoCameraDecl::Top].value);
    if (node_decl->params [OrthoCameraDecl::Bottom].state)  node.SetBottom (node_decl->params [OrthoCameraDecl::Bottom].value);
    if (node_decl->params [OrthoCameraDecl::ZNear].state)   node.SetZNear (node_decl->params [OrthoCameraDecl::ZNear].value);
    if (node_decl->params [OrthoCameraDecl::ZFar].state)    node.SetZFar (node_decl->params [OrthoCameraDecl::ZFar].value);

      //������ ������������ ����������
    
    Parse (decl, static_cast<Camera&> (node), parent, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,OrthoCamera&,Node&,SceneContext&)");
    throw;
  }
}

PerspectiveCameraDeclPtr XmlSceneParser::Impl::PreparePerspectiveCamera (const ParseNode& decl)
{
  try
  {
      //������� ����� ��������� � ����      

    if (PerspectiveCameraDeclPtr* node_decl_ptr = cache.FindValue<PerspectiveCameraDeclPtr> (decl))
      return *node_decl_ptr;
      
    PerspectiveCameraDeclPtr node_decl (new PerspectiveCameraDecl, false);
    
    node_decl->params [PerspectiveCameraDecl::FovX].Parse (decl, "fov_x");
    node_decl->params [PerspectiveCameraDecl::FovY].Parse (decl, "fov_y");
    node_decl->params [PerspectiveCameraDecl::ZNear].Parse (decl, "znear");
    node_decl->params [PerspectiveCameraDecl::ZFar].Parse (decl, "zfar");
    
      //����������� ����������� ����
      
    cache.SetValue (decl, node_decl);    
    
    return node_decl;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Impl::PreparePerspectiveCamera");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, PerspectiveCamera& node, Node& parent, SceneContext& context)
{
  try
  {
      //������� ����� ��������� � ����
      
    PerspectiveCameraDeclPtr node_decl = impl->PreparePerspectiveCamera (decl);

      //��������� ������

    if (node_decl->params [PerspectiveCameraDecl::FovX].state)  node.SetFovX (degree (node_decl->params [PerspectiveCameraDecl::FovX].value));
    if (node_decl->params [PerspectiveCameraDecl::FovY].state)  node.SetFovY (degree (node_decl->params [PerspectiveCameraDecl::FovY].value)); 
    if (node_decl->params [PerspectiveCameraDecl::ZNear].state) node.SetZNear (node_decl->params [PerspectiveCameraDecl::ZNear].value);
    if (node_decl->params [PerspectiveCameraDecl::ZFar].state)  node.SetZFar (node_decl->params [PerspectiveCameraDecl::ZFar].value);

      //������ ������������ ����������

    Parse (decl, static_cast<Camera&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,PerspectiveCamera&,Node&,SceneContext&)");
    throw;
  }
}

LightDeclPtr XmlSceneParser::Impl::PrepareLight (const ParseNode& decl)
{
  try
  {
      //������� ����� ��������� � ����      

    if (LightDeclPtr* node_decl_ptr = cache.FindValue<LightDeclPtr> (decl))
      return *node_decl_ptr;
      
    LightDeclPtr node_decl (new LightDecl, false);
    
    node_decl->light_color.Parse (decl, "light_color");
    node_decl->intensity.Parse (decl, "intensity");
    node_decl->range.Parse (decl, "range");
    node_decl->attenuation.Parse (decl, "attenuation");
    node_decl->radius.Parse (decl, "radius");
    node_decl->exponent.Parse (decl, "exponent");
    node_decl->angle.Parse (decl, "angle");
    
      //����������� ����������� ����
      
    cache.SetValue (decl, node_decl);    
    
    return node_decl;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Impl::PrepareLight");
    throw;
  }  
}

void XmlSceneParser::Parse (const ParseNode& decl, Light& node, Node& parent, SceneContext& context)
{
  try
  {
      //��������������� ������
      
    LightDeclPtr node_decl = impl->PrepareLight (decl);
    
      //��������� ����
      
    if (node_decl->light_color.state) node.SetLightColor (node_decl->light_color.value);
    if (node_decl->attenuation.state) node.SetAttenuation (node_decl->attenuation.value);
    if (node_decl->intensity.state)   node.SetIntensity (node_decl->intensity.value);
    if (node_decl->range.state)       node.SetRange (node_decl->range.value);
      
      //������ ������������ ����������
    
    Parse (decl, static_cast<Entity&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,Light&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, DirectLight& node, Node& parent, SceneContext& context)
{
  try
  {
      //��������������� ������
      
    LightDeclPtr node_decl = impl->PrepareLight (decl);    
    
      //��������� ����
      
    if (node_decl->radius.state) node.SetRadius (node_decl->radius.value);
    
      //������ ������������ ����������    
    
    Parse (decl, static_cast<Light&> (node), parent, context);        
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,DirectLight&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, SpotLight& node, Node& parent, SceneContext& context)
{
  try
  {
      //��������������� ������
      
    LightDeclPtr node_decl = impl->PrepareLight (decl);    
    
      //��������� ����
      
    if (node_decl->exponent.state) node.SetExponent (node_decl->exponent.value);
    if (node_decl->angle.state)    node.SetAngle (degree (node_decl->angle.value));
    
      //������ ������������ ����������    
    
    Parse (decl, static_cast<Light&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,SpotLight&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, PointLight& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Light&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,PointLight&,Node&,SceneContext&)");
    throw;
  }
}

StaticMeshDeclPtr XmlSceneParser::Impl::PrepareStaticMesh (const ParseNode& decl)
{
  try
  {
      //������� ����� ��������� � ����      

    if (StaticMeshDeclPtr* node_decl_ptr = cache.FindValue<StaticMeshDeclPtr> (decl))
      return *node_decl_ptr;
      
    StaticMeshDeclPtr node_decl (new StaticMeshDecl, false);
    
    node_decl->min_bound.Parse (decl, "min_bound");
    node_decl->max_bound.Parse (decl, "max_bound");

    node_decl->source = get<const char*> (decl, "source", "");

      //����������� ����������� ����
      
    cache.SetValue (decl, node_decl);    
    
    return node_decl;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Impl::PrepareStaticMesh");
    throw;
  }  
}

void XmlSceneParser::Parse (const ParseNode& decl, StaticMesh& node, Node& parent, SceneContext& context)
{
  try
  {
      //��������������� ������
      
    StaticMeshDeclPtr node_decl = impl->PrepareStaticMesh (decl);

      //��������� ����

    node.SetMeshName (node_decl->source.c_str ());

    bound_volumes::aaboxf box;
    
    if (node_decl->min_bound.state) box.set_minimum (node_decl->min_bound.value);
    if (node_decl->max_bound.state) box.set_maximum (node_decl->max_bound.value);

    node.SetBoundBox (box);

      //������ ������������ ����������
    
    Parse (decl, static_cast<Entity&> (node), parent, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,StaticMesh&,Node&,SceneContext&)");
    throw;
  }
}

SpriteDeclPtr XmlSceneParser::Impl::PrepareSprite (const ParseNode& decl)
{
  try
  {
      //������� ����� ��������� � ����      

    if (SpriteDeclPtr* node_decl_ptr = cache.FindValue<SpriteDeclPtr> (decl))
      return *node_decl_ptr;
      
    SpriteDeclPtr node_decl (new SpriteDecl, false);

    node_decl->color.Parse (decl, "color");
    node_decl->alpha.Parse (decl, "alpha");
    node_decl->mode.Parse (decl, "mode");
    node_decl->usage.Parse (decl, "usage");
    node_decl->up.Parse (decl, "up");

    node_decl->material = get<const char*> (decl, "material", "");

    if (ParseNode layout_node = decl.First ("layout"))
      node_decl->layout.reset (new stl::string (get<const char*> (layout_node, "")));

      //����������� ����������� ����

    cache.SetValue (decl, node_decl);    

    return node_decl;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Impl::PrepareSprite");
    throw;
  }  
}

void XmlSceneParser::Parse (const ParseNode& decl, Sprite& node, Node& parent, SceneContext& context)
{
  try
  {
      //��������������� ������

    SpriteDeclPtr node_decl = impl->PrepareSprite (decl);

      //��������� ����

    node.SetMaterial (node_decl->material.c_str ());

    if (node_decl->color.state)      node.SetColor (node_decl->color.value);
    if (node_decl->alpha.state)      node.SetAlpha (node_decl->alpha.value);
    if (node_decl->tex_offset.state) node.SetTexOffset (node_decl->tex_offset.value);
    if (node_decl->tex_size.state)   node.SetTexSize (node_decl->tex_size.value);
    if (node_decl->up.state)         node.SetOrtUp (node_decl->up.value);
    if (node_decl->mode.state)       node.SetMode (node_decl->mode.value);
    if (node_decl->usage.state)      node.SetUsage (node_decl->usage.value);

      //������ ������������ ����������
      
    Parse (decl, static_cast<Entity&> (node), parent, context);    
    
      //���������� �������
      
    if (node_decl->layout && !node_decl->layout->empty ())
    {
      common::PropertyMap* properties = node.Properties ();
      
      if (!properties)
        node.SetProperties (properties = new common::PropertyMap ());
        
      properties->SetProperty ("Layout", node_decl->layout->c_str ());
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,Sprite&,Node&,SceneContext&)");
    throw;
  }
}

TextLineDeclPtr XmlSceneParser::Impl::PrepareTextLine (const ParseNode& decl)
{
  try
  {
      //������� ����� ��������� � ����      

    if (TextLineDeclPtr* node_decl_ptr = cache.FindValue<TextLineDeclPtr> (decl))
      return *node_decl_ptr;
      
    TextLineDeclPtr node_decl (new TextLineDecl, false);
    
    node_decl->horizontal_alignment.Parse (decl, "horizontal_alignment");
    node_decl->vertical_alignment.Parse (decl, "vertical_alignment");
    node_decl->color.Parse (decl, "color");
    node_decl->alpha.Parse (decl, "alpha");

    node_decl->text = get<const char*> (decl, "text", "");
    node_decl->font = get<const char*> (decl, "font", "");        

      //����������� ����������� ����

    cache.SetValue (decl, node_decl);    

    return node_decl;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Impl::PrepareTextLine");
    throw;
  }  
}

void XmlSceneParser::Parse (const ParseNode& decl, TextLine& node, Node& parent, SceneContext& context)
{
  try
  {
      //��������������� ������
      
    TextLineDeclPtr node_decl = impl->PrepareTextLine (decl);
    
      //��������� ����
      
    node.SetTextUtf8 (node_decl->text.c_str ());
    node.SetFont (node_decl->font.c_str ());
    
    if (node_decl->horizontal_alignment.state) node.SetHorizontalAlignment (node_decl->horizontal_alignment.value);
    if (node_decl->vertical_alignment.state)   node.SetVerticalAlignment (node_decl->vertical_alignment.value);
    
    math::vec4f color = node.Color ();
    
    if (node_decl->color.state) color = node_decl->color.value;
    if (node_decl->alpha.state) color.w = node_decl->alpha.value;        
    
    node.SetColor (color);
      
      //������ ������������ ����������
    
    Parse (decl, static_cast<Entity&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,TextLine&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, Listener& node, Node& parent, SceneContext& context)
{
  try
  {
      //��������� ����
      
    node.SetGain (get<float> (decl, "gain", node.Gain ()));
    
      //������ ������������ ����������
    
    Parse (decl, static_cast<Entity&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,Listener&,Node&,SceneContext&)");
    throw;
  }
}

SoundEmitterDeclPtr XmlSceneParser::Impl::PrepareSoundEmitter (const ParseNode& decl)
{
  try
  {
      //������� ����� ��������� � ����      

    if (SoundEmitterDeclPtr* node_decl_ptr = cache.FindValue<SoundEmitterDeclPtr> (decl))
      return *node_decl_ptr;
      
    SoundEmitterDeclPtr node_decl (new SoundEmitterDecl, false);
    
    node_decl->source = get<const char*> (decl, "source", "");
    
    node_decl->gain.Parse (decl, "gain");

      //����������� ����������� ����

    cache.SetValue (decl, node_decl);    

    return node_decl;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Impl::PrepareSoundEmitter");
    throw;
  }  
}

template <> void XmlSceneParser::CreateNode<SoundEmitter> (const common::ParseNode& decl, Node& parent, SceneContext& context)
{
  try
  {
      //��������������� ������
      
    SoundEmitterDeclPtr node_decl = impl->PrepareSoundEmitter (decl);
    
      //��������� ����

    SoundEmitter::Pointer node = SoundEmitter::Create (node_decl->source.c_str ());
    
    if (node_decl->gain.state) node->SetGain (node_decl->gain.value);
    
      //������ ������������ ����������

    Parse (decl, static_cast<Entity&> (*node), parent, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::CreateNode<SoundEmitter>");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, SoundEmitter& node, Node& parent, SceneContext& context)
{
  try
  {    
      //��������� ����
      
    node.SetGain (get<float> (decl, "gain", node.Gain ()));
    
      //������ ������������ ����������
    
    Parse (decl, static_cast<Entity&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,SoundEmitter&,Node&,SceneContext&)");
    throw;
  }
}
