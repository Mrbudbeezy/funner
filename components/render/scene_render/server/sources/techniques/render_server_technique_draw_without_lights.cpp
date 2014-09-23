#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

namespace render
{

namespace scene
{

namespace server
{

///������� ����������: ��������� ����������� ��� ���������
class DrawWithoutLights: public Technique
{
  public:
///����������� / ����������
    DrawWithoutLights (RenderManager& in_manager, const common::ParseNode& node)
      : manager (in_manager)
    {
        //������ ����� �������

      effect_name = common::get<const char*> (node, "effect");
    }

/// ����� ��� �����������
    static const char* ClassName     () { return "draw_without_lights"; }
    static const char* ComponentName () { return "render.scene.server.techniques.draw_without_lights"; }

  protected:
///������ ��� ���������� ������� � ������ �����
    struct PrivateData
    {
      BasicRendererPtr renderer;

      PrivateData (DrawWithoutLights& technique) : renderer (new BasicRenderer (technique.manager, technique.effect_name.c_str ()), false) {}
    };

///���������� �����
    void UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder)
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
        e.touch ("render::scene::server::DrawWithoutLights::UpdateFrameCore");
        throw;
      }
    }

///���������� �������
    void UpdatePropertiesCore () {}

///���������� ������� ������� � �������� �������
    void BindProperties (common::PropertyBindingMap&) {}

  private:
    RenderManager manager;     //�������� ����������
    stl::string   effect_name; //��� �������
};

}

}

}

extern "C"
{

TechniqueComponentRegistrator<render::scene::server::DrawWithoutLights> DrawWithoutLights;

}
