class MaterialImpl;
class EntityImpl;
class ProgramParametersLayout;
class DynamicPrimitive;
class ShaderOptionsCache;
class RectAreaImpl;

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

typedef DynamicPrimitiveBuffer<unsigned short>         DynamicIndexBuffer;
typedef DynamicPrimitiveBuffer<DynamicPrimitiveVertex> DynamicVertexBuffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererPrimitive
{
  MaterialImpl*                    material;        //��������
  render::low_level::IStateBlock*  state_block;     //���� ��������� ���������
  bool                             indexed;         //�������� �� ������ �������� ��������������� ��� ��������� ������ �� ������ ��� ��������
  render::low_level::PrimitiveType type;            //��� ���������
  size_t                           first;           //������ ������ �������/�������
  size_t                           count;           //���������� ����������
  size_t                           base_vertex;     //������ ������� �������
  size_t                           tags_count;      //���������� ����� ���������
  const size_t*                    tags;            //���� ���������
  const unsigned short*            dynamic_indices; //������������ �������
  DynamicIndexBuffer*              dynamic_ib;      //������������ ��������� �����  
};

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
  EntityImpl*                     entity;                         //������
  render::low_level::IStateBlock* state_block;                    //���� ��������� �������
  ProgramParametersLayout*        entity_parameters_layout;       //������������ ���������� �������
  const RendererPrimitive*        primitive;                      //��������
  DynamicPrimitive*               dynamic_primitive;              //������������ ��������, ��������������� �������� (����� ���� 0)
  ShaderOptionsCache*             shader_options_cache;           //��� ����� �������
  const RectAreaImpl*             scissor;                        //������� ��������� (����� ���� null)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererOperationList
{
  size_t                   operations_count; //���������� ��������
  const RendererOperation* operations;       //��������
};
