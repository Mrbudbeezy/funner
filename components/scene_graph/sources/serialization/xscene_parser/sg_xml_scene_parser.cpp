#include "shared.h"

using namespace scene_graph;

/*
    �������� ���������� �������
*/

struct XmlSceneParser::Impl
{
  common::ParseNode root;      //�������� ����
  SceneParserCache  cache;     //��� �������
  ResourceGroup     resources; //������� �����
  
///�����������
  Impl (const common::ParseNode& in_root) : root (in_root) {}
  
///���������� ������ ��������
  void PrepareResources (const common::ParseNode& node)
  {
    //////???????????????
  }
};

/*
    ����������� / ����������
*/

XmlSceneParser::XmlSceneParser (const common::ParseNode& root)
{
  try
  { 
      //�������� ����������            
    
    impl = new Impl (root);
    
      //���������� ������ ��������
      
    impl->PrepareResources (root);
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

ISceneParser* XmlSceneParser::Create (const common::ParseNode& root)
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

const common::ParseNode& XmlSceneParser::Root () const
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

void XmlSceneParser::ParseDispatch (const common::ParseNode& decl, Node& parent)
{
}

/*
    �������
*/

void XmlSceneParser::Parse (const common::ParseNode& decl, Node& parent, SceneContext& context, const LogHandler& log_handler)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, Node& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, Entity& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, Camera& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, OrthoCamera& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, PerspectiveCamera& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, Light& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, DirectLight& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, SpotLight& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, PointLight& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, VisualModel& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, TextLine& node, Node& parent, SceneContext& context)
{
}

void XmlSceneParser::Parse (const common::ParseNode& decl, Sprite& node, Node& parent, SceneContext& context)
{
}
