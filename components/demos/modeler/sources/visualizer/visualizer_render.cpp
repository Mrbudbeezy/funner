#include <cstdio>
#include <ctime>
#include <exception>

#include <stl/hash_map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/iterator.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/string.h>
#include <xtl/visitor.h>

#include <mathlib.h>

#include <common/component.h>
#include <common/file.h>

#include <sg/camera.h>
#include <sg/scene.h>
#include <sg/visual_model.h>

#include <render/low_level/device.h>

#include <render/mid_level/low_level_renderer.h>

#include <render/scene_render.h>
#include <render/custom_render.h>
#include <render/mid_level/renderer.h>

#include "shared.h"

using namespace render;
using namespace scene_graph;

namespace
{

const char* SHADER_FILE_NAME  = "media/fpp_shader.wxf";

typedef xtl::com_ptr<low_level::IBlendState> BlendStatePtr;

//������ ��������� ������ ������� � ������
stl::string read_shader (const char* file_name)
{
  common::InputFile file (file_name);
  
  stl::string buffer (file.Size (), ' ');
  
  file.Read (&buffer [0], file.Size ());
  
  return buffer;
} 

void print (const char* message)
{
  printf ("Shader message: '%s'\n", message);
}

//�������� ��������� ���������� ������
BlendStatePtr create_blend_state
 (low_level::IDevice&      device,
  bool                     blend_enable,
  low_level::BlendArgument src_arg,
  low_level::BlendArgument dst_color_arg,
  low_level::BlendArgument dst_alpha_arg)
{
  low_level::BlendDesc blend_desc;

  memset (&blend_desc, 0, sizeof (blend_desc));

  blend_desc.blend_enable                     = blend_enable;
  blend_desc.blend_color_operation            = low_level::BlendOperation_Add;
  blend_desc.blend_alpha_operation            = low_level::BlendOperation_Add;
  blend_desc.blend_color_source_argument      = src_arg;
  blend_desc.blend_color_destination_argument = dst_color_arg;
  blend_desc.blend_alpha_source_argument      = src_arg;
  blend_desc.blend_alpha_destination_argument = dst_alpha_arg;
  blend_desc.color_write_mask                 = low_level::ColorWriteFlag_All;

  return BlendStatePtr (device.CreateBlendState (blend_desc), false);
}

class RenderManager
{
  public:
///��������� ������ �� �����
    RenderableModel& GetRenderableModel (low_level::IDevice* device, const char* model_name)
    {
      RenderableModelMap::iterator iter = renderable_models.find (model_name);

      if (iter == renderable_models.end ())
      {
        try
        {
          RenderableModelPtr new_model (new RenderableModel (*device, model_name));

          renderable_models.insert_pair (model_name, new_model);
        }
        catch (std::exception& exception)
        {
          printf ("exception: %s\n    at RenderManager::GetRenderableModel\n", exception.what ());
          throw;
        }
      }

      return *renderable_models[model_name];
    }

  private:
    typedef xtl::shared_ptr<RenderableModel> RenderableModelPtr;

    typedef stl::hash_map<stl::hash_key<const char*>, RenderableModelPtr> RenderableModelMap;

  private:
    RenderableModelMap  renderable_models;
};

#pragma pack (1)

struct MyShaderParameters
{
  math::mat4f object_tm; 
  math::mat4f view_tm;
  math::mat4f proj_tm;
  int         light_enable;
};

typedef xtl::com_ptr<low_level::IBuffer> BufferPtr;

/*
    ���������� �������� �����
*/

struct RenderViewVisitor: public xtl::visitor<void, VisualModel>
{
  RenderManager*          render_manager;
  low_level::IDevice*     device;
  low_level::IBlendState* envelope_blend_state;
  low_level::IBlendState* trajectory_blend_state;
  BufferPtr               cb;
  MyShaderParameters      *shader_parameters;
  float                   angle;

  RenderViewVisitor (RenderManager*          in_render_manager,
                     low_level::IDevice*     in_device,
                     BufferPtr               in_cb,
                     MyShaderParameters&     in_shader_parameters,
                     float                   in_angle,
                     low_level::IBlendState& in_envelope_blend_state,
                     low_level::IBlendState& in_trajectory_blend_state)
    : render_manager (in_render_manager),
      device (in_device),
      cb (in_cb),
      shader_parameters (&in_shader_parameters),
      angle (in_angle),
      envelope_blend_state (&in_envelope_blend_state),
      trajectory_blend_state (&in_trajectory_blend_state)
  {
  }

