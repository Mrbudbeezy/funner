#include "shared.h"

using namespace scene_graph;
using namespace common;

/*
    �������� ���������� �������
*/

typedef stl::hash_map<stl::hash_key<const char*>, ParseNode> IncludeMap;

struct XmlSceneParser::Impl
{
  ParseNode root;      //�������� ����
  SceneParserCache  cache;     //��� �������
  ResourceGroup     resources; //������� �����
  IncludeMap        includes;  //����� ������������ ������
  
///�����������
  Impl (const ParseNode& in_root) : root (in_root) {}
  
///��������������� ������
  void Prepare (const ParseNode& decl)
  {
    for (Parser::Iterator iter=decl.First (); iter; ++iter)
    {
      try
      {
        if (!strcmp (iter->Name (), "resource"))
        {
          ParseResourceNode (*iter);
        }
        else if (!strcmp (iter->Name (), "include"))
        {
          PrepareIncludeNode (*iter);
        }
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
  }

///������ ���� �������� ��������
  void ParseResourceNode (const ParseNode& decl)
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

    Parser parser (root.Log (), source_name, "xml");
    
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
    
      //���������� ������ ��������
      
    impl->Prepare (root);
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
    ��������� ������ ��������
*/

const ResourceGroup& XmlSceneParser::Resources ()
{
  return impl->resources;
}

/*
    �������� �����
*/

void XmlSceneParser::CreateScene (Node& parent, SceneContext& context, const LogHandler& log_handler)
{
  try
  {
    /////???????????????????
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::CreateScene");
    throw;
  }
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
    ������ ����
*/

void XmlSceneParser::ParseDispatch (const ParseNode& decl, Node& parent)
{
}

/*
    �������
*/

void XmlSceneParser::Parse (const ParseNode& decl, Node& parent, SceneContext& context, const LogHandler& log_handler)
{
}

void XmlSceneParser::Parse (const ParseNode& decl, Node& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const ParseNode& decl, Entity& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const ParseNode& decl, Camera& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const ParseNode& decl, OrthoCamera& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const ParseNode& decl, PerspectiveCamera& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const ParseNode& decl, Light& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const ParseNode& decl, DirectLight& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const ParseNode& decl, SpotLight& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const ParseNode& decl, PointLight& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const ParseNode& decl, VisualModel& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const ParseNode& decl, TextLine& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const ParseNode& decl, Sprite& node, Node& parent, SceneContext& context)
{
}
