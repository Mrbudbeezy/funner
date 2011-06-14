#include "shared.h"

namespace
{

/*
    ���������
*/

const char* SUPPORTED_VERSION = "1.4.1"; //����� �������������� ������ ������������ COLLADA

}

/*
    ����������� / ����������
*/

DaeParser::DaeParser (const char* file_name, Model& in_model, const LogHandler& log)
  : model (in_model), parser (file_name)
{
  try
  {
    ParseRoot (parser.Root ().First ("COLLADA"));
  }
  catch (...)
  {
    PrintLog (log);
    throw;
  }
  
  PrintLog (log);  
}

DaeParser::~DaeParser ()
{
}

/*
    ������ ������
*/

void DaeParser::PrintLog (const LogHandler& log_fn)
{
  const ParseLog& log = parser.Log ();

  try
  {  
    for (size_t i=0; i<log.MessagesCount (); ++i)
      log_fn (log.Message (i));
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}

/*
    �������� � ����� ���� ��������� ��������
*/

VertexIndexMapPtr DaeParser::GetVertexIndicesMap (const char* mesh_id, size_t surface_index)
{
  if (!mesh_id)
    return 0;

  stl::string surface_name = common::format ("%s#%u", mesh_id, surface_index);

    //������� ������ ����� ��������� ��������

  VertexIndexMaps::iterator iter = vertex_index_maps.find (surface_name.c_str ());

  if (iter != vertex_index_maps.end ())
    return iter->second;

    //�������� ����� ��������� ��������

  VertexIndexMapPtr map (new VertexIndexMap, false);

    //����������� ����� ��������� ��������

  vertex_index_maps.insert_pair (surface_name.c_str (), map);

  return map;
}

/*
    ������ ��������� ����
*/

void DaeParser::ParseRoot (Parser::Iterator iter)
{
  if (!iter)
    raise_parser_exception (parser.Root (), "Wrong file format. No 'COLLADA' tag");

  const char* version = get<const char*> (*iter, "version");

  if (strcmp (version, SUPPORTED_VERSION))
    iter->Log ().Warning (*iter, "Currently supported Collada version - %s, document uses other version (%s), "
      "some features may be not fully supported or unsupported", SUPPORTED_VERSION, version);

    //������ ���������� �������� �����
  common::ParseNode asset_node = iter->First ("asset");

  if (!asset_node)
    raise_parser_exception (*iter, "Wrong file format. No 'asset' tag");

  model.SetUnitOfMeasure (get<float> (asset_node, "unit.meter", 1.f));

  const char* up_axis_name = get<const char*> (asset_node, "up_axis.#text", "Y_UP");

  if (!xtl::xstrcmp (up_axis_name, "Y_UP"))
    up_axis = Axis_Y;
  else if (!xtl::xstrcmp (up_axis_name, "Z_UP"))
    up_axis = Axis_Z;
  else
    raise_parser_exception (asset_node, "Unsupported up axis value '%s'", up_axis_name);

  common::ParseNode authoring_tool_node = asset_node.First ("contributor.authoring_tool.#text");

  if (authoring_tool_node)
  {
    for (size_t i = 0, count = authoring_tool_node.AttributesCount (); i < count; i++)
    {
      authoring_tool += authoring_tool_node.Attribute (i);

      if (i < count - 1)
        authoring_tool += " ";
    }
  }

    //������ ���������

  ParseLibraries (iter);

    //����������� ����� �������� �����

  const char* active_scene_name = get<const char*> (*iter, "scene.instance_visual_scene.url", "");
  
  if (*active_scene_name)
  {
    active_scene_name++; //����������� �� ����������� '#'
    
    model.SetActiveSceneName (active_scene_name);
  }
  else
  {
    iter->Log ().Warning (*iter, "No 'scene.instance_visual_scene.url' detected");
  }
}

/*
    ������ ���������
*/

void DaeParser::ParseLibraries (Parser::Iterator iter)
{
  for_each_child (*iter, "library_images", bind (&DaeParser::ParseLibraryImages, this, _1));
  for_each_child (*iter, "library_effects", bind (&DaeParser::ParseLibraryEffects, this, _1));
  for_each_child (*iter, "library_materials", bind (&DaeParser::ParseLibraryMaterials, this, _1));
  for_each_child (*iter, "library_geometries", bind (&DaeParser::ParseLibraryGeometries, this, _1));
  for_each_child (*iter, "library_controllers", bind (&DaeParser::ParseLibraryControllers, this, _1));
  for_each_child (*iter, "library_lights", bind (&DaeParser::ParseLibraryLights, this, _1));
  for_each_child (*iter, "library_cameras", bind (&DaeParser::ParseLibraryCameras, this, _1));
  for_each_child (*iter, "library_visual_scenes", bind (&DaeParser::ParseLibraryVisualScenes, this, _1));
  for_each_child (*iter, "library_animations", bind (&DaeParser::ParseLibraryAnimations, this, _1));
}
