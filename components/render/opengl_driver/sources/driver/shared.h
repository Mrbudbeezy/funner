#ifndef RENDER_GL_DRIVER_DRIVER_SHARED_HEADER
#define RENDER_GL_DRIVER_DRIVER_SHARED_HEADER

#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>

#include <common/component.h>
#include <common/property_map.h>
#include <common/strlib.h>

#include <syslib/application.h>
#include <syslib/screen.h>

#include <render/low_level/device.h>
#include <render/low_level/driver.h>
#include <render/low_level/utils.h>

#include <shared/context_manager.h>
#include <shared/driver_output_manager.h>
#include <shared/input_stage.h>
#include <shared/object.h>
#include <shared/output_stage.h>
#include <shared/platform.h>
#include <shared/property_list.h>
#include <shared/query_manager.h>
#include <shared/render_target_manager.h>
#include <shared/shader_stage.h>
#include <shared/texture_manager.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ��� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver: virtual public IDriver, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Driver  ();
    ~Driver ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetCaps (DriverCaps&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t    GetAdaptersCount ();
    IAdapter* GetAdapter       (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IAdapter* CreateAdapter (const char* name, const char* path, const char* init_string);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* CreateSwapChain (size_t prefered_adapters_count, IAdapter** prefered_adapters, const SwapChainDesc& desc);
    ISwapChain* CreateSwapChain (IDevice* device, const SwapChainDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (ISwapChain* swap_chain, const char* init_string);
    IDevice* CreateDevice (size_t prefered_adapters_count, IAdapter** prefered_adapters, const char* init_string);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������� ���������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetDebugLog (const LogFunction&);
    const LogFunction& GetDebugLog ();

  private:
    void RegisterAdapter   (IAdapter*);
    void UnregisterAdapter (IAdapter*);

  private:
    typedef stl::vector<IAdapter*> AdapterArray;

  private:
    AdapterArray adapters; //�������� ���������
    LogFunction  log_fn;   //������� ����������� ����������������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Device: virtual public IDevice, virtual public IDeviceContext, public Object
{
  friend class StateBlock;
  public:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Device  (ISwapChain* swap_chain, const char* init_string);
    ~Device ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ���������� ���������, �������� ����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* GetDevice () { return this; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IAdapter* GetAdapter ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        GetCaps                        (DeviceCaps&);
    const char* GetCapString                   (DeviceCapString);
    const char* GetVertexAttributeSemanticName (VertexAttributeSemantic semantic);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IInputLayout*             CreateInputLayout             (const InputLayoutDesc&);
    IProgramParametersLayout* CreateProgramParametersLayout (const ProgramParametersLayoutDesc&);
    IRasterizerState*         CreateRasterizerState         (const RasterizerDesc&);
    IBlendState*              CreateBlendState              (const BlendDesc&);
    IDepthStencilState*       CreateDepthStencilState       (const DepthStencilDesc&);
    ISamplerState*            CreateSamplerState            (const SamplerDesc&);
    IBuffer*                  CreateBuffer                  (const BufferDesc&, const void*);
    IProgram*                 CreateProgram                 (unsigned int shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log);
    ITexture*                 CreateTexture                 (const TextureDesc&);
    ITexture*                 CreateTexture                 (const TextureDesc&, const TextureData&);
    ITexture*                 CreateTexture                 (const TextureDesc&, IBuffer* buffer, unsigned int buffer_offset, const unsigned int* mip_sizes = 0);
    ITexture*                 CreateRenderTargetTexture     (ISwapChain* swap_chain, unsigned int buffer_index);
    ITexture*                 CreateDepthStencilTexture     (ISwapChain* swap_chain);
    IView*                    CreateView                    (ITexture* texture, const ViewDesc&);
    IPredicate*               CreatePredicate               ();
    IQuery*                   CreateQuery                   (QueryType type);
    IStateBlock*              CreateStateBlock              (const StateBlockMask& mask);
    IDeviceContext*           CreateDeferredContext         ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������� (input-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          ISSetInputLayout  (IInputLayout* state);
    void          ISSetVertexBuffer (unsigned int vertex_buffer_slot,  IBuffer* buffer);
    void          ISSetIndexBuffer  (IBuffer* buffer);
    IInputLayout* ISGetInputLayout  ();
    IBuffer*      ISGetVertexBuffer (unsigned int vertex_buffer_slot);
    IBuffer*      ISGetIndexBuffer  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SOSetTargets      (unsigned int buffers_count, IBuffer** buffers, const unsigned int* offsets);
    void         SOSetTarget       (unsigned int stream_output_slot, IBuffer* buffer, unsigned int offset);
    IBuffer*     SOGetTarget       (unsigned int stream_output_slot);
    unsigned int SOGetTargetOffset (unsigned int stream_output_slot);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� �������� (shader-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                      SSSetProgram                 (IProgram* program);
    void                      SSSetProgramParametersLayout (IProgramParametersLayout* parameters_layout);
    void                      SSSetSampler                 (unsigned int sampler_slot, ISamplerState* state);
    void                      SSSetTexture                 (unsigned int sampler_slot, ITexture* texture);
    void                      SSSetConstantBuffer          (unsigned int buffer_slot, IBuffer* buffer);
    IProgramParametersLayout* SSGetProgramParametersLayout ();
    IProgram*                 SSGetProgram                 ();
    ISamplerState*            SSGetSampler                 (unsigned int sampler_slot);
    ITexture*                 SSGetTexture                 (unsigned int sampler_slot);
    IBuffer*                  SSGetConstantBuffer          (unsigned int buffer_slot);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� (rasterizer-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              RSSetState    (IRasterizerState* state);
    void              RSSetViewport (unsigned int render_target_slot, const Viewport& viewport);
    void              RSSetScissor  (unsigned int render_target_slot, const Rect& scissor_rect);
    IRasterizerState* RSGetState    ();
    const Viewport&   RSGetViewport (unsigned int render_target_slot);
    const Rect&       RSGetScissor  (unsigned int render_target_slot);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ������� (output-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                OSSetBlendState        (IBlendState* state);
    void                OSSetDepthStencilState (IDepthStencilState* state);
    void                OSSetStencilReference  (unsigned int reference);
    void                OSSetRenderTargets     (unsigned int count, IView** render_target_view, IView* depth_stencil_view);
    void                OSSetRenderTargetView  (unsigned int render_target_slot, IView* view);
    void                OSSetDepthStencilView  (IView* view);
    IBlendState*        OSGetBlendState        ();
    IDepthStencilState* OSGetDepthStencilState ();
    unsigned int        OSGetStencilReference  ();
    IView*              OSGetRenderTargetView  (unsigned int render_target_slot);
    IView*              OSGetDepthStencilView  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ClearRenderTargetView (unsigned int view_index, const Color4f& color);
    void ClearDepthStencilView (unsigned int clear_flags, float depth, unsigned char stencil);
    void ClearViews            (unsigned int clear_flags, unsigned int views_count, const unsigned int* view_indices, const Color4f* colors, float depth, unsigned char stencil);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���-������� �������� (���������� ��� ������� � ������� ������� ���������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GenerateMips (ITexture* texture);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetPredication    (IPredicate* predicate, bool predicate_value);
    IPredicate* GetPredicate      ();
    bool        GetPredicateValue ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DrawAuto             (PrimitiveType primitive_type);
    void Draw                 (PrimitiveType primitive_type, unsigned int first_vertex, unsigned int vertices_count);
    void DrawIndexed          (PrimitiveType primitive_type, unsigned int first_index, unsigned int indices_count, unsigned int base_vertex);
    void DrawInstanced        (PrimitiveType primitive_type, unsigned int vertex_count_per_instance, unsigned int instance_count, unsigned int first_vertex, unsigned int first_instance_location);
    void DrawIndexedInstanced (PrimitiveType primitive_type, unsigned int index_count_per_instance, unsigned int instance_count, unsigned int first_index, unsigned int base_vertex, unsigned int first_instance_location);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPropertyList* GetProperties ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������������� ��������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDeviceContext* GetImmediateContext () { return this; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ICommandList* FinishCommandList  (bool restore_state);
    void          ExecuteCommandList (ICommandList* list, bool restore_state);

  private:
    ITexture* CreateTexture (const TextureDesc&, const TextureData*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ���������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (unsigned int base_vertex, unsigned int base_index, IndicesLayout* out_indices_layout = 0);

  private:
    ContextManager      context_manager;        //�������� ���������� OpenGL
    PropertyList        properties;             //�������� ����������
    RenderTargetManager render_target_manager;  //�������� ������� ������� ����������
    OutputStage         output_stage;           //�������� �������
    InputStage          input_stage;            //������� �������
    TextureManager      texture_manager;        //�������� �������
    ShaderStage         shader_stage;           //������� ��������
    QueryManager        query_manager;          //�������� ��������
    unsigned int        cached_base_vertex;     //������������ ������ ������� �������
    unsigned int        cached_base_index;      //������������ ������ �������� �������
    IndicesLayout       cached_indices_layout;  //������������ ������������ ��������
    bool                need_validate_programs; //����� �� ������������ ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ��������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class StateBlock: virtual public IStateBlock, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    StateBlock (Device& device, const StateBlockMask&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetMask (StateBlockMask&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Capture (IDeviceContext* context);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Apply (IDeviceContext* context);

  private:
    typedef stl::auto_ptr<IStageState> StageStatePtr;

  private:
    Device&        device;                      //������ �� ���������� ���������
    StateBlockMask mask;                        //����� ���������
    StageStatePtr  render_target_manager_state; //��������� ��������� ������� ������� ����������
    StageStatePtr  output_stage_state;          //��������� ��������� ������
    StageStatePtr  input_stage_state;           //��������� �������� ������
    StageStatePtr  texture_manager_state;       //��������� ��������� �������
    StageStatePtr  shader_stage_state;          //��������� ���������� ������
    StageStatePtr  query_manager_state;         //��������� ��������� ��������
};

}

}

}

#endif
