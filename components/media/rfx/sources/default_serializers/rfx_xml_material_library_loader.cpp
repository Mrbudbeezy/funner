#include "shared.h"

using namespace media::rfx;
using namespace common;

namespace
{

/*
    ���������
*/

const char* COMPONENT_NAME = "media.rfx.material.loaders.xmtl"; //��� ����������
const char* LOG_NAME       = COMPONENT_NAME;                    //��� ���������
const char* EXTENSION      = "xmtl";                            //���������� ������������ �������

/*
    ���������
*/

class XmtlLoader
{
  public:
    XmtlLoader (const char* in_name, MaterialLibrary& in_library)
      : parser (in_name, "xml")
      , log (parser.Log ())
      , properties_loader (log)
    {
      try
      {
          //�������� ���������
          
        for_each_child (parser.Root (), "MaterialLibrary", xtl::bind (&XmtlLoader::LoadLibrary, this, _1));
        
          //����� ��������� ��������
          
        Log out_log (LOG_NAME);
          
        for (size_t i=0; i<log.MessagesCount (); i++)
          out_log.Print (log.Message (i));
          
          //�������� ������
          
        if (log.HasErrors ())
        {
          for (size_t i=0; i<log.MessagesCount (); i++)
            switch (log.MessageType (i))
            {
              case ParseLogMessageType_FatalError:
              case ParseLogMessageType_Error:
                throw xtl::format_operation_exception ("", "%s\n    at parse '%s'", log.Message (i), in_name);
              default:
              case ParseLogMessageType_Warning:
                break;
            }
        }

          //������
                
        in_library.Swap (library);
      }
      catch (xtl::exception& e)
      {
        e.touch ("XmtlLoader::XmtlLoader");
        throw;
      }
    }
    
  private:
///�������� ���������� ����������
    void LoadLibrary (Parser::Iterator parse_iter)
    {
      for_each_child (*parse_iter, "Templates", xtl::bind (&XmtlLoader::LoadTemplates, this, _1));
      for_each_child (*parse_iter, "Materials", xtl::bind (&XmtlLoader::LoadMaterials, this, _1));
    }
    
///�������� ��������
    void LoadTemplates (Parser::Iterator parse_iter)
    {
      for_each_child (*parse_iter, "Properties", xtl::bind (&XmtlLoader::LoadPropertiesTemplate, this, _1));
    }
    
///�������� �������
    void LoadPropertiesTemplate (Parser::Iterator parse_iter)
    {
      properties_loader.LoadTemplate (parse_iter);
    }
    
///�������� �������
    void LoadProperties (Parser::Iterator parse_iter, PropertyMap& properties)
    {
      properties_loader.LoadProperties (parse_iter, properties);
    }
    
///�������� ����������
    void LoadMaterials (Parser::Iterator parse_iter)
    {
      for_each_child (*parse_iter, "Material", xtl::bind (&XmtlLoader::LoadMaterial, this, _1));
      for_each_child (*parse_iter, "InstanceMaterial", xtl::bind (&XmtlLoader::LoadInstanceMaterial, this, _1));
    }
    
///�������� ���������
    void LoadMaterial (Parser::Iterator parse_iter)
    {
      const char* id     = get<const char*> (*parse_iter, "Name");
      const char* effect = get<const char*> (*parse_iter, "Effect");
      
      Material material;
      
      material.SetName (id);
      material.SetEffect (effect);
      
      properties_loader.LoadProperties (parse_iter, material.Properties ());
      
      library.Attach (id, material);
    }

///���������� ���������
    void LoadInstanceMaterial (Parser::Iterator parse_iter)
    {
      const char* id     = get<const char*> (*parse_iter, "Name");
      const char* source = get<const char*> (*parse_iter, "Source");
      
      Material* material = library.Find (source);
      
      if (!material)
      {
        log.Error (*parse_iter, "Undefined material '%s' used", source);
        return;
      }

      library.Attach (id, *material);
    }

  private:
    MaterialLibrary  library;           //����������� ����������
    Parser           parser;            //������
    ParseLog         log;               //�������� ��������
    PropertiesLoader properties_loader; //��������� �������
};

/*
    ���������
*/

class Component
{
  public:
    Component ()
    {
      MaterialLibraryManager::RegisterLoader (EXTENSION, &LoadLibrary);
    }
    
  private:
    static void LoadLibrary (const char* name, MaterialLibrary& library)
    {
      XmtlLoader (name, library);
    }
};

}

extern "C"
{

ComponentRegistrator<Component> XmtlLoader (COMPONENT_NAME);

}
