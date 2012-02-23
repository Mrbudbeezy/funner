#ifndef SCENE_GRAPH_XSCENE_PARSER_SHARED_HEADER
#define SCENE_GRAPH_XSCENE_PARSER_SHARED_HEADER

#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>
#include <xtl/shared_ptr.h>

#include <common/component.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <media/rms/manager.h>

#include <sg/scene_manager.h>
#include <sg/scene_parser.h>

namespace scene_graph
{

typedef xtl::shared_ptr<ISceneParser> SceneParserPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� XML ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class XmlSceneFactory: public ISceneFactory, public xtl::reference_counter, public xtl::noncopyable
{
  public:
    typedef SceneManager::LogHandler LogHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    XmlSceneFactory  (const char* file_name, const LogHandler& log_handler);
    ~XmlSceneFactory ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool GetSceneInfo (const char* name, ResourceGroup* resources); //if resources != 0 && scene present -> fill resources needed for scene

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CreateScene (const char* name, Node& parent, SceneContext& scene_context);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

typedef xtl::intrusive_ptr<XmlSceneFactory> XmlSceneFactoryPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� �������� XML ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class XmlSceneParserManagerImpl
{
  public:
    typedef XmlSceneParserManager::SceneParserCreator SceneParserCreator;
    typedef SceneManager::LogHandler                  LogHandler;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterParser       (const char* version, const SceneParserCreator& parser);
    void UnregisterParser     (const char* version);
    void UnregisterAllParsers ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetDefaultVersion (const char* version);
    const char* DefaultVersion    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SceneParserPtr CreateParser (const common::ParseNode&);
    
  private:
    typedef stl::hash_map<stl::hash_key<const char*>, SceneParserCreator> CreatorMap;
    
  private:
    CreatorMap  creators;
    stl::string default_version;
};

typedef common::Singleton<XmlSceneParserManagerImpl> XmlSceneParserManagerSingleton;

}

#endif
