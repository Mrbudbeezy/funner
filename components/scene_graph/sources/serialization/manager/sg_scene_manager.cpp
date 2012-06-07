#include "shared.h"

using namespace scene_graph;

namespace
{

/*
    ���������
*/

const char* DEFAULT_LOADER_LOG_NAME = "scene.loader"; //��� ����

/*
    �������� ���������� ��������� ����
*/

typedef xtl::shared_ptr<ISceneFactory> FactoryPtr;

///���������� ������� ����
struct FactoryDesc: public xtl::reference_counter, public xtl::instance_counter<FactoryDesc>
{
  stl::string name;        //��� �������
  stl::string name_prefix; //������� �����
  FactoryPtr  factory;     //�������

///�����������
  FactoryDesc (const char* in_name, const char* in_name_prefix, const FactoryPtr& in_factory)
    : name (in_name)
    , name_prefix (in_name_prefix)
    , factory (in_factory)
  {
    if (!name_prefix.empty ())
      name_prefix += '.'; //names like prefix.name
  }
};

typedef xtl::intrusive_ptr<FactoryDesc>   FactoryDescPtr;
typedef stl::list<FactoryDescPtr>         FactoryDescList;
typedef xtl::signal<void (SceneContext&)> SceneContextCreatorSignal;

}

struct SceneManager::Impl: public xtl::reference_counter, public xtl::instance_counter<SceneManager>
{
  FactoryDescList           factories;           //������ ������
  SceneContextCreatorSignal scene_creator;       //������ �������� ������ ��������� �����
  common::Log               default_log;         //����� ���������������� �� ���������
  LogHandler                default_log_handler; //���������� ���������������� �� ���������
  
  Impl ()
    : default_log (DEFAULT_LOADER_LOG_NAME)
    , default_log_handler (xtl::bind (&Impl::Print, this, _1))
  {
  }
  
  void Print (const char* message)
  {
    default_log.Print (message);
  }
};

/*
    ������������ / ���������� / ������������
*/

SceneManager::SceneManager ()
  : impl (new Impl)
{
}

