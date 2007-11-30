#ifndef RENDER_LOW_LEVEL_DEVICE_HEADER
#define RENDER_LOW_LEVEL_DEVICE_HEADER

#include <exception>
#include <render/low_level/state.h>
#include <render/low_level/texture.h>
#include <render/low_level/query.h>
#include <render/low_level/frame_buffer.h>

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
  ClearFlag_Color   = 1, //������� ����� ����� (render-target)
  ClearFlag_Depth   = 2, //������� ����� �������
  ClearFlag_Stencil = 4  //������� ����� ���������
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
///����������: ���������� ������� � ������������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LostDeviceException: public std::exception
{
  const char* what () const throw () { return "render::low_level::LostDeviceException"; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDevice: virtual public IDeviceObject
{
  public:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetName () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IInputLayoutState*  CreateInputLayoutState  () = 0;
    virtual ILightingState*     CreateLightingState     () = 0;
    virtual IViewerState*       CreateViewerState       () = 0;
    virtual ITransformState*    CreateTransformState    () = 0;
    virtual IMaterialState*     CreateMaterialState     () = 0;
    virtual IRasterizerState*   CreateRasterizerState   () = 0;
    virtual IBlendState*        CreateBlendState        () = 0;
    virtual IDepthStencilState* CreateDepthStencilState () = 0;
    virtual ISamplerState*      CreateSamplerState      () = 0;
    virtual IVertexBuffer*      CreateVertexBuffer      (const BufferDesc&) = 0;
    virtual IIndexBuffer*       CreateIndexBuffer       (const BufferDesc&) = 0;
    virtual ITexture*           CreateTexture           (const TextureDesc&) = 0;
    virtual IFrameBuffer*       CreateFrameBuffer       (const FrameBufferDesc&) = 0;
    virtual IFrameBuffer*       CreateFrameBuffer       (ISwapChain*) = 0;
    virtual IFrameBuffer*       CreateFrameBuffer       (ITexture* render_target) = 0;
    virtual IPredicate*         CreatePredicate         () = 0;
    virtual IStatisticsQuery*   CreateStatisticsQuery   () = 0;

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
    virtual void                OSSetDepthStencil      (IDepthStencilState* state) = 0;
    virtual void                OSSetStencilReference  (size_t reference) = 0;    
    virtual void                OSSetFrameBuffer       (IFrameBuffer* frame_buffer) = 0;
    virtual IBlendState*        OSGetBlendState        () = 0;
    virtual IDepthStencilState* OSGetDepthStencilState () = 0;
    virtual size_t              OSGetStencilReference  () = 0;
    virtual IFrameBuffer*       OSGetFrameBuffer       () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void ClearRenderTargetSurface (IRenderTargetSurface* surface, const Color4f& color) = 0;
    virtual void ClearDepthSurface        (IDepthStencilSurface* surface, float depth) = 0;
    virtual void ClearStencilSurface      (IDepthStencilSurface* surface, unsigned char value) = 0;
    virtual void Clear                    (IFrameBuffer* buffer, size_t clear_flags, const Color4f& color, float depth, unsigned char stencil) = 0;
    virtual void Clear                    (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil) = 0;

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
