#include "shared.h"

using namespace math;

namespace
{

const char* UNNAMED_NODE_FORMAT = "__unnamed_node_%u__";

}

/*
    ������ ���������� ���������� ����
*/

void DaeParser::ParseLibraryVisualScenes (Parser::Iterator iter)
{
  if (!iter->First ("visual_scene"))
  {
    iter->Log ().Warning (*iter, "Empty 'library_visual_scenes' node. Must be at least one 'visual_scene' sub-tag");
    return;
  }

  for_each_child (*iter, "visual_scene", bind (&DaeParser::ParseVisualScene, this, _1));
}

/*
    ������ ���������� �����
*/

void DaeParser::ParseVisualScene (Parser::Iterator iter)
{
  const char *id   = get<const char*> (*iter, "id"),
             *name = get<const char*> (*iter, "name", "");

    //�������� �����

  Node scene;
  
  scene.SetId (id);  

  if (up_axis == Axis_Z)
  {
    float z_to_y_transform_matrix [] = { 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f };

    scene.SetTransform (math::mat4f (z_to_y_transform_matrix));
  }
  
  if (*name)
    scene.SetName (name);
  
  for_each_child (*iter, "node", bind (&DaeParser::ParseNode, this, _1, ref (scene)));
  
    //���������� ����� � ����������
    
  model.Scenes ().Insert (id, scene);
}

/*
    ������ ����
*/

void DaeParser::ParseNode (Parser::Iterator iter, Node& parent)
{
  const char *sid  = get<const char*> (*iter, "sid", ""),
             *name = get<const char*> (*iter, "name", "");
  
  stl::string id;

  if (iter->First ("id"))
    id = get<const char*> (*iter, "id");
  else
  {
    static size_t unnamed_node_index = 0;

    id = common::format (UNNAMED_NODE_FORMAT, unnamed_node_index++);
  }

    //�������� ����
  
  Node node;
  
  node.SetId (id.c_str ());

    //������ �������������� ����

  ParseTransform (iter, id.c_str (), node);
  
    //������� � ������������� ����
    
  parent.Nodes ().Add (node);
  
    //��������� ��������������� ��������������

  if (*sid)
    node.SetSubId (sid);
    
    //��������� ����� ����
    
  if (*name)
    node.SetName (name);    
  
    //���������� ���� � ����������
    
  model.Nodes ().Insert (id.c_str (), node);
  
    //������ ���������������� ���������
    
  for_each_child (*iter, "instance_geometry", bind (&DaeParser::ParseInstanceGeometry, this, _1, ref (node.Meshes ())));
  
    //������ ���������������� ������������
    
  for_each_child (*iter, "instance_controller", bind (&DaeParser::ParseInstanceController, this, _1, ref (node.Controllers ())));
  
    //������ ���������������� ���������� �����
    
  for_each_child (*iter, "instance_light", bind (&DaeParser::ParseInstanceLight, this, _1, ref (node.Lights ())));
  
    //������ ���������������� �����
    
  for_each_child (*iter, "instance_camera", bind (&DaeParser::ParseInstanceCamera, this, _1, ref (node.Cameras ())));

    //������ ��������� �����

  for_each_child (*iter, "node", bind (&DaeParser::ParseNode, this, _1, ref (node)));  

    //������ ���������������� �������

  for (Parser::NamesakeIterator extra_technique_iter = iter->First ("extra.technique"); extra_technique_iter; ++extra_technique_iter)
  {
    if (!xtl::xstrcmp (common::get <const char*> (*extra_technique_iter, "profile"), "OpenCOLLADA"))
    {
      Parser::Iterator user_properties_iter = extra_technique_iter->First ("user_properties.#text");

      if (user_properties_iter)
      {
        stl::string properties_string;

        for (size_t i = 0, count = user_properties_iter->AttributesCount (); i < count; i++)
        {
          properties_string.append (user_properties_iter->Attribute (i));

          if (i != count - 1)
            properties_string.append (" ");
        }

        node.SetUserProperties (properties_string.c_str ());
      }
    }
  }
}

/*
    ������ �������������� ����
*/

