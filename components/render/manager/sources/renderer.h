class MaterialImpl;
class EntityImpl;
class ProgramParametersLayout;
class DynamicPrimitive;
class ShaderOptionsCache;
class RectAreaImpl;
class BatchingManager;

template <class T> class DynamicPrimitiveBuffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� / ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct DynamicPrimitiveVertex
{
  math::vec3f position;
  math::vec3f normal;
  math::vec4f color;
  math::vec2f tex_coord;
};

typedef unsigned short                                 DynamicPrimitiveIndex;
typedef DynamicPrimitiveBuffer<DynamicPrimitiveIndex>  DynamicIndexBuffer;
typedef DynamicPrimitiveBuffer<DynamicPrimitiveVertex> DynamicVertexBuffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererPrimitive
{
  MaterialImpl*                       material;         //��������
  render::low_level::IStateBlock*     state_block;      //���� ��������� ���������
  bool                                indexed;          //�������� �� ������ �������� ��������������� ��� ��������� ������ �� ������ ��� ��������
  render::low_level::PrimitiveType    type;             //��� ���������
  size_t                              first;            //������ ������ �������/�������
  size_t                              count;            //���������� ����������
  size_t                              base_vertex;      //������ ������� �������
  size_t                              tags_count;       //���������� ����� ���������
  const size_t*                       tags;             //���� ���������
  const DynamicPrimitiveIndex* const* dynamic_indices;  //��������� �� ������ ������������ ��������
  BatchingManager*                    batching_manager; //�������� ��������
  size_t                              batching_hash;    //��� ������
};

inline size_t get_batching_hash (const RendererPrimitive& p)
{
  return common::crc32 (&p.state_block, sizeof (p.state_block), common::crc32 (&p.type, sizeof (p.type), common::crc32 (&p.base_vertex, sizeof (p.base_vertex))));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererPrimitiveGroup
{
  size_t                   primitives_count; //���������� ���������� � ������
  const RendererPrimitive* primitives;       //���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ������������ ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererDynamicPrimitiveGroup: public RendererPrimitiveGroup
{
  DynamicPrimitive* dynamic_primitive;

  RendererDynamicPrimitiveGroup (const RendererPrimitiveGroup& group, DynamicPrimitive* primitive)
    : RendererPrimitiveGroup (group)
    , dynamic_primitive (primitive)
  {
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererOperation
{
  EntityImpl*                     entity;                   //������
  render::low_level::IStateBlock* state_block;              //���� ��������� �������
  ProgramParametersLayout*        entity_parameters_layout; //������������ ���������� �������
  const RendererPrimitive*        primitive;                //��������
  Program*                        program;                  //��������� (� ������ ����� �������, ���������� � EntityImpl)
  DynamicPrimitive*               dynamic_primitive;        //������������ ��������, ��������������� �������� (����� ���� 0)
  const BoxAreaImpl*              scissor;                  //������� ��������� (����� ���� null)
  size_t                          batching_hash;            //��� ������
};

inline size_t get_batching_hash (const RendererOperation& op)
{
  if (!op.primitive->batching_hash)
    return 0;

  return common::crc32 (&op.scissor, sizeof (op.scissor), common::crc32 (&op.state_block, sizeof (op.state_block),
    common::crc32 (&op.entity_parameters_layout, sizeof (op.entity_parameters_layout), common::crc32 (&op.program, sizeof (op.program), op.primitive->batching_hash))));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererOperationList
{
  size_t                   operations_count; //���������� ��������
  const RendererOperation* operations;       //��������
};
