#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    �������� ���������� ��������������� ������
*/

struct VisualModel::Impl
{
  scene_graph::VisualModel& model; //��������������� ������
  
///�����������
  Impl (scene_graph::VisualModel& in_model)
    : model (in_model)
  {
  }
};

/*
    ����������� / ����������
*/

VisualModel::VisualModel (Scene& scene, scene_graph::VisualModel& entity)
  : Renderable (scene, entity)
{
  try  
  {    
    impl = new Impl (entity);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::VisualModel::VisualModel");
    throw;
  }
}

VisualModel::~VisualModel ()
{
}

/*
    �����
*/

void VisualModel::VisitCore (IVisitor& visitor)
{
  visitor.Visit (*this);
}

/*
    ��������� � ����������
*/

void VisualModel::UpdateCore (Frame& frame)
{
}

void VisualModel::UpdateFrameCore (Frame& frame)
{
}
