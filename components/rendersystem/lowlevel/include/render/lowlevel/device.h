#ifndef RENDER_LOW_LEVEL_DEVICE_HEADER
#define RENDER_LOW_LEVEL_DEVICE_HEADER

#include <render/lowlevel/state.h>
#include <render/lowlevel/texture.h>
#include <render/lowlevel/query.h>
#include <render/lowlevel/frame_buffer.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t DEVICE_VERTEX_BUFFER_SLOTS_COUNT = 8; //���������� ������ ��� ���������� ��������� �������
const size_t DEVICE_SAMPLER_SLOTS_COUNT       = 8; //���������� ������ ��� ���������� ���������

//���������� �� ����������????

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
enum ClearFlags
{
  ClearFlags_Color   = 1, //������� ����� ����� (render-target)
  ClearFlags_Depth   = 2, //������� ����� �������
  ClearFlags_Stencil = 4  //������� ����� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDevice: virtual public IDeviceObject
{
  public:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IInputLayoutState*  CreateInputLayoutState  (const InputLayoutDesc&) = 0;    
    virtual IPredicate*         CreatePredicate         () = 0;
    virtual IStatisticsQuery*   CreateStatisticsQuery   () = 0;
        //��� �������� ��������� ����� �� ���������� Desc ��� ������������ ����� Set/GetDesc???
    
    virtual ILightingState*     CreateLightingState     (const LightingDesc&) = 0;
    virtual IViewerState*       CreateViewerState       (const ViewerDesc&) = 0;
    virtual ITransformState*    CreateTransformState    (const TransformDesc&) = 0;
    virtual IMaterialState*     CreateMaterialState     (const MaterialDesc&) = 0;
    virtual IRasterizerState*   CreateRasterizerState   (const RasterizerDesc&) = 0;
    virtual IBlendState*        CreateBlendState        (const BlendDesc&) = 0;
    virtual IDepthStencilState* CreateDepthStencilState (const DepthStencilDesc&) = 0;
    virtual ISamplerState*      CreateSamplerState      (const SamplerDesc&) = 0;
    virtual IVertexBuffer*      CreateVertexBuffer      (const BufferDesc&) = 0;
    virtual IIndexBuffer*       CreateIndexBuffer       (const BufferDesc&) = 0;
    virtual ITexture*           CreateTexture           (const TextureDesc&) = 0;
    virtual IFrameBuffer*       CreateFrameBuffer       (const FrameBufferDesc&) = 0;
      //�������� ������ ����� � �������� � ���� �������� � �������� ������-�������???

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������� (input-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void               ISSetInputLayout  (IInputLayoutState* state) = 0;
    virtual void               ISSetVertexBuffer (size_t vertex_buffer_slot,  IVertexBuffer* buffer) = 0;
    virtual void               ISSetIndexBuffer  (IIndexBuffer* buffer) = 0;
    virtual IInputLayoutState* ISGetInputLayout  () = 0;
    virtual IVertexBuffer*     ISGetVertexBuffer (size_t vertex_buffer_slot) = 0;
    virtual IIndexBuffer*      ISGetIndexBuffer  () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������� ��������� -- ������ � InputStage!!!
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void        ISSetPredication    (IPredicate* predicate, bool predicate_value) = 0;
    virtual IPredicate* ISGetPredicate      () = 0;
    virtual bool        ISGetPredicateValue () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� �������� (shader-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void             SSSetMode           (ShaderMode mode) = 0;
    virtual void             SSSetViewer         (IViewerState* state) = 0;
    virtual void             SSSetTransform      (ITransformState* state) = 0;
    virtual void             SSSetTransformBase  (size_t first_matrix_index) = 0;
    virtual void             SSSetLighting       (ILightingState* state) = 0;
    virtual void             SSSetSampler        (size_t sampler_slot, ISamplerState* state) = 0;
    virtual void             SSSetTexture        (size_t sampler_slot, ITexture* texture) = 0;
    virtual void             SSSetMaterial       (IMaterialState* state) = 0;
    virtual ShaderMode       SSGetMode           () = 0;
    virtual IViewerState*    SSGetViewer         () = 0;
    virtual ITransformState* SSGetTransform      () = 0;
    virtual size_t           SSGetTransformBase  () = 0;
    virtual ILightingState*  SSGetLighting       () = 0;
    virtual ISamplerState*   SSGetSampler        (size_t sampler_slot) = 0;
    virtual ITexture*        SSGetTexture        (size_t sampler_slot) = 0;
    virtual IMaterialState*  SSGetMaterial       () = 0;

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
///���������� �������� ������� (output-merger)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void                OMSetBlendState        (IBlendState* state) = 0;
    virtual void                OMSetDepthStencil      (IDepthStencilState* state) = 0;
    virtual void                OMSetStencilReference  (size_t reference) = 0;    
    virtual void                OMSetFrameBuffer       (IFrameBuffer* frame_buffer) = 0;
    virtual IBlendState*        OMGetBlendState        () = 0;
    virtual IDepthStencilState* OMGetDepthStencilState () = 0;
    virtual size_t              OMGetStencilReference  () = 0;
    virtual IFrameBuffer*       OMGetFrameBuffer       () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void ClearRenderTargetSurface (IRenderTargetSurface* surface, const Color4f& color) = 0;
    virtual void ClearDepthSurface        (IDepthStencilSurface* surface, float depth) = 0;
    virtual void ClearStencilSurface      (IDepthStencilSurface* surface, unsigned char value) = 0;
    virtual void Clear                    (IFrameBuffer* buffer, size_t clear_flags, const Colo4f& color, float depth, unsigned char stencil) = 0;

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
