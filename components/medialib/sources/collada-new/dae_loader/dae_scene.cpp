#include "shared.h"

using namespace math;

/*
    ������ ���������� ���������� ����
*/

void DaeParser::ParseLibraryVisualScenes (Parser::Iterator iter)
{
  if (!test (iter, "visual_scene"))
  {
    parse_log.Warning (iter, "Incorrect 'library_visual_scenes' node. Must be at least one 'visual_scene' sub-tag");
    return;
  }
  
  LogScope scope (iter, *this);

  for_each_child (iter, "visual_scene", bind (&DaeParser::ParseVisualScene, this, _1));
}

/*
    ������ ���������� �����
*/

void DaeParser::ParseVisualScene (Parser::Iterator iter)
{
  const char *id   = get<const char*> (iter, "id"),
             *name = get<const char*> (iter, "name");
  
  LogScope scope (iter, *this);
  
  if (!id)
  {
    LogError (iter, "No id");
    return;
  }

  Scene& scene = model.Scenes ().Create (id); 
  
  if (name)
    scene.SetName (name);
  
  for_each_child (iter, "node", bind (&DaeParser::ParseNode, this, _1, ref (scene)));
}

/*
    ������ ����
*/

void DaeParser::ParseNode (Parser::Iterator iter, Node& parent)
{
  const char *id   = get<const char*> (iter, "id"),
             *sid  = get<const char*> (iter, "sid"),
             *name = get<const char*> (iter, "name");
  
  LogScope scope (iter, *this);
  
  if (!id)
  {
    LogError (iter, "No id");
    return;
  }
  
    //������ �������������� ����
    
  mat4f tm;
   
  if (!ParseTransform (iter, tm))
  {
    LogError (iter, "Error at parse node transformation sub-tags");
    return;
  }
  
  Node& node = model.Nodes ().Create (id);
  
    //������� � ������������� ����
    
  parent.Nodes ().Insert (node);
  
    //��������� ��������������� ��������������

  if (sid)
    node.SetSubId (sid);
    
    //��������� ����� ����
    
  if (name)
    node.SetName (name);
    
    //��������� �������������� ����
 
  node.SetTransform (tm);
  
    //������ ���������������� ���������
    
  for_each_child (iter, "instance_geometry", bind (&DaeParser::ParseInstanceGeometry, this, _1, ref (node.Meshes ())));
  
    //������ ���������������� ���������� �����
    
  for_each_child (iter, "instance_light", bind (&DaeParser::ParseInstanceLight, this, _1, ref (node.Lights ())));
  
    //������ ���������������� �����
    
  for_each_child (iter, "instance_camera", bind (&DaeParser::ParseInstanceCamera, this, _1, ref (node.Cameras ())));

    //������ ��������� �����

  for_each_child (iter, "node", bind (&DaeParser::ParseNode, this, _1, ref (node)));  
}

/*
    ������ �������������� ����
*/

bool DaeParser::ParseTransform (Parser::Iterator iter, mat4f& tm)
{
  for (Parser::Iterator i=iter->First (); i; ++i)
  {
    if (test_tag (i, "matrix"))
    {
      mat4f sub_tm;
      
      if (!CheckedRead (i, "#text", sub_tm))
        return false;
        
      tm = sub_tm * tm;
    }
    else if (test_tag (i, "translate"))
    {
      vec3f offset;            
      
      if (!CheckedRead (i, "#text", offset))
        return false;        
        
      tm = translate (offset) * tm;
    }
    else if (test_tag (i, "rotate"))
    {
      vec4f r;
      
      if (!CheckedRead (i, "#text", r))
        return false;
        
      tm = rotatef (deg2rad (r.w), r.x, r.y, r.z) * tm;
    }
    else if (test_tag (i, "scale"))
    {
      vec3f s;      
      
      if (!CheckedRead (i, "#text", s))
        return false;        
        
      tm = scale (s) * tm;
    }
    else if (test_tag (i, "lookat"))
    {
      LogError (i, "Lookat transform doesn't supported");
      return false;
    }
    else if (test_tag (i, "skew"))
    {
      LogError (i, "Skew transform doesn't supported");
      return false;
    }
  }

  return true;
}