void DaeParser::ParseTransform (Parser::Iterator iter, const char* node_id, Node& node)
{  
  math::mat4f tm;

  for (Parser::Iterator i=iter->First (); i; ++i)
  {
    const char* name = i->Name ();
    
    if (!strcmp (name, "matrix"))
    {
      mat4f sub_tm;

      read (*i, "#text", sub_tm);

      tm = tm * sub_tm;

      //��������� ������ �� ������� � ����� �������� ��������
      const char* sid = get<const char*> (*i, "sid", "");

      if (xtl::xstrlen (sid))
      {
        stl::string target_name (common::format ("%s/%s", node_id, sid));

        animation_semantics.insert_pair (target_name.c_str (), AnimationChannelSemantic_Transform);
      }
    }
    else if (!strcmp (name, "translate"))
    {            
      vec3f offset = get<vec3f> (*i, "#text");

      tm = tm * translate (offset);
      
      const char* sid = get<const char*> (*i, "sid", "");            

      if (!strcmp (sid, "rotatePivot"))
      {
        node.SetRotationPivot (offset);
      }
      else if (!strcmp (sid, "scalePivot"))
      {
        node.SetScalePivot (offset);
      }
    }
    else if (!strcmp (name, "rotate"))
    {
      vec4f r = get<vec4f> (*i, "#text");

      tm = tm * rotate (degree (r.w), vec3f (r.x, r.y, r.z));
    }
    else if (!strcmp (name, "scale"))
    {
      vec3f s = get<vec3f> (*i, "#text");

      tm = tm * scale (s);
    }
    else if (!strcmp (name, "lookat"))
    {
      iter->Log ().Warning (*i, "Lookat transform doesn't supported");
    }
    else if (!strcmp (name, "skew"))
    {
      iter->Log ().Warning (*i, "Skew transform doesn't supported");
    }
  }

    //��������� �������������� ����
 
  node.SetTransform (tm);  
}

/*
    ������ ����������������� ��������� �����
*/

void DaeParser::ParseInstanceLight (Parser::Iterator iter, Node::LightList& lights)
{
  const char* url = get<const char*> (*iter, "url");  

  url++; //����������� �� ���������� '#'

  Light* light = model.Lights ().Find (url);

  if (!light)
    raise_parser_exception (*iter, "No light with id='%s' detected", url);

    //���������� ��������� ����� � ��������� ����

  lights.Add (*light);
}

/*
    ������ ���������������� ������
*/

void DaeParser::ParseInstanceCamera (Parser::Iterator iter, Node::CameraList& cameras)
{
  const char* url = get<const char*> (*iter, "url");

  url++; //����������� �� ���������� '#'

  Camera* camera = model.Cameras ().Find (url);
  
  if (!camera)
    raise_parser_exception (*iter, "No camera with id='%s' detected", url);

    //���������� ������ � ��������� ����

  cameras.Add (*camera);
}

/*
    ������ ���������������� ���������
*/

void DaeParser::ParseInstanceGeometry (Parser::Iterator iter, Node::MeshList& meshes)
{
  const char* url = get<const char*> (*iter, "url");  

  url++; //����������� �� ���������� '#'

  Mesh* mesh = model.Meshes ().Find (url);

  if (!mesh)
    return;

  InstanceMesh imesh;

  imesh.SetMesh (url);

    //������ ������������ ����������
  
  for_each_child (*iter, "bind_material", bind (&DaeParser::ParseBindMaterial, this, _1, ref (imesh.MaterialBinds ())));
  
    //���������� ���� � ��������� ����
    
  meshes.Add (imesh);
}

/*
    ������ ����������������� �����������
*/

void DaeParser::ParseInstanceController (Parser::Iterator iter, Node::ControllerList& controllers)
{
  const char* url = get<const char*> (*iter, "url");

  url++; //����������� �� ���������� '#'

  Skin*  skin  = model.Skins ().Find (url);
  Morph* morph = model.Morphs ().Find (url);

  if (!skin && !morph)
    raise_parser_exception (*iter, "No controller with id='%s' detected", url);

  InstanceController icontroller;
  
  icontroller.SetController (url);
  
    //������ ������������ ����������
    
  for_each_child (*iter, "bind_material", bind (&DaeParser::ParseBindMaterial, this, _1, ref (icontroller.MaterialBinds ())));
  
    //������ ������ ������ ����������
    
  for (Parser::NamesakeIterator skeleton_iter=iter->First ("skeleton"); skeleton_iter; ++skeleton_iter)
  {
    const char* root_id = get<const char*> (*skeleton_iter, "#text");

    icontroller.InsertJointSearchRoot (root_id);
  }  

    //���������� ����������� � ��������� ����

  controllers.Add (icontroller);
}

/*
    ������ ������������� ���������
*/

void DaeParser::ParseBindMaterial (Parser::Iterator iter, MaterialBinds& binds)
{
  common::ParseNode technique_common_node = get_first_child (*iter, "technique_common");
  
  for (Parser::NamesakeIterator i=technique_common_node.First ("instance_material"); i; ++i)
  {
    const char *target = get<const char*> (*i, "target"),
               *symbol = get<const char*> (*i, "symbol");

    target++; //����������� �� ���������� '#'

    Material* material = model.Materials ().Find (target);
    
    if (!material)
      raise_parser_exception (*i, "No material with id='%s' detected", target);

      //��������� �������� � ������ ������������� ����������

    binds.SetMaterial (symbol, target);

      //������ ���������� �������

    for (Parser::NamesakeIterator j=i->First ("bind_vertex_input"); j; ++j)
    {
      const char* input_semantic = get<const char*> (*j, "input_semantic", "");
      
      if (*input_semantic && !strcmp (input_semantic, "TEXCOORD"))
      {
        const char *semantic = get<const char*> (*j, "semantic"),
                   *set      = get<const char*> (*j, "input_set");

        binds.SetTexcoordChannelName (symbol, semantic, set);
      }
    }
  }
}
