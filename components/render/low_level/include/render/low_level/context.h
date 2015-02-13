#ifndef RENDER_LOW_LEVEL_CONTEXT_HEADER
#define RENDER_LOW_LEVEL_CONTEXT_HEADER

#include <exception>

#include <render/low_level/buffer.h>
#include <render/low_level/state.h>
#include <render/low_level/view.h>
#include <render/low_level/shader.h>
#include <render/low_level/query.h>
#include <render/low_level/state_block.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ClearFlag
{
  ClearFlag_RenderTarget = 1, //������� ������ ����� (render-target)
  ClearFlag_Depth        = 2, //������� ����� �������
  ClearFlag_Stencil      = 4, //������� ����� ���������
  ClearFlag_ViewportOnly = 8, //������� ������ ������� ������

  ClearFlag_DepthStencil = ClearFlag_Depth | ClearFlag_Stencil,
  ClearFlag_All          = ClearFlag_RenderTarget | ClearFlag_DepthStencil
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
///������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ICommandList: virtual public IObject {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDeviceContext: virtual public IObject
{
  public:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������� (input-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void          ISSetInputLayout  (IInputLayout* state) = 0;
    virtual void          ISSetVertexBuffer (unsigned int vertex_buffer_slot,  IBuffer* buffer) = 0;
    virtual void          ISSetIndexBuffer  (IBuffer* buffer) = 0;
    virtual IInputLayout* ISGetInputLayout  () = 0;
    virtual IBuffer*      ISGetVertexBuffer (unsigned int vertex_buffer_slot) = 0;
    virtual IBuffer*      ISGetIndexBuffer  () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� �������� (shader-stage)
/// � ������ ������������ ������������ ���������� � ��������� � ������� - ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void                      SSSetProgram                 (IProgram* program) = 0;
    virtual void                      SSSetProgramParametersLayout (IProgramParametersLayout* parameters_layout) = 0;
    virtual void                      SSSetSampler                 (unsigned int sampler_slot, ISamplerState* state) = 0;
    virtual void                      SSSetTexture                 (unsigned int sampler_slot, ITexture* texture) = 0;
    virtual void                      SSSetConstantBuffer          (unsigned int buffer_slot, IBuffer* buffer) = 0;
    virtual IProgramParametersLayout* SSGetProgramParametersLayout () = 0;
    virtual IProgram*                 SSGetProgram                 () = 0;
    virtual ISamplerState*            SSGetSampler                 (unsigned int sampler_slot) = 0;
    virtual ITexture*                 SSGetTexture                 (unsigned int sampler_slot) = 0;
    virtual IBuffer*                  SSGetConstantBuffer          (unsigned int buffer_slot) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� (rasterizer-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void              RSSetState    (IRasterizerState* state) = 0;
    virtual void              RSSetViewport (unsigned int render_target_slot, const Viewport& viewport) = 0;
    virtual void              RSSetScissor  (unsigned int render_target_slot, const Rect& scissor_rect) = 0;
    virtual IRasterizerState* RSGetState    () = 0;
    virtual const Viewport&   RSGetViewport (unsigned int render_target_slot) = 0;
    virtual const Rect&       RSGetScissor  (unsigned int render_target_slot) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ������� (output-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void                OSSetBlendState        (IBlendState* state) = 0;
    virtual void                OSSetDepthStencilState (IDepthStencilState* state) = 0;
    virtual void                OSSetStencilReference  (unsigned int reference) = 0;
    virtual void                OSSetRenderTargets     (unsigned int views_count, IView** render_target_view, IView* depth_stencil_view) = 0;
    virtual void                OSSetRenderTargetView  (unsigned int render_target_slot, IView* view) = 0;
    virtual void                OSSetDepthStencilView  (IView* view) = 0;
    virtual IBlendState*        OSGetBlendState        () = 0;
    virtual IDepthStencilState* OSGetDepthStencilState () = 0;
    virtual unsigned int        OSGetStencilReference  () = 0;
    virtual IView*              OSGetRenderTargetView  (unsigned int render_target_slot) = 0;
    virtual IView*              OSGetDepthStencilView  () = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void         SOSetTargets      (unsigned int buffers_count, IBuffer** buffers, const unsigned int* offsets) = 0;
    virtual void         SOSetTarget       (unsigned int stream_output_slot, IBuffer* buffer, unsigned int offset) = 0;
    virtual IBuffer*     SOGetTarget       (unsigned int stream_output_slot) = 0;
    virtual unsigned int SOGetTargetOffset (unsigned int stream_output_slot) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void ClearRenderTargetView (unsigned int render_target_slot, const Color4f& color) = 0;
    virtual void ClearDepthStencilView (unsigned int clear_flags, float depth, unsigned char stencil) = 0;
    virtual void ClearViews            (unsigned int clear_flags, unsigned int render_targets_count, const unsigned int* render_target_indices, const Color4f* colors, float depth, unsigned char stencil) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���-������� �������� (���������� ��� ������� � ������� ������� ���������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GenerateMips (ITexture* texture) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void        SetPredication    (IPredicate* predicate, bool predicate_value) = 0;
    virtual IPredicate* GetPredicate      () = 0;
    virtual bool        GetPredicateValue () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void DrawAuto             (PrimitiveType primitive_type) = 0;
    virtual void Draw                 (PrimitiveType primitive_type, unsigned int first_vertex, unsigned int vertices_count) = 0;
    virtual void DrawIndexed          (PrimitiveType primitive_type, unsigned int first_index, unsigned int indices_count, unsigned int base_vertex) = 0;
    virtual void DrawInstanced        (PrimitiveType primitive_type, unsigned int vertex_count_per_instance, unsigned int instance_count, unsigned int first_vertex, unsigned int first_instance_location) = 0;
    virtual void DrawIndexedInstanced (PrimitiveType primitive_type, unsigned int index_count_per_instance, unsigned int instance_count, unsigned int first_index, unsigned int base_vertex, unsigned int first_instance_location) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Flush () = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ICommandList* FinishCommandList  (bool restore_state = 0) = 0;
    virtual void          ExecuteCommandList (ICommandList* list, bool restore_state = false) = 0;
};

}

}

#endif