  void visit (VisualModel& model)
  {
    RenderableModel& renderable_model = render_manager->GetRenderableModel (device, model.MeshName ());
    
    if (!xtl::xstrcmp (model.Name (), "Envelope"))
    {
      shader_parameters->light_enable = true;

//      device->OSSetBlendState (envelope_blend_state);            
    }
    else
    {
      shader_parameters->light_enable = false;
      
//      device->OSSetBlendState (trajectory_blend_state);      
    }

    shader_parameters->object_tm = math::rotatef (angle, 0, 0, 1) * math::rotatef (angle, 0, 1, 0) * 
                                   math::rotatef (angle, 1, 0, 0) * model.WorldTM ();

    cb->SetData (0, sizeof *shader_parameters, shader_parameters);

    renderable_model.Draw (*device);
  }
};


/*
===================================================================================================
    ���������
===================================================================================================
*/

const char* COMPONENT_NAME   = "render.scene_render.ModelerRender"; //��� ����������
const char* RENDER_PATH_NAME = "ModelerRender";                     //��� ���� ����������

typedef xtl::com_ptr<low_level::IProgram>                 ProgramPtr;
typedef xtl::com_ptr<low_level::IProgramParametersLayout> ProgramParametersLayoutPtr;

/*
===================================================================================================
    ������� ������ �������
===================================================================================================
*/

class ModelerView: public IRenderView, public xtl::reference_counter, public render::mid_level::ILowLevelFrame::IDrawCallback
{
  public:
    ModelerView (render::mid_level::ILowLevelRenderer* in_renderer, scene_graph::Scene* in_scene, RenderManager* in_render_manager)
      : renderer (in_renderer),
        scene (in_scene),
        camera (0),
        render_manager (in_render_manager),
        frame (in_renderer->CreateFrame (), false),
        current_angle (0.f),
        shader_source (read_shader (SHADER_FILE_NAME))
    {
      frame->SetCallback (this);      
    }
    
///������� ������ ����������
    void SetRenderTargets (mid_level::IRenderTarget* render_target, mid_level::IRenderTarget* depth_stencil_target)
    {
      frame->SetRenderTargets (render_target, depth_stencil_target);
    }
    
    mid_level::IRenderTarget* GetRenderTarget ()
    {
      throw xtl::make_not_implemented_exception ("ModelerView::GetRenderTarget");
    }
    
    mid_level::IRenderTarget* GetDepthStencilTarget ()
    {
      throw xtl::make_not_implemented_exception ("ModelerView::GetDepthStencilTarget");
    }
  
///��������� ������� ������
    void SetViewport (const Rect& rect)
    {
      render::mid_level::Viewport viewport;
      
      viewport.x      = rect.left;
      viewport.y      = rect.top;
      viewport.width  = rect.width;
      viewport.height = rect.height;
      
      frame->SetViewport (viewport);
    }
    
    void GetViewport (Rect&)
    {
      throw xtl::make_not_implemented_exception ("ModelerView::GetViewport");
    }
    
///��������� ������
    void SetCamera (scene_graph::Camera* new_camera)
    {
      camera = new_camera;
    }

    scene_graph::Camera* GetCamera ()
    {
      throw xtl::make_not_implemented_exception ("ModelerView::GetCamera");
    }

///��������� / ������ �������
    void SetProperty (const char* name, const char* value) {}

    void GetProperty (const char*, size_t, char*)
    {
      throw xtl::make_not_implemented_exception ("ModelerView::GetProperty");      
    }

///���������
    void Draw ()
    {
      if (!camera)
        return;

      renderer->AddFrame (frame.get ());
    }

    void Draw (render::low_level::IDevice& device)
    {      
      if (!trajectory_blend_state)
        trajectory_blend_state = create_blend_state (device, true, low_level::BlendArgument_One, low_level::BlendArgument_One, low_level::BlendArgument_One);
        
      if (!envelope_blend_state)
        envelope_blend_state = create_blend_state (device, false, low_level::BlendArgument_One, low_level::BlendArgument_Zero, low_level::BlendArgument_Zero);
      
      if (!shader)
      {
        low_level::ShaderDesc shader_descs [] = {
          {"fpp_shader", size_t (-1), shader_source.c_str (), "fpp", ""},
        };

        static low_level::ProgramParameter shader_parameters[] = {
          {"myProjMatrix", low_level::ProgramParameterType_Float4x4, 0, 1, offsetof (MyShaderParameters, proj_tm)},
          {"myViewMatrix", low_level::ProgramParameterType_Float4x4, 0, 1, offsetof (MyShaderParameters, view_tm)},
          {"myObjectMatrix", low_level::ProgramParameterType_Float4x4, 0, 1, offsetof (MyShaderParameters, object_tm)},
          {"myLightEnable", low_level::ProgramParameterType_Int, 0, 1, offsetof (MyShaderParameters, light_enable)},
        };
        
        low_level::ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};

        shader = ProgramPtr (device.CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print), false);

        program_parameters_layout = ProgramParametersLayoutPtr (device.CreateProgramParametersLayout (program_parameters_layout_desc), false);
      }

