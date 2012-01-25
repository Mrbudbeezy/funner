#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

/*
    �������� ���������� ����������� ����
*/

struct DefaultController::Impl
{
  UpdateFunction updater; //������� ���������� �����������
  
  Impl (const UpdateFunction& in_updater) : updater (in_updater) {}
};

/*
    ����������� / ����������
*/

DefaultController::DefaultController (Node& node, const UpdateFunction& updater)
  : Controller (node)
  , impl (new Impl (updater))
{
  NodeOwnsController ();
}

DefaultController::~DefaultController ()
{
}

/*
    �������� �����������
*/

DefaultController::Pointer DefaultController::Create (Node& node)
{
  return Pointer (new DefaultController (node, UpdateFunction ()), false);
}

DefaultController::Pointer DefaultController::Create (Node& node, const UpdateFunction& updater)
{
  return Pointer (new DefaultController (node, updater), false);
}

/*
    ����������
*/

void DefaultController::Update (const TimeValue& time)
{
  if (impl->updater)
    impl->updater (time);
}