/*
    ������ ����������������� ��������� �����
*/

void DaeParser::ParseInstanceLight (Parser::Iterator iter, Node::LightList& lights)
{
  const char* url = get<const char*> (iter, "url");
  
  if (!url)
  {
    LogError (iter, "No url. Error at parser 'instance_light'");
    return;
  }
  
  url++; //����������� �� ���������� '#'
  
  Light* light = model.Lights ().Find (url);
  
  if (!light)
  {
    LogError (iter, "No light with id='%s' detected", url);
    return;
  }

  lights.Insert (*light);
}

/*
    ������ ���������������� ������
*/

void DaeParser::ParseInstanceCamera (Parser::Iterator iter, Node::CameraList& cameras)
{
  const char* url = get<const char*> (iter, "url");
  
  if (!url)
  {
    LogError (iter, "No url. Error at parser 'instance_camera'");
    return;
  }
  
  url++; //����������� �� ���������� '#'
  
  Camera* camera = model.Cameras ().Find (url);
  
  if (!camera)
  {
    LogError (iter, "No camera with id='%s' detected", url);
    return;
  }

  cameras.Insert (*camera);
}

/*
    ������ ���������������� ���������
*/

void DaeParser::ParseInstanceGeometry (Parser::Iterator iter, Node::MeshList& meshes)
{
  const char* url = get<const char*> (iter, "url");
  
  if (!url)
  {
    LogError (iter, "No url. Error at parser 'instance_geometry'");
    return;
  }
  
  url++; //����������� �� ���������� '#'
  
  Mesh* mesh = model.Meshes ().Find (url);
  
  if (!mesh)
  {
      //��� ����� ���� �� ���
//    LogError (iter, "No mesh with id='%s' detected", url);
    return;
  }

  InstanceMesh& imesh = meshes.Create (*mesh);
  
    //������ ������������ ����������
  
  for_each_child (iter, "bind_material", bind (&DaeParser::ParseBindMaterial, this, _1, ref (imesh.MaterialBinds ())));
}

/*
    ������ ������������� ���������
*/

void DaeParser::ParseBindMaterial (Parser::Iterator iter, MaterialBinds& binds)
{
  if (!test (iter, "technique_common"))
  {
    LogError (iter, "No 'technique_common' sub-tag");
    return;
  }
  
  for (Parser::NamesakeIterator i=iter->First ("technique_common.instance_material"); i; ++i)
  {
    const char *target = get<const char*> (i, "target"),
               *symbol = get<const char*> (i, "symbol");
    
    if (!target)
    {
      LogError (i, "No 'target' attribute");
      continue;
    }
    
    if (!symbol)
    {
      LogError (i, "No 'symbol' attribute");
      continue;
    }    
    
    target++; //����������� �� ���������� '#'
    
    Material* material = model.Materials ().Find (target);
    
    if (!material)
    {
      LogError (i, "No material with id='%s' detected", target);
      continue;
    }
    
      //��������� �������� � ������ ������������� ����������

    binds.SetMaterial (symbol, *material);

      //������ ���������� �������

    for (Parser::NamesakeIterator j=i->First ("bind_vertex_input"); j; ++j)
      if (test (j, "input_semantic", "TEXCOORD"))
      {
        const char *semantic = get<const char*> (j, "semantic"),
                   *set      = get<const char*> (j, "input_set");
                        
        if (!semantic)
        {
          LogError (j, "No 'semantic' attribute");
          continue;
        }
        
        if (!set)
        {
          LogError (j, "No 'input_set' attribute");
          continue;
        }

        binds.SetTexcoordChannelName (symbol, semantic, set);
      }
  }
}