      if (!cb)
      {
        low_level::BufferDesc cb_desc;
        
        memset (&cb_desc, 0, sizeof cb_desc);
        
        cb_desc.size         = sizeof MyShaderParameters;
        cb_desc.usage_mode   = low_level::UsageMode_Default;
        cb_desc.bind_flags   = low_level::BindFlag_ConstantBuffer;
        cb_desc.access_flags = low_level::AccessFlag_ReadWrite;

        cb = BufferPtr (device.CreateBuffer (cb_desc), false);
      }

      render::low_level::RasterizerDesc rs_desc;

      memset (&rs_desc, 0, sizeof (rs_desc));

      rs_desc.fill_mode  = render::low_level::FillMode_Wireframe;
      rs_desc.cull_mode  = render::low_level::CullMode_Back;      

      xtl::com_ptr<render::low_level::IRasterizerState> rasterizer (device.CreateRasterizerState (rs_desc), false);

      device.RSSetState (rasterizer.get ());

      MyShaderParameters my_shader_parameters;
      
      my_shader_parameters.proj_tm      = camera->ProjectionMatrix ();
      my_shader_parameters.view_tm      = invert (camera->WorldTM ());
      my_shader_parameters.light_enable = 1;

      device.SSSetProgram (shader.get ());
      device.SSSetProgramParametersLayout (program_parameters_layout.get ());
      device.SSSetConstantBuffer (0, cb.get ());

      RenderViewVisitor visitor (render_manager, &device, cb, my_shader_parameters, current_angle, *envelope_blend_state, *trajectory_blend_state);

      scene->VisitEach (visitor);

      static clock_t last = 0;

      if (clock () - last > CLK_TCK / 50)
      {
        current_angle += 0.1f * float (clock () - last) / CLK_TCK;
      
        last = clock ();
      }
    }

///������� ������
    void AddRef ()    
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

  private:
    typedef xtl::com_ptr<render::mid_level::ILowLevelFrame> FramePtr;

  private:
    render::mid_level::ILowLevelRenderer* renderer;
    scene_graph::Scene*                   scene;
    scene_graph::Camera*                  camera;
    RenderManager*                        render_manager;
    FramePtr                              frame;
    float                                 current_angle;
    BlendStatePtr                         envelope_blend_state;
    BlendStatePtr                         trajectory_blend_state;    
    stl::string                           shader_source;
    ProgramPtr                            shader;
    ProgramParametersLayoutPtr            program_parameters_layout;
    BufferPtr                             cb;
};

/*
===================================================================================================
    ������ �����
===================================================================================================
*/

class ModelerRender: public ICustomSceneRender, public xtl::reference_counter
{
  public:
    ModelerRender (mid_level::IRenderer* in_renderer) : renderer (0)
    {
      renderer = dynamic_cast<mid_level::ILowLevelRenderer*> (in_renderer);

      if (!renderer)
        throw xtl::format_exception<xtl::bad_argument> ("ModelerRender::ModelerRender", "Renderer is not castable to render::mid_level::ILowLevelRenderer");
    }
  
///�������� �������� ������
    IRenderView* CreateRenderView (scene_graph::Scene* scene)
    {
      return new ModelerView (renderer, scene, &render_manager);
    }

///������ � ���������
    void LoadResource (const char* tag, const char* file_name)
    {
      printf ("LoadResource(%s, %s)\n", tag, file_name);
    }

///��������� ������� ����������� ����������������
    void SetLogHandler (const ICustomSceneRender::LogFunction&) {}

    const ICustomSceneRender::LogFunction& GetLogHandler ()
    {
      throw xtl::make_not_implemented_exception ("ModelerView::GetLogHandler");
    }
    
    void SetQueryHandler (const QueryFunction& in_handler)
    {
      query_handler = in_handler;
    }
    
    const QueryFunction& GetQueryHandler () { return query_handler; }

///������� ������
    void AddRef ()
    {
      addref (this);
    }    

    void Release ()
    {
      release (this);
    }

///�������� �������
    static ICustomSceneRender* Create (mid_level::IRenderer* renderer, const char* path_name)
    {
      printf ("ModelerRender::Create(%s, %s)\n", renderer ? renderer->GetDescription () : "null", path_name);
      
      return new ModelerRender (renderer);
    }

  private:
    render::mid_level::ILowLevelRenderer* renderer;
    QueryFunction                         query_handler;
    RenderManager                         render_manager;
};

/*
===================================================================================================
    ����������� ����������
===================================================================================================
*/

class ModelerRenderComponent
{
  public:
    ModelerRenderComponent ()
    {
      SceneRenderManager::RegisterRender (RENDER_PATH_NAME, &CreateRender);
    }

  private:
    static ICustomSceneRender* CreateRender (mid_level::IRenderer* renderer, const char*)
    {
      return new ModelerRender (renderer);
    }
};

}

extern "C" 
{

common::ComponentRegistrator<ModelerRenderComponent> ModelerRender (COMPONENT_NAME);

}