SceneManager::SceneManager (const SceneManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

SceneManager::~SceneManager ()
{
  release (impl);
}

SceneManager& SceneManager::operator = (const SceneManager& manager)
{
  SceneManager (manager).Swap (*this);
  
  return *this;
}

/*
    �������� ��������
*/

namespace
{

struct DefaultLogger
{
  DefaultLogger () : log (DEFAULT_LOADER_LOG_NAME) {}

  void operator () (const char* s) const
  {
    log.Printf (s);
  }
  
  common::Log log;
};

}

void SceneManager::LoadScene (const char* file_name, const char* name_prefix)
{
  LoadScene (file_name, name_prefix, LogHandler ());
}

void SceneManager::LoadScene (const char* file_name, const char* name_prefix, const LogHandler& in_log_handler)
{
  try
  {
      //�������� ������������ ����������
    
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
      
    if (!name_prefix)
      throw xtl::make_null_argument_exception ("", "name_prefix");
      
      //�������� ������� �����

    for (FactoryDescList::iterator iter=impl->factories.begin (), end=impl->factories.end (); iter!=end; ++iter)
    {
      FactoryDesc& desc = **iter;
      
      if (desc.name == file_name)
        throw xtl::format_operation_exception ("", "Scene '%s' has already loaded");
    }
    
      //�������� ������������ ����
      
    LogHandler log_handler = in_log_handler;
    
    if (!log_handler)
      log_handler = DefaultLogger ();    

      //�������� ����� �������
      
    static common::ComponentLoader loader ("scene.loaders.*");

    FactoryPtr     factory (SceneSerializationManagerImpl::GetLoader (file_name, common::SerializerFindMode_ByName)(file_name, log_handler));
    FactoryDescPtr desc (new FactoryDesc (file_name, name_prefix, factory), false);

      //����������� �������
      
    impl->factories.push_front (desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::SceneManager::LoadScene");
    throw;
  }
}

void SceneManager::UnloadScene (const char* file_name)
{
  if (!file_name)
    return;
    
  for (FactoryDescList::iterator iter=impl->factories.begin (), end=impl->factories.end (); iter!=end; ++iter)
  {
    FactoryDesc& desc = **iter;
    
    if (desc.name == file_name)
    {
      impl->factories.erase (iter);

      return;
    }
  }
}

/*
    �������� ������� �����
*/

bool SceneManager::HasScene (const char* scene_name) const
{
  if (!scene_name)
    return false;

  for (FactoryDescList::iterator iter=impl->factories.begin (), end=impl->factories.end (); iter!=end; ++iter)      
  {
    FactoryDesc& desc = **iter;

    try
    {
      if (desc.factory->GetSceneInfo (scene_name, 0))
        return true;
    }
    catch (...)
    {
    }
  }

  return false;
}

/*
    ��������� ������ �������� �����
*/

ResourceGroup SceneManager::SceneResources (const char* scene_name) const
{
  try
  {
    if (!scene_name)
      throw xtl::make_null_argument_exception ("", "scene_name");
      
    ResourceGroup resources;
    
    for (FactoryDescList::iterator iter=impl->factories.begin (), end=impl->factories.end (); iter!=end; ++iter)      
    {
      FactoryDesc& desc = **iter;

      if (desc.factory->GetSceneInfo (scene_name, &resources))
        return resources;
    }
    
    throw xtl::format_operation_exception ("", "Scene '%s' not found", scene_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::SceneManager::SceneResources");
    throw;
  }
}

namespace
{

struct ResourceSceneEnumerator
{
  ResourceGroup  resources;
  ISceneFactory* factory;
  
  ResourceSceneEnumerator () : factory () {}
  
  void operator () (const char* scene_name)
  {
    factory->GetSceneInfo (scene_name, &resources);
  }
};

}

ResourceGroup SceneManager::Resources () const
{
  try
  {
    ResourceSceneEnumerator        enumerator;
    ISceneFactory::SceneEnumerator enum_handler = xtl::ref (enumerator);
    
    for (FactoryDescList::iterator iter=impl->factories.begin (), end=impl->factories.end (); iter!=end; ++iter)      
    {
      FactoryDesc& desc = **iter;
      
      enumerator.factory = &*desc.factory;
      
      desc.factory->EnumScenes (enum_handler);
    }
    
    return enumerator.resources;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::SceneManager::Resources");
    throw;
  }
}

/*
    �������� ��������� �����
*/

SceneContext SceneManager::CreateSceneContext () const
{
  try
  {
    SceneContext context;
    
    context.SetLogHandler (impl->default_log_handler);
    
    impl->scene_creator (context);    
    
    return context;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::SceneManager::CreateSceneContext");
    throw;
  }
}

xtl::connection SceneManager::RegisterSceneContextCreator (const SceneContextCreator& creator)
{
  return impl->scene_creator.connect (creator);
}

/*
    �������� �����
*/

Node::Pointer SceneManager::CreateScene (const char* scene_name) const
{
  try
  {
    SceneContext context = CreateSceneContext ();

    return CreateScene (scene_name, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::SceneManager::CreateScene(const char*)");
    throw;
  }
}

Node::Pointer SceneManager::CreateScene (const char* scene_name, SceneContext& context) const
{
  try
  {
    if (!scene_name)
      throw xtl::make_null_argument_exception ("", "scene_name");
      
      //�������� ������������� ����
      
    Node::Pointer root = Node::Create ();
    
      //����� �����      

    for (FactoryDescList::iterator iter=impl->factories.begin (), end=impl->factories.end (); iter!=end; ++iter)      
    {
      FactoryDesc& desc = **iter;
      
      if (!desc.name_prefix.empty () && strncmp (scene_name, desc.name_prefix.c_str (), desc.name_prefix.size ()))
        continue;
        
      if (desc.factory->GetSceneInfo (scene_name + desc.name_prefix.size (), 0))
      {
        ResourceGroup resources;
        
        if (!desc.factory->GetSceneInfo (scene_name + desc.name_prefix.size (), &resources))
          continue;

          //�������� �����

        desc.factory->CreateScene (scene_name + desc.name_prefix.size (), *root, context);
        
        root->SetName (scene_name);

          //������������� ��������
          
        media::rms::Binding binding = media::rms::ResourceManager::CreateBinding (resources);
        
        binding.Load ();

        controllers::ResourceController::Pointer controller = controllers::ResourceController::Create (*root, binding);

        controller->NodeOwnsController ();

        return root;
      }
    }

    throw xtl::format_operation_exception ("", "Scene '%s' not found", scene_name);
  }  
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::SceneManager::CreateScene(const char*,SceneContext&)");
    throw;
  }
}

/*
    ���������� �����
*/

void SceneManager::SaveScene (const char* file_name, Node& node)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
      
    static common::ComponentLoader loader ("scene.savers.*");

    SceneSerializationManagerImpl::GetSaver (file_name, common::SerializerFindMode_ByName)(file_name, node);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::SceneManager::SaveScene");
    throw;
  }
}

/*
    �����
*/

void SceneManager::Swap (SceneManager& manager)
{
  stl::swap (impl, manager.impl);
}

namespace scene_graph
{

void swap (SceneManager& manager1, SceneManager& manager2)
{
  manager1.Swap (manager2);
}

}
