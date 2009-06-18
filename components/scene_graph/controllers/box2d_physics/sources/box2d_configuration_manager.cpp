#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::physics;
using namespace scene_graph::physics::box2d;

namespace
{

/*
    ���������� ��������� ������������
*/

class ConfigurationManagerImpl
{
  public:
///��������� ������������
    Configuration::Pointer GetConfiguration (const char* file_name)
    {
      try
      {
        if (!file_name)
          throw xtl::make_null_argument_exception ("", "file_name");
          
        ConfigurationMap::iterator iter = configurations.find (file_name);
        
        if (iter != configurations.end ())
          return iter->second; //������������ ������� � ����
          
          //�������� ����� ������������

        Configuration::Pointer configuration (new Configuration (file_name), false);

        configurations.insert_pair (file_name, configuration);

        return configuration;
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("scene_graph::physics::box2d::ConfigurationManagerImpl::GetConfiguration");
        throw;
      }
    }

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, Configuration::Pointer> ConfigurationMap;

  private:
    ConfigurationMap configurations; //����� ����������� ������������
};

typedef common::Singleton<ConfigurationManagerImpl> ConfigurationManagerSingleton;

}

/*
    �������� ������������
*/

Configuration::Pointer ConfigurationManager::GetConfiguration (const char* file_name)
{
  return ConfigurationManagerSingleton::Instance ()->GetConfiguration (file_name);
}
