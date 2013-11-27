#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

//TODO: recursive rendering (move state to the technique)

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
      , frame (manager.Manager ().CreateFrame ())
      , entity_draw_handler (xtl::bind (&DrawWithoutLights::EntityDrawHandler, this, _1, _2, _3, _4))
    {
        //������� ��������� ������� ��� ��� frame-entity

      common::PropertyMap properties;

      properties.SetProperty ("ModelViewMatrix",           math::mat4f (1.0f));
      properties.SetProperty ("ModelViewProjectionMatrix", math::mat4f (1.0f));

      mv_matrix_property_index   = properties.IndexOf ("ModelViewMatrix");
      mvp_matrix_property_index  = properties.IndexOf ("ModelViewProjectionMatrix");      

      frame.SetInitialEntityDrawProperties (properties);

        //������� ��������� ������� �����

      frame_properties.SetProperty ("ViewMatrix", math::mat4f (1.0f));

      frame.SetProperties (frame_properties);

      view_matrix_property_index = frame_properties.IndexOf ("ViewMatrix");
    }    

/// ����� ��� �����������
    static const char* ClassName     () { return "draw_without_lights"; }
    static const char* ComponentName () { return "render.scene.server.techniques.draw_without_lights"; }

  private:
    struct PrivateData
    {
      size_t      view_transaction_id;
      size_t      view_proj_transaction_id;
      math::mat4f view_tm;
      math::mat4f view_proj_tm;

      PrivateData ()
        : view_transaction_id ()
        , view_proj_transaction_id ()
        , view_tm (1.0f)
        , view_proj_tm (1.0f)
      {
      }
    };
  
  private:
///���������� �����
    void UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder)
    {
      try
      {
          //��������� ��������� ������ �������

        PrivateData& private_data = private_data_holder.Get<PrivateData> ();

        if (private_data.view_proj_transaction_id != parent_context.Camera ().ViewProjectionMatrixTransactionId ())
        {
          private_data.view_proj_tm             = parent_context.Camera ().ViewProjectionMatrix ();
          private_data.view_proj_transaction_id = parent_context.Camera ().ViewProjectionMatrixTransactionId ();
        }

        if (private_data.view_transaction_id != parent_context.Camera ().ViewMatrixTransactionId ())
        {
          private_data.view_tm             = parent_context.Camera ().ViewMatrix ();
          private_data.view_transaction_id = parent_context.Camera ().ViewMatrixTransactionId ();

          frame_properties.SetProperty (view_matrix_property_index, parent_context.Camera ().ViewMatrix ());
        }

          //���������� ������ ������� �� ���������

        TraverseResult& result = parent_context.TraverseResult ();
        
          //���������� ��������������� ��������

        RenderingContext context (parent_context, frame);

        Technique::Draw (context, result.visual_models);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene_render3d::DrawWithoutLights::UpdateFrameCore");
        throw;
      }
    }

///���������� ��������� ��������
    void EntityDrawHandler (manager::Frame& frame, manager::Entity& entity, void* user_data, manager::EntityDrawParams& out_params)
    {
      common::PropertyMap& properties    = out_params.properties;
      PrivateData&         private_data  = *reinterpret_cast<PrivateData*> (user_data);
      VisualModel&         model         = *reinterpret_cast<VisualModel*> (entity.UserData ());

      if (!&private_data || !&model)
        return;

      const math::mat4f& object_tm = model.WorldMatrix ();
      
      out_params.mvp_matrix = private_data.view_proj_tm * object_tm;

      properties.SetProperty (mv_matrix_property_index,  private_data.view_tm * object_tm);
      properties.SetProperty (mvp_matrix_property_index, out_params.mvp_matrix);
    }

///���������� �������
    void UpdatePropertiesCore () {}

///���������� ������� ������� � �������� �������
    void BindProperties (common::PropertyBindingMap&) {}

  private:
    RenderManager                      manager;                    //�������� ����������
    manager::Frame                     frame;                      //����� �������
    manager::Frame::EntityDrawFunction entity_draw_handler;        //���������� ��������� ��������
    common::PropertyMap                frame_properties;           //�������� �����
    size_t                             mv_matrix_property_index;   //������ �������� ������� ModelView
    size_t                             mvp_matrix_property_index;  //������ �������� ������� ModelViewProjection
    size_t                             view_matrix_property_index; //������ �������� ������� View (� ��������� �����)
};

}

}

}

extern "C"
{

TechniqueComponentRegistrator<DrawWithoutLights> DrawWithoutLights;

}
