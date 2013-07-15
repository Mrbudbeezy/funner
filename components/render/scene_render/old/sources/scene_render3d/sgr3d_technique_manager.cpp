#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

namespace
{

/*
    ���������� ����������� �������
*/

template <class T>
Technique* create_technique (RenderManager& manager, common::ParseNode& configuration)
{
  return new T (manager, configuration);
}

/*
    ������ ��������� ������
*/

typedef Technique* (*TechniqueCreator)(RenderManager& manager, common::ParseNode& configuration);

struct TechniqueDesc
{
  const char*      name;     //��� �������
  TechniqueCreator creator;  //����������� �������
};

TechniqueDesc technique_descs [] = {
  {"for_each_light", &create_technique<ForEachLightTechnique>}
};

const size_t techniques_count = sizeof (technique_descs) / sizeof (*technique_descs);

}

/*
    �������� �������
*/

TechniquePtr TechniqueManager::CreateTechnique (const char* technique, RenderManager& manager, common::ParseNode& configuration)
{
  try
  {
    if (!technique)
      throw xtl::make_null_argument_exception ("", "technique");
    
    for (size_t i=0; i<techniques_count; i++)
    {
      if (!technique_descs [i].name || !technique_descs [i].creator)
        continue;
      
      if (!xtl::xstrcmp (technique_descs [i].name, technique))
        return TechniquePtr (technique_descs [i].creator (manager, configuration), false);
    }
        
    throw xtl::format_operation_exception ("", "Can't create unknown technique '%s'", technique);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::TechniqueManager::CreateTechnique");
    throw;
  }
}
