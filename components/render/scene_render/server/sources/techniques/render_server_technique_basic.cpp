#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    ����������� / ����������
*/

BasicTechnique::BasicTechnique (RenderManager& in_manager, const common::ParseNode& node)
  : manager (in_manager)
{
    //������ ����� �������

  effect_name = common::get<const char*> (node, "effect");
}    

BasicTechnique::~BasicTechnique ()
{
}

/*
    ���������� �����
*/

void BasicTechnique::UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder)
{
  try
  {
      //��������� ��������� ������ �������

    PrivateData& private_data = private_data_holder.Get<PrivateData> (*this);

      //���������� �����

    private_data.renderer->Draw (parent_context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::BasicTechnique::UpdateFrameCore");
    throw;
  }
}

/*
    ���������� ������� / ���������� ������� ������� � �������� �������
*/

void BasicTechnique::UpdatePropertiesCore ()
{
}

void BasicTechnique::BindProperties (common::PropertyBindingMap&)
{
}
