#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

/*
    �������� ���������� ���������� ��������
*/

struct ResourceController::Impl: public xtl::instance_counter<ResourceController>
{
  media::rms::Binding binding; //���������� � ������� ��������
  
  Impl (media::rms::Binding& in_binding) 
    : binding (in_binding)
  {
  }
};

/*
    ����������� / ����������
*/

ResourceController::ResourceController (Node& node, media::rms::Binding& binding)
  : Controller (node, false)
  , impl (new Impl (binding))
{
  NodeOwnsController ();
}

ResourceController::~ResourceController ()
{
}

/*
    �������� �����������
*/

ResourceController::Pointer ResourceController::Create (Node& node, media::rms::Binding& binding)
{
  return Pointer (new ResourceController (node, binding), false);
}

/*
    ���������� � ������� ��������
*/

void ResourceController::SetBinding (media::rms::Binding& binding)
{
  impl->binding = binding;
}

media::rms::Binding& ResourceController::Binding () const
{
  return impl->binding;
}
