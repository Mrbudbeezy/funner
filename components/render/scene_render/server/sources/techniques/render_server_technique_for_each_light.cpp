#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

//TODO: recursive rendering!!
//TODO: use code from DrawWithoutLightsTechnique (ForEachLightTechnique is not ready)

namespace render
{

namespace scene
{

namespace server
{

namespace
{

/*
    ���������
*/

const size_t LIGHTS_RESERVE_SIZE = 64;   //������������� ���������� ���������� �����

}

///������� ����������: ��������� ����������� �� ������� ��������� � �������� ���������� �����������
class ForEachLightTechnique: public Technique
{
  public:
///����������� / ����������
    ForEachLightTechnique (RenderManager& in_manager, const common::ParseNode& node)
      : manager (in_manager)
      , entity_draw_handler (xtl::bind (&ForEachLightTechnique::EntityDrawHandler, this, _1, _2, _3, _4))
      , properties_update_counter (1)
    {
      light_frames.reserve (LIGHTS_RESERVE_SIZE);    

      SetDefaultProperties (node);
    }    

/// ����� ��� �����������
    static const char* ClassName     () { return "for_each_light"; }
    static const char* ComponentName () { return "render.scene.server.techniques.for_each_light"; }

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

          //����������� ������ ���������� � ��������������� ��������, ���������� � ������
        
        TraverseResult& result = context.TraverseResult ();

          //����� ����������        
          
        size_t light_index = 0;

        for (TraverseResult::LightArray::iterator iter=result.lights.begin (), end=result.lights.end (); iter!=end; ++iter, ++light_index)
          UpdateFromLight (context, **iter, result, light_index);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene_render3d::ForEachLightTechnique::UpdateFrameCore");
        throw;
      }
    }

///���������� ����� �� ��������� �����
    void UpdateFromLight (RenderingContext& context, Light& light, TraverseResult& result, size_t light_index)
    {
        //�������������� ���������� ������
        
      manager::Frame& frame = AllocateFrame (light_index);

        //��������� ���������� ���������� ������
        
      //TODO: configure frame
      
        //���������� ��������������� ��������

      Technique::Draw (context, result.visual_models);

        //���������� ���������� ������ � ������������

      context.Frame ().AddFrame (frame);
    }

///���������� ��������� ��������
    void EntityDrawHandler (manager::Frame& frame, manager::Entity& entity, void* user_data, manager::EntityDrawParams& out_params)
    {
//    common::PropertyMap& properties = out_params.properties;
      
//    math::mat4f model_view_tm = frame.Properties ().GetMatrix ("ViewMatrix") * entity.Properties ().GetMatrix ("ObjectMatrix");  

//    out_params.mvp_matrix = frame.Properties ().GetMatrix ("ProjectionMatrix") * model_view_tm;

//    properties.SetProperty ("ModelViewMatrix", model_view_tm);
//    properties.SetProperty ("ModelViewProjectionMatrix", out_params.mvp_matrix);    
    }

///�������������� ������
    manager::Frame& AllocateFrame (size_t light_index)
    {
      if (light_index < light_frames.size ())
        return light_frames [light_index];    
      
      manager::Frame frame = manager.Manager ().CreateFrame ();
      
      frame.SetEntityDrawHandler (entity_draw_handler);
        
      light_frames.push_back (frame);
        
      return light_frames.back ();
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
    typedef stl::vector<manager::Frame> FrameArray;

  private:
    RenderManager                      manager;                   //�������� ����������
    FrameArray                         light_frames;              //������ ���������� �����
    manager::Frame::EntityDrawFunction entity_draw_handler;       //���������� ��������� ��������
    size_t                             properties_update_counter; //������� ���������� �������
};

}

}

}

extern "C"
{

TechniqueComponentRegistrator<ForEachLightTechnique> ForEachLightTechnique;

}
