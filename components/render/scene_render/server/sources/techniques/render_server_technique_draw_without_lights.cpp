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
      , entity_draw_handler (xtl::bind (&DrawWithoutLights::EntityDrawHandler, this, _1, _2, _3))
      , properties_update_counter (1)
    {
      SetDefaultProperties (node);
    }    

/// ����� ��� �����������
    static const char* ClassName     () { return "draw_without_lights"; }
    static const char* ComponentName () { return "render.scene.server.techniques.draw_without_lights"; }

  private:
    struct PrivateData
    {
      size_t properties_update_counter;

      PrivateData ()
        : properties_update_counter ()
      {
      }
    };

  
  private:
///���������� �����
    void UpdateFrameCore (RenderingContext& context, TechniquePrivateData& private_data_holder)
    {
      try
      {
          //��������� ��������� ������ �������

        PrivateData& private_data = private_data_holder.Get<PrivateData> ();

        if (private_data.properties_update_counter != properties_update_counter)
        {
            //TODO: update properties

          private_data.properties_update_counter = properties_update_counter;
        }

          //���������� ������ ������� �� ���������

        TraverseResult& result = context.TraverseResult ();

          //��������� ���������� ������

        //TODO: configure frame
        
          //���������� ��������������� ��������

        Technique::Draw (context, result.visual_models);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene_render3d::DrawWithoutLights::UpdateFrameCore");
        throw;
      }
    }

///���������� ��������� ��������
    void EntityDrawHandler (manager::Frame& frame, manager::Entity& entity, manager::EntityDrawParams& out_params)
    {
//    common::PropertyMap& properties = out_params.properties;
      
//    math::mat4f model_view_tm = frame.Properties ().GetMatrix ("ViewMatrix") * entity.Properties ().GetMatrix ("ObjectMatrix");  

//    out_params.mvp_matrix = frame.Properties ().GetMatrix ("ProjectionMatrix") * model_view_tm;

//    properties.SetProperty ("ModelViewMatrix", model_view_tm);
//    properties.SetProperty ("ModelViewProjectionMatrix", out_params.mvp_matrix);    
    }

///���������� �������
    void UpdatePropertiesCore ()
    {
      properties_update_counter++;
    }

///���������� ������� ������� � �������� �������
    void BindProperties (common::PropertyBindingMap& map)
    {
      //TODO: bind properties
    }

  private:
    RenderManager                      manager;                   //�������� ����������
    manager::Frame::EntityDrawFunction entity_draw_handler;       //���������� ��������� ��������
    size_t                             properties_update_counter; //������� ���������� �������
};

}

}

}

extern "C"
{

TechniqueComponentRegistrator<DrawWithoutLights> DrawWithoutLights;

}
