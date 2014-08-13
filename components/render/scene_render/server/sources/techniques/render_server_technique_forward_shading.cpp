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

namespace
{

/*
    ���������
*/

const size_t LIGHTS_RESERVE_SIZE = 64;   //������������� ���������� ���������� �����

}

///������� ����������: ��������� ����������� �� ������� ��������� � �������� ���������� �����������
class ForwardShading: public BasicTechnique
{
  public:
///����������� / ����������
    ForwardShading (RenderManager& in_manager, const common::ParseNode& node)
      : BasicTechnique (in_manager, node, false)
      , manager (in_manager)
    {
        //������ ������������

      lighting_effect                = common::get<const char*> (node, "lighting_effect");
      root_effect                    = common::get<const char*> (node, "root_effect");
      spot_light_shader_light_type   = common::get<const char*> (node, "spot_light_shader_type", "");
      direct_light_shader_light_type = common::get<const char*> (node, "direct_light_shader_type", "");
      point_light_shader_light_type  = common::get<const char*> (node, "point_light_shader_type", "");
    }    

/// ����� ��� �����������
    static const char* ClassName     () { return "forward_shading"; }
    static const char* ComponentName () { return "render.scene.server.techniques.forward_shading"; }

  private:
    struct PrivateData: public BasicTechnique::PrivateData, public xtl::reference_counter
    {
      common::PropertyMap shader_defines;

      PrivateData (ForwardShading& technique)
        : BasicTechnique::PrivateData (technique, technique.lighting_effect.c_str ())
      {
        frame.SetShaderOptions (shader_defines);
      }
    };

    typedef xtl::intrusive_ptr<PrivateData> PrivateDataPtr;
    typedef stl::vector<PrivateDataPtr>     PrivateDataArray;

    struct RootPrivateData
    {
      PrivateDataArray light_frames;
      manager::Frame   frame;

      RootPrivateData (ForwardShading& technique)
        : frame (technique.manager.Manager ().CreateFrame ())
      {
        frame.SetEffect (technique.root_effect.c_str ());

        light_frames.reserve (LIGHTS_RESERVE_SIZE);
      }
    };
  
  private:
///���������� �����
    void UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder)
    {
      try
      {
          //��������� ��������� ������ �������

        RootPrivateData& private_data = private_data_holder.Get<RootPrivateData> (*this);

          //����������� ������ ���������� � ��������������� ��������, ���������� � ������
        
        TraverseResult& result = parent_context.TraverseResult ();

          //����� ����������        

        RenderingContext context (parent_context, private_data.frame);

        size_t light_index = 0;
          
        for (TraverseResult::LightArray::iterator iter=result.lights.begin (), end=result.lights.end (); iter!=end; ++iter, ++light_index)
          UpdateFromLight (context, **iter, result, light_index, private_data);

          //���������� ��������� �����

        parent_context.Frame ().AddFrame (private_data.frame);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene_render3d::ForEachLightTechnique::UpdateFrameCore");
        throw;
      }
    }

///���������� ����� �� ��������� �����
    void UpdateFromLight (RenderingContext& context, Light& light, TraverseResult& result, size_t light_index, RootPrivateData& root_private_data)
    {
        //��������� ������ ���������

     PrivateData& private_data = AllocateLightContext (root_private_data, light_index);

        //��������� ���������� ���������� ������
        
      BasicTechnique::ConfigureFrame (context, private_data);

        //��������� ���������� ���������

      manager::Frame&                 frame  = private_data.frame;
      const interchange::LightParams& params = light.Params ();

      common::PropertyMap& frame_properties = private_data.frame_properties;

      frame_properties.SetProperty ("LightWorldPosition", light.WorldMatrix () * math::vec3f (0.0f));
      frame_properties.SetProperty ("LightWorldDirection", light.WorldMatrix() * math::vec4f (0.0f, 0.0f, 1.0f, 0.0f));
      frame_properties.SetProperty ("LightColor", params.color * params.intensity);
      frame_properties.SetProperty ("LightAttenuation", params.attenuation);
      frame_properties.SetProperty ("LightRange", params.range);
      frame_properties.SetProperty ("LightExponent", params.exponent);
      frame_properties.SetProperty ("LightAngle", math::radian (params.angle));
      frame_properties.SetProperty ("LightRadius", params.radius);

      common::PropertyMap& frame_defines     = private_data.shader_defines;
      const char*          shader_light_type = "";

      switch (light.Type ())
      {
        case interchange::NodeType_PointLight:
          shader_light_type = point_light_shader_light_type.c_str ();
          break;
        case interchange::NodeType_DirectLight:
          shader_light_type = direct_light_shader_light_type.c_str ();
          break;
        case interchange::NodeType_SpotLight:
          shader_light_type = spot_light_shader_light_type.c_str ();
          break;
        default:
          return;
      }

      frame_defines.SetProperty ("LightType", shader_light_type);
     
        //���������� ��������������� ��������

      BasicTechnique::DrawVisualModels (context, private_data);
    }

///�������������� ��������� ���������
    PrivateData& AllocateLightContext (RootPrivateData& root_private_data, size_t light_index)
    {
      if (light_index < root_private_data.light_frames.size ())
        return *root_private_data.light_frames [light_index];
      
      PrivateDataPtr private_data (new PrivateData (*this), false);      
        
      root_private_data.light_frames.push_back (private_data);
        
      return *private_data;
    }

  private:
    RenderManager  manager;                        //�������� ����������
    stl::string    lighting_effect;                //������ ��������� � ���������� �����
    stl::string    root_effect;                    //�������� ������ ���������
    stl::string    spot_light_shader_light_type;   //��������� ������� ��� ����������� ��������� �����
    stl::string    direct_light_shader_light_type; //��������� ������� ��� ��������������� ��������� �����
    stl::string    point_light_shader_light_type;  //��������� ������� ��� ��������� ��������� �����
};

}

}

}

extern "C"
{

TechniqueComponentRegistrator<render::scene::server::ForwardShading> ForwardShading;

}
