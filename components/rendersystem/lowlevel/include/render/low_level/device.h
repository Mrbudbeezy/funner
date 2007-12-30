#ifndef RENDER_LOW_LEVEL_DEVICE_HEADER
#define RENDER_LOW_LEVEL_DEVICE_HEADER

#include <exception>
#include <render/low_level/buffer.h>
#include <render/low_level/state.h>
#include <render/low_level/view.h>
#include <render/low_level/query.h>

namespace render
{

namespace low_level
{

//forward declarations
class ISwapChain;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t DEVICE_VERTEX_BUFFER_SLOTS_COUNT = 8; //���������� ������ ��� ���������� ��������� �������
const size_t DEVICE_SAMPLER_SLOTS_COUNT       = 8; //���������� ������ ��� ���������� ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ShaderMode
{
  ShaderMode_Flat,     //���������� ������ ���������
  ShaderMode_Gourand,  //������ ��������� �� ����
  ShaderMode_Phong,    //������ ��������� �� �����

  ShaderMode_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ClearFlag
{
  ClearFlag_RenderTargetView = 1, //������� ������ ����� (render-target)
  ClearFlag_DepthStencilView = 2, //������� ����� ������� � ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PrimitiveType
{
  PrimitiveType_PointList,     //������ �����
  PrimitiveType_LineList,      //������ ��������
  PrimitiveType_LineStrip,     //������� ��������
  PrimitiveType_TriangleList,  //������ �������������
  PrimitiveType_TriangleStrip, //������� �������������
  PrimitiveType_TriangleFan,   //����� �������������

  PrimitiveType_Num
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
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IInputLayoutState*  CreateInputLayoutState    (const InputLayoutDesc&) = 0;
    virtual ILightingState*     CreateLightingState       (const LightingDesc&) = 0;
    virtual IViewerState*       CreateViewerState         (const ViewerDesc&) = 0;
    virtual ITransformState*    CreateTransformState      (const TransformDesc&) = 0;
    virtual IMaterialState*     CreateMaterialState       (const MaterialDesc&) = 0;
    virtual IRasterizerState*   CreateRasterizerState     (const RasterizerDesc&) = 0;
    virtual IBlendState*        CreateBlendState          (const BlendDesc&) = 0;
    virtual IDepthStencilState* CreateDepthStencilState   (const DepthStencilDesc&) = 0;
    virtual ISamplerState*      CreateSamplerState        (const SamplerDesc&) = 0;
    virtual IBuffer*            CreateVertexBuffer        (const BufferDesc&) = 0;
    virtual IBuffer*            CreateIndexBuffer         (const BufferDesc&) = 0;
    virtual ITexture*           CreateTexture             (const TextureDesc&) = 0;
    virtual ITexture*           CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index) = 0;
    virtual ITexture*           CreateDepthStencilTexture (ISwapChain* swap_chain) = 0;
    virtual IView*              CreateView                (ITexture* texture, const ViewDesc& desc) = 0;
    virtual IPredicate*         CreatePredicate           () = 0;
    virtual IStatisticsQuery*   CreateStatisticsQuery     () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������� (input-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void               ISSetInputLayout  (IInputLayoutState* state) = 0;
    virtual void               ISSetVertexBuffer (size_t vertex_buffer_slot,  IBuffer* buffer) = 0;
    virtual void               ISSetIndexBuffer  (IBuffer* buffer) = 0;
    virtual IInputLayoutState* ISGetInputLayout  () = 0;
    virtual IBuffer*           ISGetVertexBuffer (size_t vertex_buffer_slot) = 0;
    virtual IBuffer*           ISGetIndexBuffer  () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� �������� (shader-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void             SSSetMode       (ShaderMode mode) = 0;
    virtual void             SSSetViewer     (IViewerState* state) = 0;
    virtual void             SSSetTransform  (ITransformState* state) = 0;
    virtual void             SSSetLighting   (ILightingState* state) = 0;
    virtual void             SSSetSampler    (size_t sampler_slot, ISamplerState* state) = 0;
    virtual void             SSSetTexture    (size_t sampler_slot, ITexture* texture) = 0;
    virtual void             SSSetMaterial   (IMaterialState* state) = 0;
    virtual ShaderMode       SSGetMode       () = 0;
    virtual IViewerState*    SSGetViewer     () = 0;
    virtual ITransformState* SSGetTransform  () = 0;
    virtual ILightingState*  SSGetLighting   () = 0;
    virtual ISamplerState*   SSGetSampler    (size_t sampler_slot) = 0;
    virtual ITexture*        SSGetTexture    (size_t sampler_slot) = 0;
    virtual IMaterialState*  SSGetMaterial   () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � ��������� ������� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool SSIsSupported (ShaderMode mode) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� (rasterizer-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void              RSSetState    (IRasterizerState* state) = 0;
    virtual void              RSSetViewport (const Viewport& viewport) = 0;
    virtual void              RSSetScissor  (const Rect& scissor_rect) = 0;
    virtual IRasterizerState* RSGetState    () = 0;
    virtual const Viewport&   RSGetViewport () = 0;
    virtual const Rect&       RSGetScissor  () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ������� (output-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void                OSSetBlendState        (IBlendState* state) = 0;
    virtual void                OSSetDepthStencilState (IDepthStencilState* state) = 0;
    virtual void                OSSetStencilReference  (size_t reference) = 0;
    virtual void                OSSetRenderTargets     (IView* render_target_view, IView* depth_stencil_view) = 0;
    virtual IBlendState*        OSGetBlendState        () = 0;
    virtual IDepthStencilState* OSGetDepthStencilState () = 0;
    virtual size_t              OSGetStencilReference  () = 0;
    virtual IView*              OSGetRenderTargetView  () = 0;
    virtual IView*              OSGetDepthStencilView  () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void ClearRenderTargetView (const Color4f& color) = 0;
    virtual void ClearDepthStencilView (float depth, unsigned char stencil) = 0;
    virtual void ClearViews            (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void        SetPredication    (IPredicate* predicate, bool predicate_value) = 0;
    virtual IPredicate* GetPredicate      () = 0;
    virtual bool        GetPredicateValue () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Draw        (PrimitiveType primitive_type, size_t first_vertex, size_t vertices_count) = 0;
    virtual void DrawIndexed (PrimitiveType primitive_type, size_t first_index, size_t indices_count, size_t base_vertex) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Flush () = 0;
};

}

}

#endif
