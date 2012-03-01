#include "scene_graph.h"

using namespace scene_graph;

namespace
{

/*
    ���������
*/

const char* SCENE_MANAGER_LIBRARY = "Scene.SceneManager";

/*
    ��������������� �������
*/

SceneManager create_scene_manager ()
{
  return SceneManager ();
}

}

namespace engine
{

namespace scene_graph_script_binds
{

void bind_scene_manager_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_MANAGER_LIBRARY);
  
    //����������� ����������� �������
    
  lib.Register ("Create", make_invoker (&create_scene_manager));

    //����������� ��������  
    
  lib.Register ("LoadScene", make_invoker (
    make_invoker<void (SceneManager&, const char*, const char*, const SceneManager::LogHandler&)> (xtl::bind (xtl::implicit_cast<void (SceneManager::*)(const char*, const char*, const SceneManager::LogHandler&)> (&SceneManager::LoadScene), _1, _2, _3, _4)),
    make_invoker<void (SceneManager&, const char*, const char*)> (xtl::bind (xtl::implicit_cast<void (SceneManager::*)(const char*, const char*)> (&SceneManager::LoadScene), _1, _2, _3)),
    make_invoker<void (SceneManager&, const char*)> (xtl::bind (xtl::implicit_cast<void (SceneManager::*)(const char*, const char*)> (&SceneManager::LoadScene), _1, _2, ""))
  ));
  lib.Register ("UnloadScene", make_invoker (&SceneManager::UnloadScene));
  lib.Register ("SaveScene", make_invoker (&SceneManager::SaveScene));  
  lib.Register ("HasScene", make_invoker (&SceneManager::HasScene));
  lib.Register ("CreateScene", make_invoker (xtl::implicit_cast<Node::Pointer (SceneManager::*)(const char*) const> (&SceneManager::CreateScene)));
  lib.Register ("CreateLogHandler", make_callback_invoker<SceneManager::LogHandler::signature_type> ());
    
    //����������� ���� ������

  environment.RegisterType<SceneManager> (SCENE_MANAGER_LIBRARY);
}

}

}
