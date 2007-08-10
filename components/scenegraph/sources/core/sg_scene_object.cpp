#include "scene_object.h"

using namespace scene_graph;

/*
    ����������� / ����������
*/

SceneObject::SceneObject (scene_graph::Entity& in_entity)
  : entity (in_entity)
{
  prev_object = next_object = 0;
}

SceneObject::~SceneObject ()
{
  BindToSpace (0);
}

/*
    ���������� ������� � �������������
*/

void SceneObject::BindToSpace (SceneSpace* new_space)
{
    //�������� ������� �� ������������    
    
  if (space)
  {
      //�������� ������� �� ������� �������� ������������
    
    prev_object->next_object = next_object;
    next_object->prev_object = prev_object;
    
    if (this == space->first_object)
    {
      if (prev_object != next_object) space->first_object = next_object;
      else                            space->first_object = 0;
    }
    
    space->objects_count--;
  }
  
    //��������� ������ ������������
    
  space = new_space;  

  if (!new_space)
    return;
  
    //���������� ������� � ������������
    
  if (space->first_object)
  {
    next_object = space->first_object;  
    prev_object = next_object->prev_object;
    next_object->prev_object = prev_object->next_object = this;
  }
  else space->first_object = next_object = prev_object = this;
  
  space->objects_count++;
}
