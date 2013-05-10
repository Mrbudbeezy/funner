#ifndef RENDER_LOW_LEVEL_DEVICE_HEADER
#define RENDER_LOW_LEVEL_DEVICE_HEADER

#include <exception>
#include <render/low_level/context.h>

namespace render
{

namespace low_level
{

//forward declarations
class ISwapChain;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct DeviceCaps
{
  size_t max_texture_1d_size;            //������������ ����� ���������� ��������
  size_t max_texture_2d_size;            //������������ ������ ����� ��������� ��������
  size_t max_texture_3d_size;            //������������ ������ ����� ��������� ��������
  size_t max_texture_cubemap_size;       //������������ ������ ����� ���������� ��������
  size_t max_texture_anisotropy;         //������������ ������� �����������
  size_t samplers_count;                 //���������� ���������� ���������
  bool   has_advanced_blend;             //�������������� ������� ������ ���������� ������
  bool   has_depth_texture;              //�������������� �������� � ����������� � �������
  bool   has_multisample;                //�������������� ��������������
  bool   has_occlusion_queries;          //�������������� ������� ��������� ���������
  bool   has_shadow_maps;                //�������������� ����� �����
  bool   has_texture_anisotropic_filter; //�������������� ������������ ����������
  bool   has_texture_cube_map;           //�������������� ���������� ��������
  bool   has_texture_lod_bias;           //�������������� ������ ����������� �������
  bool   has_texture_mirrored_repeat;    //�������������� ��������� ���������� ���������
  bool   has_texture_non_power_of_two;   //�������������� �������� � ��������� �� �������� ������� ������
  bool   has_texture1d;                  //�������������� ���������� ��������
  bool   has_texture3d;                  //�������������� ��������� ��������
  bool   has_two_side_stencil;           //�������������� ������������ ����� ���������
  bool   has_right_hand_viewport;        //��������� �� ������� ������ � �������������� ������� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����� ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum DeviceCapString
{
  DeviceCapString_ShaderProfiles, //������ �� ������� �������� ��������
  
  DeviceCapString_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDevice: virtual public IObject
{
  public:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetName () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void        GetCaps                        (DeviceCaps&) = 0;
    virtual const char* GetCapString                   (DeviceCapString) = 0;
    virtual const char* GetVertexAttributeSemanticName (VertexAttributeSemantic semantic) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IInputLayout*             CreateInputLayout             (const InputLayoutDesc&) = 0;
    virtual IProgramParametersLayout* CreateProgramParametersLayout (const ProgramParametersLayoutDesc&) = 0;
    virtual IRasterizerState*         CreateRasterizerState         (const RasterizerDesc&) = 0;
    virtual IBlendState*              CreateBlendState              (const BlendDesc&) = 0;
    virtual IDepthStencilState*       CreateDepthStencilState       (const DepthStencilDesc&) = 0;
    virtual ISamplerState*            CreateSamplerState            (const SamplerDesc&) = 0;
    virtual IBuffer*                  CreateBuffer                  (const BufferDesc&, const void* buffer_data = 0) = 0;
    virtual IProgram*                 CreateProgram                 (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log) = 0;
    virtual ITexture*                 CreateTexture                 (const TextureDesc&) = 0;
    virtual ITexture*                 CreateTexture                 (const TextureDesc&, const TextureData&) = 0;
    virtual ITexture*                 CreateTexture                 (const TextureDesc&, IBuffer* buffer, size_t buffer_offset, const size_t* mip_sizes) = 0;
    virtual ITexture*                 CreateRenderTargetTexture     (ISwapChain* swap_chain, size_t buffer_index) = 0;
    virtual ITexture*                 CreateDepthStencilTexture     (ISwapChain* swap_chain) = 0;
    virtual IView*                    CreateView                    (ITexture* texture, const ViewDesc& desc) = 0;
    virtual IPredicate*               CreatePredicate               () = 0;
    virtual IQuery*                   CreateQuery                   (QueryType type) = 0;
    virtual IStateBlock*              CreateStateBlock              (const StateBlockMask& mask) = 0;
    virtual IDeviceContext*           CreateDeferredContext         () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������������� ��������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IDeviceContext* GetImmediateContext () = 0;
};

}

}

#endif
