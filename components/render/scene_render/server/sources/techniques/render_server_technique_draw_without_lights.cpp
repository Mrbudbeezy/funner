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
    {
        //������ ����� �������

      effect_name = common::get<const char*> (node, "effect");

        //������� ��������� ������� �����

      frame_properties.SetProperty ("ViewMatrix", math::mat4f (1.0f));
      frame_properties.SetProperty ("ProjectionMatrix", math::mat4f (1.0f));

      view_matrix_property_index = frame_properties.IndexOf ("ViewMatrix");
      proj_matrix_property_index = frame_properties.IndexOf ("ProjectionMatrix");
    }    

/// ����� ��� �����������
    static const char* ClassName     () { return "draw_without_lights"; }
    static const char* ComponentName () { return "render.scene.server.techniques.draw_without_lights"; }

  private:
    struct PrivateData
    {
      manager::Frame  frame;                      //�����
      size_t          view_transaction_id;        //������� ���������� ������� ����
      size_t          proj_transaction_id;        //������� ���������� ������� ��������
      size_t          view_proj_transaction_id;   //������� ���������� ������� ���-��������
      math::mat4f     view_tm;                    //������� ����
      math::mat4f     view_proj_tm;               //������� ���-��������

      PrivateData (DrawWithoutLights& technique)
        : frame (technique.CreateFrame ())
        , view_transaction_id ()
        , proj_transaction_id ()
        , view_proj_transaction_id ()
        , view_tm (1.0f)
        , view_proj_tm (1.0f)
      {
      }
    };

/// �������� �����
    manager::Frame CreateFrame ()
    {
      try
      {
        manager::Frame frame = manager.Manager ().CreateFrame ();

          //������� �������
   
        frame.SetEffect (effect_name.c_str ());

          //������� ��������� ������� ��� ��� frame-entity

        common::PropertyMap properties;

        properties.SetProperty ("ModelViewMatrix",           math::mat4f (1.0f));
        properties.SetProperty ("ModelViewProjectionMatrix", math::mat4f (1.0f));
        properties.SetProperty ("ObjectMatrix",              math::mat4f (1.0f));

        mv_matrix_property_index     = properties.IndexOf ("ModelViewMatrix");
        mvp_matrix_property_index    = properties.IndexOf ("ModelViewProjectionMatrix");
        object_matrix_property_index = properties.IndexOf ("ObjectMatrix");

        frame.SetEntityDependentProperties (properties);
        frame.SetEntityDrawHandler (xtl::bind (&DrawWithoutLights::EntityDrawHandler, this, _1, _2, _3, _4));

          //������� ������� ������

        frame.SetProperties (frame_properties);

        return frame;
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::server::DrawWithoutLights::CreateFrame");
        throw;
      }
    }
  
  private:
///���������� �����
    void UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder)
    {
      try
      {
          //��������� ��������� ������ �������

        PrivateData& private_data = private_data_holder.Get<PrivateData> (*this);

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

        if (private_data.proj_transaction_id != parent_context.Camera ().ProjectionMatrixTransactionId ())
        {
          private_data.proj_transaction_id = parent_context.Camera ().ProjectionMatrixTransactionId ();

          frame_properties.SetProperty (proj_matrix_property_index, parent_context.Camera ().ProjectionMatrix ());
        }

          //���������� ������ ������� �� ���������

        TraverseResult& result = parent_context.TraverseResult ();
        
          //���������� ��������������� ��������

        RenderingContext context (parent_context, private_data.frame);

        Technique::Draw (context, result.visual_models, &private_data);

          //���������� ��������� �����

        parent_context.Frame ().AddFrame (private_data.frame);
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
      properties.SetProperty (object_matrix_property_index, object_tm);
    }

///���������� �������
    void UpdatePropertiesCore () {}

///���������� ������� ������� � �������� �������
    void BindProperties (common::PropertyBindingMap&) {}

  private:
    RenderManager       manager;                      //�������� ����������
    stl::string         effect_name;                  //��� �������
    common::PropertyMap frame_properties;             //�������� �����
    size_t              mv_matrix_property_index;     //������ �������� ������� ModelView
    size_t              mvp_matrix_property_index;    //������ �������� ������� ModelViewProjection
    size_t              object_matrix_property_index; //������ �������� ������� ObjectMatrix
    size_t              view_matrix_property_index;   //������ �������� ������� View (� ��������� �����)
    size_t              proj_matrix_property_index;   //������ �������� ������� Projection (� �������� �����)
};

}

}

}

extern "C"
{

TechniqueComponentRegistrator<render::scene::server::DrawWithoutLights> DrawWithoutLights;

}
